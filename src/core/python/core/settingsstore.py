#!/usr/bin/python3
#===============================================================================
## @file settingsstore.py
## @brief Obtain settings from JSON file(s)
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .jsonreader import JsonReader
from .paths      import normalizedSearchPath, settingsPath, FilePath, SearchPath


### Standard Python modules
from typing     import Sequence, Optional, Union
import os
import os.path
import sys
import platform
import re
import pathlib

FilePath  = str
FilePaths = Sequence[FilePath]


class SettingsStore (JsonReader):
    '''A specialized dictionary containing configuration settings, loaded from one
    or more JSON file(s) located within specific settings folders.

    Any script-style or C++-style comments (prefixed with either `#` or `//`)
    are strippped off before parsing.

    See `__init__()` for a description of which folder(s) are used.
    '''

    ## Set this to override default search path
    search_path = settingsPath()

    def __init__(self,
                 filenames  : FilePath|FilePaths = [],
                 searchpath : FilePaths = None):

        '''Initialize a new SettingsStore instance from the specified JSON file(s).
        Any script-style or C++-style comments (prefixed with either `#` or `//`)
        are strippped off before parsing.

        @param[in] filenames
           A single JSON file or a list of JSON files from which to load initial
           settings.  If a `.json` suffix is missing, it is added.

        @param[in] searchpath
           A list of directories in which to look for relative filenames. Each
           directory name may be absolute or relative. In the latter case, an
           upward search for that (dominating) folder name starts at the
           installation folder where this module is located, and ends at the
           root of the filesystem.

        If `searchpath` is not not provided, it is obtained from the environment
        variable `CONFIGPATH`, where folder names are presumed to be separated
        by the OS-specific path separator (`:` on UNIX, `;` on Windows).

        If `CONFIGPATH` is not defined, the default search path comprises:

        * a per-unit configuration folder (`/etc/cc` on UNIX or `C:/CC` on
          Windows)

        * an application-provided default settings folder (`share/cc/settings`),
          presumed relative to the top-level folder where the software is
          installed.

        For example, consider a deployment where this module is located
        somewhere inside `/usr`, and default configurations are stored in JSON
        files within the folder `/usr/share/cc/settings/`.  Let's say you create
        a SettingsStore instance as follows:

        ```
        my_settings = SettingsStore(
                          filenames = ['my_settings.json', 'factory_settings.json'])
        ```

        Since `searchpath` is not explicitly provided, the default value
        `['/etc/cc', 'share/cc/settings']` is used.  Settings are then loaded
        and merged in from whichever of the following paths exist, in turn:

        1. `/etc/cc/my_settings.json`
        2. `/usr/share/cc/settings/my_settings.json`
        3. `/etc/cc/factory_settings.json`
        4. `/usr/share/cc/settings/factory_settings.json`

        See also `load_settings()`.

        '''

        self.filepath   = None
        if searchpath is not None:
            self.searchpath = normalizedSearchPath(searchpath)
        else:
            self.searchpath = type(self).search_path

        if isinstance(filenames, str):
            self.filenames = (filenames,)
        elif isinstance(filenames, (list, tuple)):
            self.filenames = filenames
        else:
            raise ValueError("One or more filename must be provided.")

        for filename in self.filenames:
            self.load_settings(filename)


    def load_settings(self,
                      filename   : FilePath,
                      searchpath : Optional[FilePaths] = None):

        '''Load in settings from the specified file.

        @param[in] filename
            Settings file to load. The name may be absolute or relative,
            see `__init__()` for details.

        Any comments starting with `#` or `//` until the end of the line are
        stripped away prior to JSON parsing.
        '''

        if searchpath:
            search = normalizedSearchPath(searchpath)
        else:
            search = self.searchpath

        for filepath in self._settingsFilePaths(filename, search):
            self.merge_file(filepath)


    def merge_file(self, filepath: FilePath):
        '''Merge in settings from the specified file.

        @param[in] filepath
            Settings file to merge. The name may be absolute or relative,
            see `__init__()` for details.
        '''
        try:
            super().merge_file(filepath)
        except EnvironmentError as e:
            pass
        else:
            if not self.filepath:
                self.filepath = filepath



    @classmethod
    def _settingsFilePaths(cls,
                           basename : FilePath,
                           searchpath):
        stem, ext = os.path.splitext(basename)
        if not ext:
            basename += ".json"

        filepaths = []

        if os.path.isabs(basename):
            filepaths.append(pathlib.Path(basename))
        else:
            for folder in searchpath:
                filepath = folder.joinpath(basename)
                if filepath.is_file():
                    filepaths.append(filepath)

        return filepaths
