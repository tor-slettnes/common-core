'''
logbase.py - Mix-in class to initialize logging
'''

import inspect
import logging
import sys
from .settingsstore import SettingsStore


# if sys.hexversion >= 0x030c0000:
#     # Python 3.12 and above have `taskName`
#     DEFAULT_FORMAT = "%(asctime)s|%(levelname)8s|%(taskName)-8s|%(name)-24s|%(message)s"
# else:
DEFAULT_FORMAT = "%(asctime)s|%(levelname)8s|%(threadName)-12s|%(name)-24s|%(message)s"

def init_logging(threshold: int = logging.INFO,
                 format: str = DEFAULT_FORMAT,
                 force: bool = True):
    logging.basicConfig(level=threshold, format=format, force=force)



class LogBase:
    log_name  = None
    _logger   = None
    _settings = SettingsStore('logging')

    def __init__(self,
                 logger: logging.Logger|None = None,
                 log_name: str|None = None):

        if log_name:
            self.log_name = log_name

        self.logger = logger

    @property
    def logger(self) -> logging.Logger:
        '''
        Return an existing or new logger for this instance.
        '''
        if self._logger is None:
            log_name = self.log_name or self.package_name()
            self._logger = logging.getLogger(log_name)
            if level := self.threshold_setting(log_name):
                self._logger.setLevel(level)
        return self._logger

    @logger.setter
    def logger(self, logger: logging.Logger):
        '''
        Assign a logger to this instance.
        '''
        self._logger = logger

    def package_name(self):
        return '.'.join(self.__module__.split('.')[:-1])

    @classmethod
    def threshold_setting(cls, log_name: str) -> int|None:
        if level_name := cls._settings.get_value(path = ('thresholds', log_name)):
            try:
                level = getattr(logging, level_name)
                assert isinstance(level, int)
            except (AttributeError, AssertionError):
                return None
            else:
                return level
        else:
            return None
