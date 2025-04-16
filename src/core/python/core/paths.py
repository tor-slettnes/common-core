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

def programName() -> str:
    return os.path.splitext(os.path.basename(sys.argv[0]))[0]

def installRoot() -> FilePath:
    '''
    Obtain installation root folder
    '''

    installRoot, _ = locateDominatingPath('share')
    return installRoot

def packageRoot(package: str):
    '''
    Obtain root installation folder for a specific Python package
    '''
    package_path = importlib.resources.files(package)
    package_parts = package.split('.')
    while package_parts:
        package_path = parent_path(package_path)
        package_parts.pop()
    return package_path


def pythonRoot() -> FilePath:
    '''
    Obtain root installation folder this Python package
    '''
    return packageRoot(__package__)


def addSettingsFolder(folder: FilePath,
                      prepend: bool) -> bool:
    '''
    Add a folder to search path for settings files.

    @param folder
        Folder to add. If this is a relative path, it is determined relative to
        the intallation root (e.g. `/usr`).

    @param prepend
        Insert the folder in the beginning rather than end of the search path.

    @return
        True if the search path was modified, False otherwise.
    '''

    if normfolder := normalizedFolder(folder):
        path = settingsPath()
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


def defaultSettingsPath() -> SearchPath:
    '''
    Obtain the built-in default list of folders in which to look for
    configuration files. This list comprises:

    * `$HOME/.config/common-core` if `$HOME` is defined
    * A machine-specific settings directory (`/etc/common-core` on UNIX
      or `c:\\common-core\\config` on Windows)
    * An application-provided settings folder (`share/common-core/settings`,
      relative to installation root of this package)
    * a `settings` folder directly within this distriution archive (`.whl`),
      if any.
    '''

    searchpath = []
    if homedir := os.getenv("HOME"):
        configdir = os.path.join(homedir, ".config")
        if os.path.isdir(configdir):
            searchpath.append(os.path.join(configdir, ORGANIZATION))

    searchpath.extend([
        LOCAL_SETTINGS_DIR, # Local (host specific) settings
        SETTINGS_DIR,       # Supplied defaults
        'settings'          # Inside virtualenv and/or `.whl` container
    ])

    return normalizedSearchPath(searchpath)


def settingsPath() -> SearchPath:
    '''
    Return list of folders in which to look for configuration files.

    This list may have been ameded via prior calls to `addSettingsFolder()`.
    To obtain the original settings path, use `defaultSettingsPath()`.
    '''

    global _settingspath
    if _settingspath is None:
        if configpath := os.getenv('CONFIGPATH', ''):
            _settingspath = normalizedSearchPath(configpath)
        else:
            _settingspath = defaultSettingsPath()

    return _settingspath


def normalizedSearchPath(searchpath: SearchPath) -> list[pathlib.Path]:
    if isinstance(searchpath, str):
        searchpath = searchpath.split(os.pathsep)

    normpath = []
    for folder in searchpath:
        if normfolder := normalizedFolder(folder):
            normpath.append(normfolder)

    return normpath

def normalizedFolder(folder: FilePath):
    if isinstance(folder, str):
        if os.path.isabs(folder):
            return pathlib.Path(folder)
        else:
            _, normalized = locateDominatingPath(folder)
            return normalized

    elif isinstance(folder, pathlib.Path):
        return folder.absolute()

    else:
        return None

def locateDominatingPath(name: FilePath):
    base = importlib.resources.files(__package__)
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
