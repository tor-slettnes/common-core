#!/usr/bin/python3
#===============================================================================
## @file paths.py
## @brief Path related utilities
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import os
import os.path
import platform

from typing import Sequence, Optional
FilePath   = str
SearchPath = Sequence[FilePath]

def installRoot() -> FilePath:
    '''Obtain installation root folder'''

    installRoot, pythonRoot = locateDominatingPath('share/python')
    return installRoot

def pythonRoot() -> FilePath:
    '''Obtain installation folder for Python modules'''

    installRoot, pythonRoot = locateDominatingPath('share/python')
    return pythonRoot

def settingsPath() -> SearchPath:
    try:
        searchpath = os.getenv('CONFIGPATH').split(os.pathsep)
    except AttributeError:
        searchpath = [
            ('/etc/cc', 'C:\\CC\\Config')[platform.system() == 'Windows'],
            'share/settings'
        ]

    return normalizedSearchPath(searchpath)

def normalizedSearchPath(searchpath: SearchPath):
    if isinstance(searchpath, str):
        searchpath = os.pathsep.split(searchpath)

    normpath = []
    for folder in searchpath:
        if not os.path.isabs(folder):
            base, folder = locateDominatingPath(folder)

        if folder:
            normpath.append(folder)

    return normpath

def locateDominatingPath(name: FilePath,
                         start: FilePath = os.path.dirname(__file__)):

    base = os.path.normpath(start)
    previous = None
    while not os.path.exists(os.path.join(base, name)):
        if base == previous:
            return None, None
        previous = base
        base = os.path.dirname(base)

    return base, os.path.normpath(os.path.join(base, name))
