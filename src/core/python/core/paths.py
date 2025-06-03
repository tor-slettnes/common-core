'''
Utilities to access application file paths.
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

### Package modules
from ..buildinfo import SETTINGS_DIR, LOCAL_SETTINGS_DIR, ORGANIZATION

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

def install_root() -> FilePath:
    '''
    Obtain installation root folder
    '''

    root, _ = locate_dominating_path('share')
    return root

def package_root(package: str):
    '''
    Obtain root installation folder for a specific Python package
    '''
    package_path = importlib.resources.files(package)
    package_parts = package.split('.')
    while package_parts:
        package_path = parent_path(package_path)
        package_parts.pop()
    return package_path


def python_root() -> FilePath:
    '''
    Obtain root installation folder this Python package
    '''
    return package_root(__package__)


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

    if normfolder := normalized_folder(folder):
        path = settings_path()
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
        searchpath.append(SETTINGS_DIR)

        ### Embedded `settings` folder inside distribution archive (wheel or executable)
        searchpath.append(python_root().joinpath('settings'))

    return normalized_search_path(searchpath)


def settings_path() -> SearchPath:
    '''
    Return list of folders in which to look for configuration files.

    This list may have been ameded via prior calls to `add_to_settings_path()`.
    To obtain the original settings path, use `default_settings_path()`.
    '''

    global _settingspath
    if _settingspath is None:
        if configpath := os.getenv('CONFIGPATH', ''):
            _settingspath = normalized_search_path(configpath)
        else:
            _settingspath = default_settings_path()

    return _settingspath


def normalized_search_path(searchpath: SearchPath) -> list[pathlib.Path]:
    if isinstance(searchpath, str):
        searchpath = searchpath.split(os.pathsep)

    normpath = []
    for folder in searchpath:
        if normfolder := normalized_folder(folder):
            normpath.append(normfolder)

    return normpath

def normalized_folder(folder: FilePath):
    if isinstance(folder, str):
        if os.path.isabs(folder):
            return pathlib.Path(folder)
        else:
            _, normalized = locate_dominating_path(folder)
            return normalized

    elif isinstance(folder, pathlib.Path):
        return folder.absolute()

    else:
        return None

def locate_dominating_path(name: FilePath,
                           start: FilePath|None = None,
                           ) -> tuple[FilePath, FilePath]:


    base = start or program_path()
    previous = None

    while not base.joinpath(name).exists():
        if base == previous:
            return None, None
        previous = base
        base = parent_path(base)

    return base, base.joinpath(name)

def parent_path(path: FilePath):
    try:
        return path.parent
    except AttributeError:
        return path.joinpath("..").resolve()
