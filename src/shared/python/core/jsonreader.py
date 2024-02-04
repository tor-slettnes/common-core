#!/usr/bin/python3 -i
#===============================================================================
## @file jsonreader.py
## @brief Parse JavaScript Object Notation files with Comments
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import re
import json
import typing
import os.path

FilePath = str

class JsonReaderError (ValueError):
    '''Failed to parse JSON file %(filename)r: %(error)s'''

    def __init__(self, error, filename):
        self.error = error
        self.filename = filename

    def __str__ (self):
        return self.__doc__%dict(filename=self.filename, error=self.error)


class JsonReader (dict):
    '''A specialized dictionary loaded from and saved to JSON file(s).
    Any script-style or C++-style comments (prefixed with either `#` or `//`)
    are stripped away before JSON parsing.
    '''

    def load_file(self, filepath):
        '''Load settings from the specified file.

        @param[in] filepath
            Settings file to load.
        '''

        self.clear()
        self.update(self.read_file(filepath))


    def merge_file(self, filepath : FilePath):
        '''Merge in settings from the specified file.

        @param[in] filepath
            Settings file to load. The name may be absolute or relative.
        '''
        JsonReader._recursive_merge(self, self.read_file(filepath))


    @classmethod
    def _recursive_merge(cls,
                         base   : dict,
                         update : dict):

        for (key, value) in update.items():
            basevalue = base.get(key)

            if isinstance(value, dict) and isinstance(basevalue, dict):
                JsonReader._recursive_merge(basevalue, value)
            elif isinstance(value, (list, tuple)) and isinstance(basevalue, (list, tuple)):
                base[key].extend(value)
            else:
                base[key] = value

    @classmethod
    def read_file(cls, filepath):
        with open(filepath) as fp:
            text = fp.read()

        text = cls._remove_comments(text)
        try:
            return json.loads(text)
        except json.JSONDecodeError as e:
            raise JsonReaderError(e, filepath) from None


    @classmethod
    def _remove_comments(cls, text):
        return re.sub(cls._jsonComments, r"\3\4", text)

    _jsonComments = re.compile(
        r'(//.*?$)|'               # (1) C++ style comments, throw away
        r'(/\*.*?\*/)|'            # (2) C-style comments, throw away
        r'(\'(?:\\.|[^\\\'])*\')|' # (3) Single-quoted strings, retain
        r'("(?:\\.|[^\\"])*")',    # (4) Double-quoted strings, retain
        re.DOTALL | re.MULTILINE
        )


