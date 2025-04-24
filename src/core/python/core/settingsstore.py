'''
Obtain settings from `.yaml` and/or `.json` files
'''

__all__ = ['Endpoint']
__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


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
    '''
    A specialized dictionary containing configuration settings, loaded from
    one or more JSON and/or YAML file(s) located within specific settings
    folders.

    Settings may be loaded on initialization (see `__init__()`) or subsequently
    by invoking `load_settings()`.
    '''


    ## Set this to override default search path
    search_path = settingsPath()

    parser_map = {
        '.json': JsonReader.parse_text,
        '.yaml': yaml.safe_load
    }

    def __init__(self,
                 filenames  : FilePath|FilePaths|None = [],
                 searchpath : FilePaths|None = None):

        '''
        Initialize a new SettingsStore instance from the specified JSON
        and/or YAML file(s), located relative to one or more folders in
        `searchpath`.  See `load_settings()` for details.
        '''

        self.filenames = []
        self.filepaths = []
        if searchpath is not None:
            self.searchpath = normalizedSearchPath(searchpath)
        else:
            self.searchpath = type(self).search_path

        if filenames is not None:
            if isinstance(filenames, (str, Traversable)):
                self.load_settings(filenames)

            elif isinstance(filenames, (list, tuple)):
                for filename in filenames:
                    self.load_settings(filename)

            else:
                raise TypeError("Unsupported type for filenames argument: %s"%
                                (type(filenames).__name__,))


    def load_settings(self,
                      filename   : FilePath,
                      searchpath : Optional[FilePaths] = None):

        '''
        Load values from the specified JSON and/or YAML file, if found.
        Values are merged in recursively, with precedence given to those already
        in the store.  To replace existing values, first invoke `.clear()`.

        @param filename:
            Absolute or relative path to a JSON or YAML file from which to load
            additional settings.

        @param searchpath:
             A list of directories in which to look for relative filenames.
             If not provided, it is obtained from the environment variable
             `CONFIGPATH`, or ultimately from predefined folders (see below).

        The `filename` argument is resolved as follows:
        - If the file name is relative, it is resolved with respect to each
          folder in `settingspath`, possibly resulting in multiple candidate
          paths.
        - If the file name ends in `.json` or `.yaml` the corresponding parser
          is used; otherwise each suffix is in turn appended to each candidate
          path.
        - If one or more of the resulting candidate path(s) exist on the
          filesystem, they are consecutively parsed and merged in with the
          existing settings.

        If `searchpath` is not provided, a default search path is obtained as
        follows:
         - If the environment variable `CONFIGPATH` is defined, it is presumed
           to contain folder names delimited by the OS-specific path separator
           (`:` on UNIX, `;` on Windows)
         - Otherwise, the default search path comprises:
           * a user-specific configuration folder (`$HOME/.config/common-core`
             if available, otherwise no default)
           * a machine-specific configuration folder (`/etc/common-core` on UNIX
             or `c:\\common-core\\config` on Windows)
           * an application-provided default settings folder
             (`share/common-core/settings`), relative to the top-level folder
             where this application is installed.
           * a `settings` folder directly within an archive (such as a Python Wheel).

        Each directory name in `searchpath` may be absolute or relative. In the
        latter case, an upward search for that (dominating) folder name starts
        at the installation folder where this application module is located, and
        ends at the root of the filesystem.

        Any comments starting with `#` or `//` until the end of the line are
        stripped away prior to JSON parsing.  The YAML parser natively supports
        embedded comments starting with `#`.

        ### Example

        Consider a deployment where this module is located somewhere inside
        `/usr`, and default configurations are stored in YAML files within the
        folder `/usr/share/common-core/settings`.
        Let's say you create a `SettingsStore` instance as follows:

        ```python
        my_settings = SettingsStore()
        my_setings.load('my_settings')
        ```

        Since `searchpath` is not explicitly provided, the default value
        `['/etc/common-core', 'share/common-core/settings']` is used.  Settings
        are then loaded and merged in from whichever of the following paths
        exist, in turn:

        1. `$HOME/.config/common-core/my_settings.json`
        2. `$HOME/.config/common-core/my_settings.yaml`
        3. `/etc/common-core/my_settings.json`
        4. `/etc/common-core/my_settings.yaml`
        5. `/usr/share/common-core/settings/my_settings.json`
        6. `/usr/share/common-core/settings/my_settings.yaml`
        '''

        if searchpath:
            search = normalizedSearchPath(searchpath)
        else:
            search = self.searchpath

        for filepath in self.find_file_paths(filename, search):
            self.merge_file(filepath)

        self.filenames.append(filename)


    def merge_file(self, filepath: FilePath):
        '''
        Merge in settings from the specified file.

        @param filepath:
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
            self.filepaths.append(filepath)

    def merge_settings(self, settings: dict):
        '''Merge in the specified settings'''

        type(self)._recursive_merge(self, settings)


    def get_value(self,
                  key: str,
                  expected_type: type,
                  default: object|None,
                  raise_invalid_type: bool = False,
                  ) -> object:
        '''
        If the speified key exists and is of the expected type, return the corresponding value.

        @param key
            Settings key

        @param expected_type
            A type object or a tuple of type objects

        @param default
            Value to return if `key` does not exist, or if it an incorrect type
            and `raise_invalid_type` is False.

        @param raise_invalid_type
            If the specified key is found but the corresponding value is not
            a dictionary, raise a TypeError.

        @return The value corresponding to `key` if it exists and is of the
            expected type, otherwise `default`
        '''

        try:
            value = self[key]
            assert(isinstance(value, expected_type))

        except KeyError:
            return default

        except AssertionError:
            if raise_invalid_type:
                raise TypeError(
                    'Setting %r should be a %s, not %s'%
                    (key, expected_type.__name__, type(value).__name__)) from None
            else:
                return default
        else:
            return value


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
    def find_file_paths(cls,
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
