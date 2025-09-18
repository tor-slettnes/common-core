#!/usr/bin/python3 -i
#===============================================================================
## @file inifilereader.py
## @brief Parse .INI-style settings files
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from importlib.resources.abc import Traversable
from typing import Sequence
import configparser as cp

FilePath  = str | Traversable
FilePaths = Sequence[FilePath]

class INIFileReader:
    '''
    Read a `.ini`-style file and return 
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
        result = {}
        parser = cp.RawConfigParser()
        parser.read_string(text)
        for section_name in parser.sections():
            result[section_name] = dict(parser.items(section_name))

        return result
