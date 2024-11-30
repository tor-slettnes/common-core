#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file timeutil.py
## @brief Miscellaneous date/time utilities
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
import time, enum

class ZoneSuffix(enum.Enum):
    NONE   = 0
    OFFSET = 1   # '+0100' for CET, '-0800' for PST, 'Z' for UTC
    NAME   = 2   # 'CET'/'CEST'/'PST'/'PDT'/etc, or 'UTC'


### Standardized time string
def isotime(timestamp        : float = None,
            time_delimiter   : str  = " ",
            decimals         : int  = 3,
            local            : bool = True,
            zone_suffix      : ZoneSuffix = ZoneSuffix.NONE,
            zone_delimiter   : str = ""):
    '''
    ISO 8601 string representation of a timestamp.

    @param timestamp
        Python timestamp to represent, or `None` for current time.

    @param time_delimiter
        Delimiter between date and time.  ISO 8601 allows space (' ') or 'T'.

    @param decimals
        Sub-second precision.  The default is 3 (milliseconds).

    @param local
        Return local time (default).  If `False`, return UTC instead.

    @param zone_suffix
        Append timezone suffix. One of:
        - ZoneSuffix.NONE   - No suffix
        - ZoneSuffix.OFFSET - ISO zone indicator, e.g. '-0800' for local time
                              or 'Z' for UTC ("Zulu"/"Zero Offset").
        - ZoneSuffix.NAME   - Abbreviated effective none name, e.g. `PST` or `PDT`.
                              (Not ISO 8601 compliant, nor globally unique).

    @param zone_delimiter
        Delimiter between time and zone name.
    '''


    if timestamp is None:
        timestamp = time.time()

    timestruct = time.localtime(timestamp) if local else time.gmtime(timestamp)
    parts = [ time.strftime(f"%F{time_delimiter}%T", timestruct) ]

    if decimals:
        fraction = timestamp - int(timestamp)
        parts.append(".%0*d"%(decimals, (10**decimals) * fraction))

    if zone_suffix == ZoneSuffix.OFFSET:
        parts.append(zone_delimiter)
        if local:
            parts.append(
                "%+03d%02d"%(-time.timezone / 3600,
                             (-time.timezone % 3600) / 60))
        else:
            parts.append("Z")

    elif zone_suffix == ZoneSuffix.NAME:
        parts.append(zone_delimiter)
        if local:
            name = time.tzname[timestruct.tm_isdst]

            ### On Windows we get the full name, e.g. "Pacific Daylight Time"
            ### or "Central European Summer Time".   Extract only caps.
            abbrev = filter(lambda c: c.isupper(), name)

            parts.append(abbrev)
        else:
            parts.append("UTC")

    return "".join(parts)


def jstime(timestamp : float = None):
    '''
    Time representation per JSON/JavaScript standard:

       YYYY-MM-DDTHH:MM:SSZ

    '''

    return isotime(timestamp,
                   time_delimiter="T",
                   decimals=3,
                   local=False,
                   zone_suffix=ZoneSuffix.OFFSET,
                   zone_delimiter="")
