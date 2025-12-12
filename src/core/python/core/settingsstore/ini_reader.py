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
    Read an return the contents of an `.ini`-style file as a dictionary.
    Top-level keys correspond to INI file sections, where each value is a flat
    dictionary of options and values witin the corresponding section.
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
