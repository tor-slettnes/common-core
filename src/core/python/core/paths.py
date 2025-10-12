'''
Utilities to access application file paths.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Package modules
from ..buildinfo import SETTINGS_DIR, LOCAL_SETTINGS_DIR, SHARED_DATA_DIR, INSTALL_ROOT, ORGANIZATION

### Standard python modules
import enum
import os
import os.path
import platform
import pathlib
import zipfile
import sys
import importlib.resources
from typing import Sequence, Optional

FilePath        = pathlib.PurePath | zipfile.Path
SearchPath      = Sequence[FilePath]

FilePathInput   = FilePath | str
SearchPathInput = Sequence[FilePathInput]

ModuleName      = str

_settingspaths: dict[str, SearchPath] = {}


def program_name() -> str:
    return os.path.splitext(os.path.basename(sys.argv[0]))[0]


def program_path() -> FilePath:
    return pathlib.Path('.').absolute().joinpath(sys.argv[0])


def install_root(fallback: FilePathInput = INSTALL_ROOT) -> FilePath:
    '''
    Obtain installation root folder
    '''
    if dominating_parent := locate_dominating_parent(SHARED_DATA_DIR, python_root()):
        return dominating_parent
    else:
        return pathlib.Path(fallback)


def python_root() -> FilePath:
    '''
    Obtain root installation folder this Python package
    '''
    return package_root(__package__)


def package_root(package: str) -> FilePath:
    '''
    Obtain root installation folder for a specific Python package
    '''
    package_path = importlib.resources.files(package)
    package_parts = package.split('.')
    while package_parts:
        package_path = package_path.parent
        package_parts.pop()
    return package_path


def package_dir(package: str) -> FilePath:
    '''
    Obtain installation folder for a specific Python package
    '''

    if package in ('', '.'):
        return python_root()
    else:
        return importlib.resources.files(package)


def shared_data_dir() -> FilePath:
    return install_root() / SHARED_DATA_DIR;


def add_to_settings_path(
        folder: FilePathInput,
        prepend: bool = False,
        preinstalled: bool = False) -> bool:

    '''
    Add a folder to search path for settings files.

    @param folder
        Folder to add. If this is a relative path, it is determined relative to
        the intallation root (e.g. `/usr`).

    @param prepend
        Insert the folder in the beginning rather than end of the search path.

    @param preinstalled
        Add the folder to `preinstalled_settings_path()` rather than
        `local_settings_path()`. Local settings are loaded before and thus take
        precendence over preinstalled ones. See also
        `cc.core.settingsstore.SettingsStore.save_delta()` for other
        implications of choosing one over the other.

    @return
        `True` if the search path was modified, `False` otherwise.
    '''

    normfolder = combine_paths(install_root(), folder)
    if normfolder.is_dir():
        path = (preinstalled_settings_path() if preinstalled
                else local_settings_path())

        if normfolder in path:
            return False
        elif prepend:
            path.insert(0, normfolder)
            return True
        else:
            path.append(normfolder)
            return True
    else:
        return False


def settings_path(
        package: str|None = None) -> SearchPath:
    '''
    Return list of folders in which to look for configuration files.

    If provided, the `package` argument is expanded to the folder in which the
    corresponding Python package is installed, and appended to the result.
    '''
    return local_settings_path() + preinstalled_settings_path(package)


def preinstalled_settings_path(package: str|None = None) -> SearchPath:
    '''
    Obtain a list of folders in which to find settings files containing
    preinstalled defaults.  See `cc.core.settingsstore.SettingsStore()` for
    details on local overrides vs. preinstalled defaults.

    The return value is a list of `pathlib.Path()` instances, comprising:

    * A shared settings folder (`share/common-core/settings`, relative to
      `install_root()` of this package).

    * a `settings` folder directly within `python_root()` (i.e. the top-level
      Python modules folder in which this package exists, such as
      `lib/pythonX.XX/site-packages`).

    * The folder in which the Python `package` provided to `__init__`, if any,
      is installed.
    '''

    PREINSTALLED_PATH_SELECTOR = "preinstalled"

    try:
        return _settingspaths[PREINSTALLED_PATH_SELECTOR]
    except KeyError:
        searchpath = []

        ### Default settings installed from release package
        searchpath.append(install_root() / SETTINGS_DIR)

        ### Embedded `settings` folder inside distribution archive (wheel or executable)
        searchpath.append(python_root() / 'settings')

        if package is not None:
            searchpath.append(package_dir(package))

        _settingspaths[PREINSTALLED_PATH_SELECTOR] = searchpath
        return searchpath



def local_settings_path() -> SearchPath:
    '''
    Obtain a list of folders in which to look up host-specific configuration
    files.  See `cc.core.settingsstore.SettingsStore()` for details on local
    settings overrides vs. preinstalled defaults.

    The return value is a list of `pathlib.Path()` instances, as follows:

    * If the environment variable `CONFIGPATH` is set, it is presumed to contain
      directory names delimited by the OS specific path separator (`:` on UNIX,
      `;` on Windows). These are included, with relative names resolved with
      respect to `install_root()`.

    * Otherwise, the following directories are included:

      - `$HOME/.config/common-core`, if and only if `$HOME` is defined,
        and on UNIX, if not running as a `root` user.

      - A machine-specific settings directory (`/etc/common-core` on UNIX or
        `c:\\common-core\\config` on Windows).
    '''

    LOCAL_PATH_SELECTOR = "local"

    try:
        return _settingspaths[LOCAL_PATH_SELECTOR]

    except KeyError:
        if configpath := os.getenv('CONFIGPATH', ''):
            searchpath = normalized_search_path(configpath)
        else:
            searchpath = []

            ### User-specific settings
            if userdir := user_settings_dir():
                searchpath.append(userdir)

            ### Locally managed (host specific) settings
            if hostdir := host_settings_dir():
                searchpath.append(hostdir)

        _settingspaths[LOCAL_PATH_SELECTOR] = searchpath
        return searchpath


def user_settings_dir() -> FilePath:
    '''
    Return the user-specific settings folder, `$HOME/.config/common-core`.

    Returns None if if the environment variable `$HOME` is not defined, or on
    UNIX, if the effective user ID of this process is 0 (root).
    '''

    if not is_super_user():
        if homedir := os.getenv("HOME"):
            configdir = pathlib.Path(homedir) / ".config"
            if configdir.is_dir():
                return configdir / ORGANIZATION

    return None


def host_settings_dir() -> FilePath:
    '''
    Return the host-specific settings folder (`/etc/common-core` on UNIX or
    `c:\\common-core\\config` on Windows).
    '''

    return install_root() / LOCAL_SETTINGS_DIR


def normalized_search_path(searchpath: SearchPath,
                           base_directory: FilePathInput|None = None,
                           package: str|None = None) -> list[pathlib.Path]:

    if isinstance(searchpath, str):
        searchpath = searchpath.split(os.pathsep)

    if base_directory is None:
        base_directory = install_root()

    normpath = []
    for folder in searchpath:
        candidate = combine_paths(base_directory, folder)
        if candidate.is_dir():
            normpath.append(candidate)

    return normpath


def locate_dominating_parent(target: FilePathInput,
                             start: FilePathInput,
                             ) -> FilePath|None:
    '''
    Search upwards from the specified `start` directory until the relative
    path `target` is found inside, or until the filesystem root has been reached
    without any results.

    Returns the parent directory in which `target` is found if any, or `None`.
    '''

    base = (start
            if isinstance(start, FilePath)
            else pathlib.Path(start))

    previous = None

    while not base.joinpath(target).exists():
        if base == previous:
            return None
        previous = base
        base = base.parent

    return base


def combine_paths(first: FilePathInput, second: FilePathInput) -> FilePath:
    combined = as_path(first)
    if isinstance(second, zipfile.Path):
        return second
    else:
        return first / second


def as_path(input: FilePathInput):
    if isinstance(input, FilePath):
        return input

    elif isinstance(input, str):
         return pathlib.Path(input)

    else:
        raise TypeError(f"Input value cannot be converted to path: {input!r}")

def is_super_user() -> bool:
    '''
    Returns True iff this process is running with superuser (root) privileges on UNIX.
    '''
    try:
        euid = os.geteuid()
    except AttributeError:
        return False
    else:
        return euid == 0

