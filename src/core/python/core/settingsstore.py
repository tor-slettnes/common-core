'''
Obtain settings from `.yaml`, '.json', and/or `.ini` files
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


### Modules within package
from .jsonreader import JsonReader
from .ini_reader import INIFileReader
from .paths import FilePath, FilePathInput, SearchPath, \
    normalized_search_path, settings_path, local_settings_path, preinstalled_settings_path

### Standard Python modules
from typing import Sequence, Mapping, Optional, Union, Any
import yaml
import json
import os
import os.path
import sys
import platform
import re
import pathlib

#Type hints
FilePaths      = Sequence[FilePath]
FilePathInputs = Sequence[FilePathInput]

VariantList    = list['Variant']
VariantMap     = dict[str, 'Variant']
Variant        = None | bool | int | float | str | VariantList | VariantMap


class SettingsStore (dict):
    '''
    A specialized dictionary containing configuration settings, loaded from
    one or more JSON, YAML and/or INI file(s) located within specific settings
    folders.

    Settings may be loaded on initialization or subsequently by invoking
    `load_settings()`.  In either case, each provided filename is resolved as
    follows:

    - If the file name is relative, it is resolved with respect to each folder
      in the search path for this instance (see `__init__()`), possibly
      resulting in multiple candidate paths.

    - If the file name ends in '.json', `.yaml` or `.ini` the corresponding
      parser is used. If the file name has no suffix, each of these supported
      suffixes is in turn appended to each candidate path.  Any filename with a
      different/unsupported suffix is ignored.

    - If one or more of the resulting candidate path(s) exist on the
      filesystem, they are consecutively parsed and merged in with the
      existing settings.

    Comments are supported as follows:

    - In JSON files, comments starting with `#` or `//`
      until the end of the line are stripped away prior to parsing.

    - The YAML parser natively supports embedded comments starting with `#`.

    - INI files are parsed with Python's native RawConfigParser, which supports
      comment following `;` or `#`.
    '''

    parser_suffixes \
        =  (JSON_SUFFIX, YAML_SUFFIX, INI_SUFFIX) \
        =  ('.json', '.yaml', '.ini')

    parser_map = {
        JSON_SUFFIX: JsonReader.parse_text,
        YAML_SUFFIX: yaml.safe_load,
        INI_SUFFIX: INIFileReader.parse_text
    }

    def __init__(self,
                 filenames  : FilePathInput|FilePathInputs|None = [],
                 searchpath : FilePathInputs|None = None,
                 package    : str|None = None):

        '''
        Initialize a new SettingsStore instance from the specified file(s),
        located relative to one or more folders in `searchpath` if provided,
        otherwise in the default search path.

        See the `SettingsStore()` class description for details on the
        `filenames` argument.

        Each directory name in `searchpath` may be absolute or relative. In the
        latter case, an upward search for that (dominating) folder name starts
        at the installation folder where this application module is located, and
        ends at the root of the filesystem.

        If no `searchpath` is provided, a default search path is obtained as
        follows:

         - If the environment variable `CONFIGPATH` is defined, it is presumed
           to contain folder names delimited by the OS-specific path separator
           (`:` on UNIX, `;` on Windows)

         - Otherwise, the default search path comprises:
           * a user-specific configuration folder (`$HOME/.config/common-core`
             if available, otherwise no default),
           * a machine-specific configuration folder (`/etc/common-core` on UNIX
             or `c:\\common-core\\config` on Windows),
           * an application-provided default settings folder
             (`share/common-core/settings`), relative to the top-level folder
             where this application is installed,
           * a `settings` folder directly within an archive (such as a Python Wheel),
           * the folder in which the Python package `package` (if specified) is
             installed.

        The `package` argument may be used to supply application-provided
        defaults within the same folder as code modules for a specific Python
        package. To include the folder where the calling module is installed,
        use

          ```python
          my_settings = SettingsStore('my_settings', package=__package__)
          ```

        (This has lower precedence than any `my_settings.{json,yaml,ini}`
        file(s) found earlier in the search path.)
        '''

        self._filenames = []
        self._filepaths = []
        self.package    = package
        if searchpath is not None:
            self.searchpath = normalized_search_path(searchpath)
        else:
            self.searchpath = settings_path(package)

        if filenames is not None:
            if isinstance(filenames, FilePathInput):
                self.load_settings(filenames)

            elif isinstance(filenames, (list, tuple)):
                for filename in filenames:
                    self.load_settings(filename)

            else:
                raise TypeError("Unsupported type for filenames argument: %s"%
                                (type(filenames).__name__,))


    def load_settings(self,
                      filename : FilePathInput):

        '''
        Load values from the specified settings file, if found.  Values are
        merged in recursively, with precedence given to those already in the
        store.  To replace existing values, first invoke `.clear()`.

        @param filename:
            Absolute or relative path to a JSON, YAML or INI file from which to
            load additional settings.  See `__init__()` for details.


        ### Example

        Consider a deployment where this module is located somewhere inside
        `/usr`, and default configurations are stored in YAML files within the
        folder `/usr/share/common-core/settings`.
        Let's say you create a `SettingsStore` instance as follows:

        ```python
        my_settings = SettingsStore('my_settings')
        ```

        Since `searchpath` is not explicitly provided, the default value
        `['/etc/common-core', 'share/common-core/settings']` is used.  Settings
        are then loaded and merged in from whichever of the following paths
        exist, in turn:

        1. `$HOME/.config/common-core/my_settings.json`
        2. `$HOME/.config/common-core/my_settings.yaml`
        3. `$HOME/.config/common-core/my_settings.ini`
        4. `/etc/common-core/my_settings.json`
        5. `/etc/common-core/my_settings.yaml`
        6. `/etc/common-core/my_settings.ini`
        7. `/usr/share/common-core/settings/my_settings.json`
        8. `/usr/share/common-core/settings/my_settings.yaml`
        9. `/usr/share/common-core/settings/my_settings.ini`
        '''

        for filepath in self.find_paths(filename):
            self.merge_file(filepath)

        self._filenames.append(filename)


    def merge_file(self, filepath: FilePathInput):
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
            settings = parser(text) or {}
            self.merge_settings(settings)
            self._filepaths.append(filepath)

    def merge_settings(self, settings: dict):
        '''Merge in the specified settings'''

        type(self)._recursive_merge(self, settings)


    def get_value(self,
                  key: str|Sequence[str|int],
                  expected_type: Union[type]|None = None,
                  default: Variant|None = None,
                  raise_invalid_type: bool = False,
                  raise_missing: bool = False,
                  ) -> Variant:
        '''
        Return the value corresponding to `key`, which may be either a
        string or a sequence of strings and/or integers.  In the latter case,
        `key` denotes a hierarchical path into which to recursively descend,
        dereferenceing intermediate dictionaries and/or lists along the way.

        @param key
            The setting to retrieve, or a sequence of strings and/or integers
            denoting hiearchical path into which to descend. The path may
            comprise a mix of strings and integers, recursively dereferencing
            dictionaries and lists, respectively.

        @param expected_type
            A type or a union of types (normally separated by `|`).

        @param default
            Value to return if `key` does not exist or if its type is not of the
            requested type(s).  If `key` is a hierarchical sequence, this might
            also mean that any of its intermediate components is either missing,
            or that it is not a dictionary or list from which the next component
            (string or int, respectively) might be dereferenced.

        @param raise_invalid_type
            Raise a TypeError if `key` is found but the corresponding value is
            not of the requested type.  In the case where `key` is a sequence of
            strings and/or ints, also raise a TypeError if any of its
            intermediate components exists in the corresponding hierarchical
            position, but is not a dictionary or a list from which that
            component may be dereferenced.

        @param raise_missing
            Raise a KeyError if `key` is missing. In the case where `key` is a
            sequence, also raise a KeyError or ValueError if any of it its
            intermediate components is missing from the dictionary or list in
            the corresponding hierarchical location.

        @return The value corresponding to `key` if it exists and is of the
            expected type, otherwise `default`
        '''

        if isinstance(key, str):
            path = [key]

        elif isinstance(key, Sequence) and len(key) > 0:
            path = list(key)

        else:
            raise ValueError(
                "Settings key must be a string "
                "or a non-empty sequence of strings and/or ints")

        try:
            value = self
            for component in path:
                if isinstance(component, int) and not isinstance(value, (tuple, list)):
                    raise TypeError

                value = value[component]

        except KeyError:
            if raise_missing:
                raise KeyError(
                    f"Settings key {component!r} missing from value: {value}"
                ) from None
            else:
                return default

        except IndexError:
            if raise_missing:
                raise TypeError(
                    f"Settings index {component} out of range for value: {value}"
                ) from None
            else:
                return default

        except TypeError:
            if raise_invalid_type:
                raise TypeError(
                    f"Cannot get settings key/index {component!r} from value: {value!r}"
                ) from None
            else:
                return default

        if expected_type is None or isinstance(value, expected_type):
            return value

        elif raise_invalid_type:
            raise TypeError(
                f"Expected setting {key!r} to be of type {expected_type.__name__}, "
                f"got {type(value).__name__}: {value!r}"
            ) from None
        else:
            return default


    def set(self,
            key: str|Sequence[str],
            value: Variant,
            save: bool = False):
        '''
        Set the specified `key` to `value`.

        @param key
            The hierarchical location of the setting to update, as either a
            string or a sequence of strings. In the latter case, any leading
            components of the settings key are created as needed.

        @param value
            The new or updated value associated with the specified key.

        @param save
            Automatically save the resulting settings store delta in the local
            settings folder.
        '''

        if isinstance(key, str):
            path = []

        elif isinstance(key, Sequence) and len(key) > 0:
            path = list(key)
            key = path.pop()

        else:
            raise ValueError("Settings key must be a string or a non-empty sequence of strings")

        obj = self
        for element in path:
            sub = obj.get(element)
            if not isinstance(sub, dict):
                sub = obj[element] = {}
                obj = sub

        obj[key] = value

        if save and self.filenames:
            self.save_delta()


    def defaults(self,
                 filenames: FilePathInputs|None = None,
                 ) -> 'SettingsStore':
        '''
        Return default settings as loaded from preinstalled settings files
        (relative to folders such as `/usr/share/common-core/settings`, or the
        `settings` folder within the Python distribution itself). In other
        words, exclude any host-specific or user-specific overrides.

        If `filenames` is not provided, use those that were previously loaded
        into this instance via `__init__()` and/or `load_settings()`.
        '''
        return SettingsStore(
            filenames = filenames or self.filenames,
            searchpath = preinstalled_settings_path())


    def save_delta(self,
                   filename: FilePathInput|None = None,
                   skipkeys: bool = False,
                   ensure_ascii: bool = True,
                   check_circular: bool = True,
                   allow_nan: bool = True,
                   indent: int = 4,
                   ):
        '''
        Save the recursive difference between the default settings, loaded
        from the corresponding unqualified filenames as preinstalled with the
        application, and the current values within this instance.

        @param filename
            Filename in which to save the resulting delta, relative to the local
            settings folder (`/etc/common-core` or `$HOME/.config/common-core`,
            whichever is writable by the current user).

        See `help(json.dump)` for information on the additional arguments
        `skipkeys`, `ensure_ascii`, `check_circular`, `allow_nan` and `indent`.
        '''

        args = locals()
        args.update(only_delta = True)
        return type(self).save(**args)


    def save(self,
             filename: FilePathInput|None = None,
             only_delta: bool = False,
             skipkeys: bool = False,
             ensure_ascii: bool = True,
             check_circular: bool = True,
             allow_nan: bool = True,
             indent: int = 4,
             ):
        '''
        Save settings to a JSON file.

        @param filename
            Absolute or relative filename in which to save settings. If it is
            relative, the path is resolved in turn with respect to the local
            settings folders `/etc/common-core` and `$HOME/.config/common-core`,
            whichever is or can be made writable.  A `.json` suffix is appended
            if missing.  If not specified, the first filename from which
            settings were loaded (but with a .`json` suffix) is used.

        @param delta
            Save only the recursive difference between the current settings
            hierarchy and the default settings, obtained from the software
            distribution (i.e, corresponding filenames within
            `/usr/share/common-core/settings`).

        See `help(json.dump)` for information on the additional arguments
        `skipkeys`, `ensure_ascii`, `check_circular`, `allow_nan` and `indent`.
        '''

        if only_delta:
            settings = self.recursive_delta()
        else:
            settings = self

        if not filename:
            try:
                filename = self.filenames[0]
            except IndexError:
                raise RuntimeError('No settings file specified, and none has been loaded')

        for candidate_folder in local_settings_path():
            filepath = candidate_folder.joinpath(filename).with_suffix('.json')
            try:
                os.makedirs(candidate_folder, exist_ok=True)
                fp = filepath.open('w')
            except EnvironmentError as e:
                failure = e
            else:
                with fp:
                    json.dump(settings,
                              fp,
                              skipkeys = skipkeys,
                              ensure_ascii = ensure_ascii,
                              check_circular = check_circular,
                              allow_nan = allow_nan,
                              indent = indent)
                break
        else:
            if failure:
                raise failure


    @property
    def filenames(self) -> FilePaths:
        '''
        Return the original filenames provided to this SettingsStore instance,
        either to the `__init__()` method or subsequently to `load_settings()`.
        These may be relative or absolute, and with or without a filename suffix.
        '''
        return self._filenames

    @property
    def filepaths(self) -> FilePaths:
        '''
        Return the full path names of each settings file that has been loaded
        into this instance.  Only existing file paths are returned.

        Note that this may or may not incluce the host-specific output file into
        which the `save()` method would write these settings.
        '''
        return self._filepaths


    def find_paths(self,
                   basename: FilePathInput,
                   searchpath: FilePathInputs | None = None) -> FilePaths:
        '''
        Find settings files with the specified base name within the
        specified search path, or the default search path for this SettingsStore
        instance.

        @param basename
            Stem (with or without a suffix) of the filename we are looking for.
            If no suffix is provided, each of the supported settings suffixes
            is tried in turn: `.json`, `.yaml`, `.ini`'

        @param searchpath
            A list of directories in which to search. If unspecified, use the
            default search path for this instance.

        @return
            A list of absolute pathnames to matching settings files.
        '''

        return type(self).find_file_paths(basename, searchpath or self.searchpath)


    def recursive_delta(self) -> dict:
        '''
        Return the recursive difference between the default settings as
        preinstalled with the application and this instance.

        See also `defaults()`.
        '''
        return type(self)._recursive_delta(self, self.defaults())


    @classmethod
    def find_file_paths(cls,
                        basename : FilePathInput,
                        searchpath: FilePathInputs) -> FilePaths:
        '''
        Find settings files with the specified base name within the provided
        search path.

        @param basename
            Stem (with or without a suffix) of the filename we are looking for.
            If no suffix is provided, each of the supported settings suffixes
            is tried in turn: `.json`, `.yaml`, '.ini`.

        @param searchpath
            An iterable over folders in which to look for the specified file.

        @return
            A list of absolute pathnames to matching settings files.
        '''

        if isinstance(basename, str):
            basename = pathlib.Path(basename)

        if basename.suffix:
            basenames = [basename]
        else:
            basenames = [basename.with_suffix(suffix)
                         for suffix in cls.parser_suffixes]

        filepaths = []

        if basename.is_absolute():
            filepaths.extend(basenames)
        else:
            for folder in searchpath:
                if isinstance(folder, str):
                    folder = pathlib.Path(folder)

                for name in basenames:
                    filepath = folder.joinpath(name)
                    if filepath.is_file():
                        filepaths.append(filepath)

        return filepaths


    @classmethod
    def _recursive_merge(cls,
                         base   : dict,
                         update : dict):

        for (key, value) in update.items():
            basevalue = base.get(key)

            if isinstance(value, dict) and isinstance(basevalue, dict):
                cls._recursive_merge(basevalue, value)

            elif basevalue is None:
                base[key] = value



    @classmethod
    def _recursive_delta(cls,
                         complete: dict,
                         base    : dict):

        delta = {}
        for (key, value) in complete.items():
            basevalue = base.get(key)
            if isinstance(value, dict) and isinstance(basevalue, dict):
                if deltavalue := cls._recursive_delta(value, basevalue):
                    delta[key] = deltavalue
            elif value != basevalue:
                delta[key] = value
        return delta
