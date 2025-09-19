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
    '''Failed to parse JSON: %(error)s'''

    def __init__(self, error):
        self.error = error

    def __str__ (self):
        return self.__doc__%dict(error=self.error)


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
        if text := cls._remove_comments(text).strip():
            try:
                return json.loads(text)
            except json.JSONDecodeError as e:
                raise JsonReaderError(e) from None
        else:
             return None

    @classmethod
    def _remove_comments(cls, text: str):
        return re.sub(cls._jsonComments, r"\4\5", text)

    _jsonComments = re.compile(
        r'(//.*?$)|'               # (1) C++ style comments, throw away
        r'(/\*.*?\*/)|'            # (2) C-style comments, throw away
        r'(#.*?$)|'                # (3) Script-style comments, throw away
        r'(\'(?:\\.|[^\\\'])*\')|' # (4) Single-quoted strings, retain
        r'("(?:\\.|[^\\"])*")',    # (5) Double-quoted strings, retain
        re.DOTALL | re.MULTILINE
        )

