#!/usr/bin/python3
#===============================================================================
## @file paths.py
## @brief Path related utilities
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

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

def programName() -> str:
    return os.path.splitext(os.path.basename(sys.argv[0]))[0]

def installRoot() -> FilePath:
    '''
    Obtain installation root folder
    '''

    installRoot, _ = locateDominatingPath('share')
    return installRoot

def pythonRoot() -> FilePath:
    '''
    Obtain installation folder for Python modules
    '''

    package_path = importlib.resources.files(__package__)
    package_parts = __package__.split('.')
    while package_parts:
        package_path = parent_path(package_path)
        package_parts.pop()
    return package_path


def settingsPath() -> SearchPath:
    try:
        searchpath = os.getenv('CONFIGPATH').split(os.pathsep)
    except AttributeError:
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
