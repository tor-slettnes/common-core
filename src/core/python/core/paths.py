'''
Utilities to access application file paths.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Package modules
from ..buildinfo import SETTINGS_DIR, LOCAL_SETTINGS_DIR, SHARED_DATA_DIR, INSTALL_ROOT, ORGANIZATION

### Standard python modules
from importlib.resources.abc import Traversable
import os
import os.path
import platform
import pathlib
import sys
import importlib.resources
from typing import Sequence, Optional

FilePath   = str | Traversable
SearchPath = Sequence[FilePath]
ModuleName = str

_settingspath = None


def program_name() -> str:
    return os.path.splitext(os.path.basename(sys.argv[0]))[0]


def program_path() -> FilePath:
    return pathlib.Path('.').absolute().joinpath(sys.argv[0])


def install_root(fallback: FilePath = INSTALL_ROOT) -> FilePath:
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
        package_path = parent_path(package_path)
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


def add_to_settings_path(folder: FilePath,
                         prepend: bool) -> bool:
    '''
    Add a folder to search path for settings files.

    @param folder
        Folder to add. If this is a relative path, it is determined relative to
        the intallation root (e.g. `/usr`).

    @param prepend
        Insert the folder in the beginning rather than end of the search path.

    @return
        `True` if the search path was modified, `False` otherwise.
    '''

    if normfolder := normalized_folder(folder, python_root()):
        path = mutable_settings_path()
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


def default_settings_path(
        package: str|None = None,
        include_local: bool = True,
        include_global: bool = True,
        ) -> SearchPath:
    '''
    Obtain the built-in default list of folders in which to look for
    configuration files.

    If `include_local` is set, this list comprises:
    * `$HOME/.config/common-core`, only if `$HOME` is defined.
    * A machine-specific settings directory (`/etc/common-core` on UNIX or
      `c:\\common-core\\config` on Windows).

    Additionally, if `include_global` is set, the following folders are added:
    * An application-provided settings folder (`share/common-core/settings`,
      relative to installation root of this package).
    * a `settings` folder directly within this distriution archive (`.whl`)
      if any.
    '''

    searchpath = []

    if include_local:
        ### User-specific settings
        if homedir := os.getenv("HOME"):
            configdir = os.path.join(homedir, ".config")
            if os.path.isdir(configdir):
                searchpath.append(os.path.join(configdir, ORGANIZATION))

        ### Locally managed (host specific) settings
        searchpath.append(LOCAL_SETTINGS_DIR)

    if include_global:
        ### Default settings installed from release package
        searchpath.append(install_root() / SETTINGS_DIR)

        ### Embedded `settings` folder inside distribution archive (wheel or executable)
        searchpath.append(python_root() / 'settings')

        if package is not None:
            searchpath.append(package_dir(package))

    return normalized_search_path(searchpath)


def mutable_settings_path() -> SearchPath:
    '''
    Return a mutable list of folders in which to look for configuration files.

    This list may have been ameded via prior calls to `add_to_settings_path()`.
    To obtain the original settings path, use `default_settings_path()`.

    To obtain a final settings path, optionally tailored for a specific Python
    namespace/package, use `settings_path()`.
    '''

    global _settingspath
    if _settingspath is None:
        if configpath := os.getenv('CONFIGPATH', ''):
            _settingspath = normalized_search_path(configpath)
        else:
            _settingspath = default_settings_path()

    return _settingspath


def settings_path(
        package: str|None = None) -> SearchPath:
    '''
    Return list of folders in which to look for configuration files.

    If provided, the `package` argument is expanded to the folder in which the
    corresponding Python package is installed, and appended to the result.
    '''

    settingspath = mutable_settings_path().copy()
    if package is not None:
        settingspath.append(package_dir(package))
    return settingspath


def normalized_search_path(searchpath: SearchPath,
                           package: str|None = None) -> list[pathlib.Path]:
    if isinstance(searchpath, str):
        searchpath = searchpath.split(os.pathsep)

    normpath = []
    for folder in searchpath:
        if normfolder := normalized_folder(folder, python_root()):
            normpath.append(normfolder)

    return normpath


def normalized_folder(folder: FilePath,
                      start: FilePath):

    if isinstance(folder, str):
        if os.path.isabs(folder):
            return pathlib.Path(folder)

        elif dominating_parent := locate_dominating_parent(folder, start):
            return dominating_parent / folder

    elif isinstance(folder, pathlib.Path):
        return folder.absolute()

    else:
        return None


def locate_dominating_parent(target: FilePath,
                             start: FilePath,
                             ) -> FilePath:
    '''
    Search upwards from the specified `start` directory until the relative
    path `target` is found inside, or until the filesystem root has been reached
    without any results.

    Returns the parent directory in which `target` is found if any, or `None`.
    '''

    base = start
    previous = None

    while not base.joinpath(target).exists():
        if base == previous:
            return None
        previous = base
        base = parent_path(base)

    return base


def parent_path(path: FilePath):
    try:
        return path.parent
    except AttributeError:
        return path.joinpath("..").resolve()
