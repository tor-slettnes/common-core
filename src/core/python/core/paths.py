#!/usr/bin/python3
#===============================================================================
## @file paths.py
## @brief Path related utilities
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Package modules
from ..buildinfo import SETTINGS_DIR

### Standard python modules
from importlib.resources.abc import Traversable
import os
import os.path
import platform
import pathlib
import importlib.resources
from typing import Sequence, Optional

FilePath   = str | Traversable
SearchPath = Sequence[FilePath]
ModuleName = str

def installRoot() -> FilePath:
    '''Obtain installation root folder'''

    installRoot, _ = locateDominatingPath('share')
    return installRoot

def pythonRoot() -> FilePath:
    '''Obtain installation folder for Python modules'''

    pythonRoot, _ = locateDominatingPath('core')
    return pythonRoot

def settingsPath() -> SearchPath:
    try:
        searchpath = os.getenv('CONFIGPATH').split(os.pathsep)
    except AttributeError:
        searchpath = [
            ('/etc/cc', 'C:\\CC\\Config')[platform.system() == 'Windows'], # Local overrides
            SETTINGS_DIR, # Package defaults
            'settings'    # Inside virtualenv/`.whl` container
        ]

    return normalizedSearchPath(searchpath)

def normalizedSearchPath(searchpath: SearchPath) -> list[pathlib.Path]:
    if isinstance(searchpath, str):
        searchpath = searchpath.split(os.pathsep)

    normpath = []
    for folder in searchpath:
        if isinstance(folder, str):
            if os.path.isabs(folder):
                folder = pathlib.Path(folder)
            else:
                _, folder = locateDominatingPath(folder)

        elif isinstance(folder, pathlib.Path):
            folder = folder.absolute()

        if folder:
            normpath.append(folder)

    return normpath

def locateDominatingPath(name: FilePath,
                         start: ModuleName = __spec__.name):

    package = start.rsplit('.', 1)[0]
    base = importlib.resources.files(package)
    candidate = base.joinpath(name)
    previous = None

    while not base.joinpath(name).exists():
        if base == previous:
            return None, None
        previous = base
        base = base.parent
        candidate = base.joinpath(name)

    return base, candidate
