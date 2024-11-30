#!/usr/bin/python3 -i
#===============================================================================
## @file jsonreader.py
## @brief Parse JavaScript Object Notation files with Comments
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from importlib.resources.abc import Traversable
from typing import Sequence
import re
import json
import pathlib

FilePath  = str | Traversable
FilePaths = Sequence[FilePath]

class JsonReaderError (ValueError):
    '''Failed to parse JSON file %(filename)r: %(error)s'''

    def __init__(self, error, filename):
        self.error = error
        self.filename = filename

    def __str__ (self):
        return self.__doc__%dict(filename=self.filename, error=self.error)


class JsonReader:
    '''
    Remove a JSON file that may have embedded script-style or C++-style
    comments (prefixed with either `#` or `//`).  These are stripped away before
    passing the remaining text to `json.loads()`.
    '''

    @classmethod
    def read_file(cls, filepath: FilePath) -> dict:
        if isinstance(filepath, str):
            filepath = pathlib.Path(filepath)

        with filepath.open() as fp:
            text = fp.read()

        return cls.parse_text(text)

    @classmethod
    def parse_text(cls, text: str) -> dict:
        text = cls._remove_comments(text)
        try:
            return json.loads(text)
        except json.JSONDecodeError as e:
            raise JsonReaderError(e, filepath) from None


    @classmethod
    def _remove_comments(cls, text: str):
        return re.sub(cls._jsonComments, r"\3\4", text)

    _jsonComments = re.compile(
        r'(//.*?$)|'               # (1) C++ style comments, throw away
        r'(/\*.*?\*/)|'            # (2) C-style comments, throw away
        r'(\'(?:\\.|[^\\\'])*\')|' # (3) Single-quoted strings, retain
        r'("(?:\\.|[^\\"])*")',    # (4) Double-quoted strings, retain
        re.DOTALL | re.MULTILINE
        )


