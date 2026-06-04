'''
Obtain settings from `.yaml`, '.json', and/or `.ini` files
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'


### Standard Python modules
from typing import Sequence, Mapping, Hashable, Optional, Union, Any
import yaml
import json
import os
import os.path
import sys
import platform
import re
import pathlib


### Modules within package
from ..paths import (
    FilePath, FilePaths, SearchPath,
    FilePathInput, FilePathInputs, SearchPathInput,
    normalized_search_path, settings_path,
    local_settings_path, preinstalled_settings_path,
    find_settings_files, settings_suffixes,
)
from ..maputils import recursive_merge, recursive_delta
from .jsonreader import JsonReader
from .ini_reader import INIFileReader

#Type hints

VariantList = list['Variant']
VariantMap  = dict[str, 'Variant']
Variant     = None | bool | int | float | str | VariantList | VariantMap


class SettingsStore (dict):
    '''
    A specialized dictionary containing configuration settings, loaded from
    one or more JSON, YAML and/or INI file(s) located within specific settings
    folders.

    Settings may be loaded from absolute or relative paths on initialization or
    subsequently; see `__init__()` and `load_settings()` for details.

    Comments are supported as follows:

    - In JSON files, comments starting with `#` or `//` until the end of the
      line are stripped away prior to parsing.

    - The YAML parser natively supports embedded comments starting with `#`.

    - INI files are parsed with Python's native RawConfigParser, which supports
      comments following `;` or `#`.


    ### Example

    Consider a deployment where this module is located somewhere inside
    `/usr`, and default configurations are stored in YAML files within the
    folder `/usr/share/common-core/settings`.  Let's say you create a
    `SettingsStore` instance as follows:

    ```python
    my_settings = SettingsStore('my_settings')
    ```

    Since `searchpath` is not explicitly provided on instantiation, the
    default value `['/etc/common-core', 'share/common-core/settings']` is
    used.  Settings are then loaded and merged in from whichever of the
    following paths exist, in turn:

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

    parser_map = {
        '.json': JsonReader.parse_text,
        '.yaml': yaml.safe_load,
        '.ini': INIFileReader.parse_text,
    }

    ### Delimiter between elements in settings paths
    PATH_DELIMITER = '.'

    def __init__(self,
                 filenames  : FilePathInput|FilePathInputs|None = [],
                 searchpath : SearchPathInput|None = None,
                 package    : str|None = None,
                 name       : str|None = None):

        '''
        Initialize a new SettingsStore instance from the specified file(s).

        The `filenames` argument may contain absolute or relative file paths;
        in the latter case, it is located relative to one or more folders in
        `searchpath` if provided, otherwise in the default search path.
        See `load_settings()` for details.

        Similarly, each directory name in `searchpath` may also be absolute or
        relative.  In the latter case, names are resolved with respect to
        `cc.core.paths.install_root()`, defined at build time (e.g. `/usr`).

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
             (`share/common-core/settings`), located relative to
             `cc.core.paths.install_root()`.

           * a `settings` folder directly within the Python root where this
             module is located (e.g. `lib/python3.XX/site-packages/settings` or
             `lib/python3/dist-packages/settings` within a VENV, or `settings`
             directly within a Python wheel).

           * the folder in which the Python package `package` (if provided) is
             installed.  This is intended for submodule settings.

        If `name` is provided it will be used to identify this SettingsStore
        instance in any generated log messages and exceptions.
        '''

        self._filenames = []
        self._filepaths = []
        self.name       = name
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


    @property
    def description(self):
        if self.name is not None:
            return self.name
        else:
            return '%s(%s)'%(
                type(self).__name__,
                self.filenames,
            )

    def load_settings(self,
                      filename : FilePathInput,
                      searchpath : SearchPathInput|None = None,
                      clear: bool = False):

        '''
        Load values from the specified settings file, if found.

        If `clear` is True, existing values are cleared.  Otherwise, new values
        are merged in recursively, with precedence given to those already in the
        store.

        The provided `filename` is resolved as follows:

        - If the name is relative, it is resolved with respect to each folder in
          the search path (see below), possibly resulting in multiple candidate
          paths.

        - If the file name ends in '.json', `.yaml` or `.ini` the corresponding
          parser is used.  If the file name has no suffix, each of these
          supported suffixes is in turn appended to each candidate path, in that
          order.  Any filename with a different/unsupported suffix is ignored.

        - If one or more of the resulting candidate path(s) exist on the
          filesystem, they are consecutively parsed and merged in with the
          existing settings.

        If provided, `searchpath` overrides the default search path constructed
        by `__init__()`. As in that case, it may similarly contain absolute or
        relative paths, each of which are then resolved with respect to
        `cc.core.paths.install_root()` as defined at build time (e.g. `/usr`).
        '''

        if clear:
            self.clear()

        for filepath in self.find_paths(filename, searchpath):
            self.merge_file(filepath)

        self._filenames.append(filename)


    def merge_file(self, filepath: FilePathInput):
        '''
        Merge in settings from the specified file.

        @param filepath
            Settings file to merge. The name may be absolute or relative,
            see `load_settings()` for details.
        '''

        if isinstance(filepath, str):
            filepath = pathlib.Path(filepath)

        parser = type(self).parser_map.get(filepath.suffix, '.json')

        try:
            with filepath.open() as fp:
                text = fp.read()

        except EnvironmentError as e:
            pass

        else:
            settings = parser(text) or {}
            self.merge_settings(settings)
            self._filepaths.append(filepath)


    def merge_settings(self, settings: dict):
        '''Merge in the specified settings'''

        recursive_merge(self, settings)


    def get_value(self,
                  key: str|Sequence[str|int]|None = None,
                  expected_type: Union[type]|None = None,
                  default: Variant|None = None,
                  *,
                  path: str|Sequence[str|int]|None = None,
                  raise_invalid_type: bool = False,
                  raise_missing: bool = False,
                  ) -> Variant:
        '''
        Return the value referenced by `key`.

        @param key
            Either a single '.'-delimited string or a sequence comprising a
            string followed by any number of strings and/or integers, in either
            case denoting a hierarchical path to the target value.  Within this
            sequence, a string (including the first path element) descends into
            a corresponding dictionary item along the way, whereas an integer
            selects a list item.

        @param expected_type
            A type or a union of types (normally separated by `|`).

        @param default
            Value to return if `path` does not reference an existing value, or
            if the value is not of the requested type(s).

        @param path
            Keyword-only alternative to `key` for backwards compatibilty.

        @param raise_invalid_type
            Raise a TypeError if `path` is found but the corresponding value is
            not of the requested type. Also raise a TypeError after failing to
            descend into an intermediate path element because the corresponding
            value exists but is not a dictionary or list, as applicable.

        @param raise_missing
            Raise a KeyError if the setting denoted by `path` is missing. In the
            case where `path` is a sequence, also raise a KeyError or IndexError
            if any of it its intermediate elements is missing from the preceding
            dictionary or list.

        @return The value corresponding to `key` if it exists and is of the
            expected type, otherwise `default`
        '''


        if (key is not None and path is not None) or (key is None and path is None):
            raise TypeError(
                "%s: Exactly one of `key` or `path` must be provided"%(
                    self.description,
                )
            )

        elif key is None:
            key = path


        if isinstance(key, str):
            path = key.split(self.PATH_DELIMITER)

        elif isinstance(key, Sequence) and len(key) > 0:
            path = key

        elif key is not None:
            path = [str(key)]

        elif not isinstance(path, Sequence) or len(path) == 0:
            raise ValueError(
                "%s: `path` must be a non-empty sequence of strings and/or ints"%(
                    self.description,
                )
            )

        components = []
        try:
            value = self
            for component in path:
                components.append(str(component))

                if isinstance(value, (tuple, list)):
                    if isinstance(component, str) and component.isdigit():
                        component = int(component)

                value = value[component]

        except KeyError:
            if raise_missing:
                raise KeyError(
                    "%s: No such setting: %r"%(
                        self.description,
                        self.PATH_DELIMITER.join(components),
                    )
                ) from None
            else:
                return default

        except IndexError:
            if raise_missing:
                raise IndexError(
                    "%s: Index out of range: %r"%(
                        self.description,
                        self.PATH_DELIMITER.join(components),
                    )
                ) from None
            else:
                return default

        except TypeError:
            if raise_invalid_type:
                raise TypeError(
                    "%s: Invalid key for dereferencing %s value: %r"%(
                        self.description,
                        type(value).__name__,
                        self.PATH_DELIMITER.join(components),
                    )
                ) from None
            else:
                return default

        if expected_type is None or isinstance(value, expected_type):
            return value

        try:
            return expected_type(value)
        except (TypeError, ValueError):
            if raise_invalid_type:
                raise TypeError(
                    "%s: Expected setting %r to be of type %s, got %s: %r"%(
                        self.description,
                        self.PATH_DELIMITER.join(components),
                        expected_type.__name__,
                        type(value).__name__,
                        value,
                    )
                ) from None

            else:
                return default


    def set(self,
            key: str|Sequence[str|int],
            value: Variant,
            *,
            raise_missing: bool = False,
            save: bool = False):
        '''
        Set the value referenced by `key` to `value`.

        @param key
            Either a single '.'-delimited string or a sequence comprising a
            string followed by any number of strings and/or integers, in either
            case denoting a hierarchical path to the target value.  Within this
            sequence, a string (including the first path element) descends into
            a corresponding dictionary item along the way, whereas an integer
            selects a list item.

        @param value
            The new or updated value associated with the specified key.

        @param raise_missing
            Raise a KeyError after failing to descend into an intermediate path
            element because the corresponding key is missing from the preceding
            dictionary. Without this flag, leading path components are created
            as needed.

        @param save
            Automatically save the resulting settings store delta in the local
            settings folder.

        @exception IndexError
            The provided `path` contains a index to dereference an intermediate
            list value, but the index is not within `range(len(LIST)+1)`.

        @exception TypeError
            An attempt was made to dereference an intermediate object that is
            either not a dictionary or a list, or using a path element of an
            invalid type (for instance an unhashable type, or a non-integer
            value to dereference a list).

        @exception KeyError
            An attempt was made to dereference an intermediate dictionary value
            with a non-existing key, with the `raise_missing` option set.
        '''

        if isinstance(key, str):
            path = key.split(self.PATH_DELIMITER)
            key = path.pop()

        elif isinstance(key, Sequence) and len(key) > 0:
            path = list(key)
            key = path.pop()

        else:
            raise ValueError(
                "%s: `key` must be a string or a non-empty sequence of strings and/or ints"%(
                    self.description,
                )
            )

        obj = self
        components = []
        for component in path:
            components.append(component)

            if isinstance(obj, list):
                if isinstance(component, str) and component.isdigit():
                    component = int(component)

                if component == len(obj):
                    obj.append({})

            try:
                sub = obj[component]

            except IndexError:
                raise IndexError(
                    "%s: Index out of range: %r"%(
                        self.description,
                        self.PATH_DELIMITER.join(components),
                    )
                ) from None

            except KeyError:
                if raise_missing:
                    raise KeyError(
                        "%s: No such setting: %r"%(
                            self.description,
                            self.PATH_DELIMITER.join(components),
                        )
                    ) from None
                else:
                    sub = obj[component] = {}

            except TypeError:
                if not isinstance(component, typing.Hashable):
                    raise TypeError(
                        "%s: Unhashable path component type %s: %s"%(
                            self.description,
                            type(component).__name__,
                            self.PATH_DELIMITER.join(components),
                        )
                    ) from None

                else:
                    raise TypeError(
                        "%s: Invalid key for dereferencing %s value: %r"%(
                            self.description,
                            type(value).__name__,
                            self.PATH_DELIMITER.join(components),
                        )
                    ) from None

            obj = sub

        if isinstance(obj, list):
            if isinstance(key, str) and key.isdigit():
                key = int(key)

            if key == len(obj):
                obj.append(None)

        obj[key] = value

        if save and self.filenames:
            self.save_delta()


    def unset(self,
              key: str|Sequence[str|int],
              *,
              save: bool = False) -> bool:
        '''
        Remove an existing key, as well as any empty intermediate objects
        leading up to it.

        @param key
            Either a single '.'-delimited string or a sequence comprising a
            string followed by any number of strings and/or integers, in either
            case denoting a hierarchical path to the target value.

        @param save
            Automatically save the resulting settings store delta in the local
            settings folder.

        @return
            True iff an existing key key was removed.
        '''

        if isinstance(key, str):
            path = key.split(self.PATH_DELIMITER)
            key = path.pop()

        elif isinstance(key, Sequence) and len(key) > 0:
            path = list(key)
            key = path.pop()

        else:
            raise ValueError(
                "%s: `key` must be a string or a non-empty sequence of strings and/or ints"%(
                    self.description,
                )
            )

        obj = self
        stack = []
        for component in path:
            if isinstance(obj, list):
                if isinstance(component, str) and component.isdigit():
                    component = int(component)


            try:
                sub = obj[component]
            except (IndexError, KeyError, TypeError):
                return False

            stack.append((component, obj, sub))
            obj = sub

        if isinstance(obj, list):
            if isinstance(key, str) and key.isdigit():
                key = int(key)

        try:
            del obj[key]
        except (IndexError, KeyError):
            return False
        else:
            while stack:
                component, obj, sub = stack.pop()
                if not sub:
                    del obj[component]

            if save and self.filenames:
                self.save_delta()

            return True


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
                   keep_empty: bool = False,
                   sort_keys: bool = True,
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

        @param keep_empty
            Save to the file even if this settings object is empty.  Otherwise,
            nothing is saved, and any existing file matches are removed from the
            local settings folder.

        @param sort_keys
            See `help(json.dump)`

        @param skipkeys
            See `help(json.dump)`

        @param ensure_ascii
            See `help(json.dump)`

        @param check_circular
            See `help(json.dump)`

        @param allow_nan
            See `help(json.dump)`

        @param indent
            See `help(json.dump)`
        '''

        args = locals()
        args.update(only_delta = True)
        return type(self).save(**args)


    def save(self,
             filename: FilePathInput|None = None,
             keep_empty: bool = False,
             only_delta: bool = False,
             sort_keys: bool = True,
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

        @param keep_empty
            Save to the file even if this settings object is empty.  Otherwise,
            nothing is saved, and any existing file matches are removed from the
            local settings folder.

        @param only_delta
            Save only the recursive difference between the current settings
            hierarchy and the default settings, obtained from the software
            distribution (i.e, corresponding filenames within
            `/usr/share/common-core/settings`).

        @param sort_keys
            See `help(json.dump)`

        @param skipkeys
            See `help(json.dump)`

        @param ensure_ascii
            See `help(json.dump)`

        @param check_circular
            See `help(json.dump)`

        @param allow_nan
            See `help(json.dump)`

        @param indent
            See `help(json.dump)`

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

        if settings or keep_empty:
            failure = None
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
                                  sort_keys = sort_keys,
                                  skipkeys = skipkeys,
                                  ensure_ascii = ensure_ascii,
                                  check_circular = check_circular,
                                  allow_nan = allow_nan,
                                  indent = indent)
                    break
            else:
                if failure:
                    raise failure

        else:
            for path in find_settings_files(filename, search_path=local_settings_path()):
                try:
                    os.remove(path)
                except EnvironmentError as e:
                    pass



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
                   searchpath: SearchPathInput|None = None) -> FilePaths:
        '''
        Find settings files with the specified base name within the
        specified search path, or the default search path for this SettingsStore
        instance.

        @param basename
            Stem (with or without a suffix) of the filename we are looking for.
            If no suffix is provided, each of the supported settings suffixes
            is tried in turn: `.json`, `.yaml`, `.ini`'

        @param searchpath
            Folder paths in which to look for files.  Each path may be absolute
            or relative; in the latter case, it is resolved with respect to
            `cc.core.paths.install_root()` as defined at build time
            (e.g. `/usr`).

        @return
            A list of absolute pathnames to matching settings files.
        '''

        return find_settings_files(
            basename = basename,
            search_path = (
                normalized_search_path(searchpath) if searchpath is not None
                else self.searchpath
            )
        )

    def recursive_delta(self) -> dict:
        '''
        Return the recursive difference between the default settings as
        preinstalled with the application and this instance.

        See also `defaults()`.
        '''
        return recursive_delta(self, self.defaults())


