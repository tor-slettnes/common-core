#!/usr/bin/python3
#===============================================================================
## @file settingsstore.py
## @brief Obtain settings from JSON file(s)
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .jsonreader import JsonReader
from .paths import normalizedSearchPath, settingsPath, FilePath, SearchPath


### Standard Python modules
from typing import Sequence, Optional, Union
from importlib.resources.abc import Traversable
import yaml
import os
import os.path
import sys
import platform
import re
import pathlib

FilePath  = str | Traversable
FilePaths = Sequence[FilePath]


class SettingsStore (dict):
    '''A specialized dictionary containing configuration settings, loaded from one
    or more JSON and/or YAML file(s) located within specific settings folders.

    Settings may be loaded on initialization (see :func:`__init__`) or
    subsequently by invoking :func:`load_settings`.

    '''

    ## Set this to override default search path
    search_path = settingsPath()

    parser_map = {
        '.json': JsonReader.parse_text,
        '.yaml': yaml.safe_load
    }

    def __init__(self,
                 filenames  : FilePath|FilePaths = [],
                 searchpath : FilePaths = None):

        '''Initialize a new SettingsStore instance from the specified JSON
        and/or YAML file(s), located relative to one or more folders in
        `searchpath`.  See `:func:load_settings` for details.

        '''

        self.filepath   = None
        if searchpath is not None:
            self.searchpath = normalizedSearchPath(searchpath)
        else:
            self.searchpath = type(self).search_path

        if isinstance(filenames, (str, Traversable)):
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

        '''Load values from the specified JSON and/or YAML file, if found.
        Values are merged in recursively, with precedence given to those already
        in the store.  To replace existing values, first invoke `.clear()`.

        @param[in] filename
            Absolute or relative path to a JSON or YAML file from which to load
            additional settings.

            If the name is relative, it is resolved with respect to each folder
            in `settingspath`, resulting in multiple candidate paths.

            If the name ends in `.json` or `.yaml` the corresponding parser is
            used; otherwise each suffix is in turn appended to each candidate
            path.

            If one or more of the resulting candidate path(s) exist on the
            filesystem, they are consecutively parsed and merged in with the
            existing settings.

        @param[in] searchpath
           A list of directories in which to look for relative filenames. Each
           directory name may in turn be absolute or relative. In the latter
           case, an upward search for that (dominating) folder name starts at
           the installation folder where this application module is located, and
           ends at the root of the filesystem.

           If not provided, it is obtained from the environment variable
           `CONFIGPATH`, where folder names are presumed to be separated by the
           OS-specific path separator (`:` on UNIX, `;` on Windows).

           If CONFIGPPATH is not defined, the default search path comprises:

           * a machine-specific configuration folder (`/etc/common-core` on UNIX
             or `c:\\common-core\\config` on Windows)

           * an application-provided default settings folder
             (`share/common-core/settings`), relative to the top-level folder
             where this application is installed.

           * a `settings` folder directly within an archive (such as a Python Wheel).

        Any comments starting with `#` or `//` until the end of the line are
        stripped away prior to JSON parsing.  The YAML parser natively supports
        embedded comments starting with `#`.

        For example, consider a deployment where this module is located
        somewhere inside `/usr`, and default configurations are stored in YAML
        files within the folder `/usr/share/common-core/settings/`.  Let's say
        you create a SettingsStore instance as follows:

        ```
        my_settings = SettingsStore()
        my_setings.load('my_settings')
        ```

        Since `searchpath` is not explicitly provided, the default value
        `['/etc/common-core', 'share/common-core/settings']` is used.  Settings
        are then loaded and merged in from whichever of the following paths
        exist, in turn:

        1. `/etc/common-core/my_settings.json`
        2. `/etc/common-core/my_settings.yaml`
        3. `/usr/share/common-core/settings/my_settings.json`
        4. `/usr/share/common-core/settings/my_settings.yaml`

        '''

        if searchpath:
            search = normalizedSearchPath(searchpath)
        else:
            search = self.searchpath

        filepaths = self._settingsFilePaths(filename, search)
        for filepath in filepaths:
            self.merge_file(filepath)


    def merge_file(self, filepath: FilePath):
        '''Merge in settings from the specified file.

        @param[in] filepath
            Settings file to merge. The name may be absolute or relative,
            see `load_settings()` for details.
        '''

        if isinstance(filepath, str):
            filepath = pathlib.Path(filepath)

        try:
            parser = type(self).parser_map[filepath.suffix]

            with filepath.open() as fp:
                text = fp.read()

        except KeyError as e:
            raise KeyError("Unsupported settings filename suffix %r"%(filepath,))

        except EnvironmentError as e:
            pass

        else:
            settings = parser(text)
            self.merge_settings(settings)

    def merge_settings(self, settings: dict):
        '''Merge in the specified settings'''

        type(self)._recursive_merge(self, settings)


    @classmethod
    def _recursive_merge(cls,
                         base   : dict,
                         update : dict):

        for (key, value) in update.items():
            basevalue = base.get(key)

            if isinstance(value, dict) and isinstance(basevalue, dict):
                cls._recursive_merge(basevalue, value)
            # elif isinstance(value, (list, tuple)) and isinstance(basevalue, (list, tuple)):
            #     base[key].extend(value)
            else:
                base.setdefault(key, value)


    @classmethod
    def _settingsFilePaths(cls,
                           basename : FilePath,
                           searchpath: FilePaths):

        if isinstance(basename, str):
            basename = pathlib.Path(basename)

        if basename.suffix in cls.parser_map:
            basenames = [basename]
        else:
            basenames = [basename.with_suffix(suffix)
                         for suffix in cls.parser_map]

        filepaths = []

        if basename.is_absolute():
            filepaths.extend(basenames)
        else:
            for folder in searchpath:
                for name in basenames:
                    filepath = folder.joinpath(name)
                    if filepath.is_file():
                        filepaths.append(filepath)

        return filepaths
