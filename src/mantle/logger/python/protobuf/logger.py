#!/bin/echo Do not invoke directly.
#===============================================================================
## @file logger.py
## @brief Support for ProtoBuf types from `logger.proto`.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Generated from `logger.proto`
from cc.generated.logger_pb2 import ColumnType as _ColumnType, ColumnSpec
from cc.generated.logger_pb2 import *

### Standard Python modules
import enum

ColumnType = enum.IntEnum(_ColumnType.DESCRIPTOR.name, _ColumnType.items())

ColumnTypeMap = {
    None: ColumnType.COLTYPE_NONE,
    bool: ColumnType.COLTYPE_BOOL,
    int: ColumnType.COLTYPE_INT,
    float: ColumnType.COLTYPE_REAL,
    str: ColumnType.COLTYPE_TEXT,
    bytes: ColumnType.COLTYPE_BLOB,
}


def encodeColumnType(coltype : type|ColumnType) -> ColumnType:
    if isinstance(coltype, (ColumnType, _ColumnType)):
        return coltype
    else:
        return ColumnTypeMap[coltype]


def encodeColumnSpec(colspec : None|tuple|ColumnSpec) -> ColumnSpec:
    if isinstance(colspec, ColumnSpec):
        return colspec

    elif isinstance(colspec, tuple):
        spec = ColumnSpec()

        if len(colspec) >= 1:
            spec.event_field = colspec[0]

        if len(colspec) >= 2:
            spec.column_name = colspec[1]

        if len(colspec) >= 3:
            spec.column_type = encodeColumnType(colspec[2])

        if len(colspec) >= 4:
            spec.format_string = colspec[3]

        return spec

    else:
        raise TypeError("Invalid column spec type: %r"%(type(colspec,)))
