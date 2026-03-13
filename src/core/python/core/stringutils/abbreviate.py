'''
abbreviate.py - Abbreviate a qualified identifer of the form `fullyQualified.identifier` to fit within a specified sixe constraint. This is done first by shortening leading components to their inital and capitalized character (`fQ.identifier`) as needed, then ellipsizing the result (`fQ.ident…`).
'''

__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

from enum import IntEnum
import re

ELLIPSIS = '…'

class EllipsizePosition (IntEnum):
    START  = 0
    MIDDLE = 1
    END    = -1


def fq_abbrev(identifier: str,
              maxlen: int,
              ) -> str:
    '''
    Abbreviate a qualified identifer of the form `fullyQualified.identifier`
    to fit within a specified size constraint. This is done first by shortening
    leading components to their inital and capitalized character
    (`fQ.identifier`) as needed, then ellipsizing the result (`fQ.ident…`).

    @param identifier
        Fully qualified identifier

    @param maxlen
        Maximum size of the abbreviated result.

    @return
        Abbreviated result
    '''

    if len(identifier) <= maxlen:
        return identifier

    ## Round 1: Abbreviate each but the last component of the qualified name,
    ## reetaining trailing suffixes.
    parts = identifier.split('.')

    for index, part in enumerate(parts[:-1]):
        parts[index] = ident_abbrev(part)

    identifier = '.'.join(parts)

    if len(identifier) <= maxlen:
        return identifier

    # ## Round 2: Abbreviate each but the last component of the qualified name,
    # ## stripping trailing suffixes

    # for index, part in enumerate(parts[:-1]):
    #     parts[index] = ident_abbrev(part, strip_trailing_lowercase=True)

    # identifier = '.'.join(parts)

    # if len(identifier) <= maxlen:
    #     return identifier

    ## Round 3: Ellipsize and strip out leading components of the qualified
    ## name.
    parts.insert(0, ELLIPSIS)

    while (len(parts) > 2) and (len(identifier) > maxlen):
        del parts[1]
        identifier = '.'.join(parts)

    if len(identifier) <= maxlen:
        return identifier

    ## Round 4: Abbreviate the last component of the qualified name,
    ## retaining tailing suffix

    parts[-1] = ident_abbrev(parts[-1])
    identifier = '.'.join(parts)

    if len(identifier) <= maxlen:
        return identifier

    ## Round 4: Ellipsize the identifer
    identifier = ellipsize(identifier, maxlen)

    return identifier


__rx_initial_lowercase = re.compile(r'^([a-z])')
__rx_abbreviated = re.compile(r'([A-Z0-9]|_[a-z])')
__rx_trailing_lowercase = re.compile(r'(?<!_)([a-z]+)$')

def ident_abbrev(identifier: str,
                 strip_initial_lowercase: bool = False,
                 strip_trailing_lowercase: bool = False,
                 ) -> str:
    '''
    Abbreviate simple mixed-case identifer of the form `simpleIdentifer` to
    fit within a specified size constraint. This is done by eliminating
    substrings of lowercase characters leading up to either an uppercase
    character or an underscore, except that by default, the initial character is
    kept regardless of case.

    @param identifier
        Mixed-case identifier

    @param strip_first_lowercase
        Also strip the initial lowercase character if applicable

    @param strip_trailing_lowercase
        Also strip any trailing sequence of lowercase characters

    @return
        Abbreviated result
    '''
    parts = []
    if not strip_initial_lowercase:
        if m := __rx_initial_lowercase.search(identifier):
            parts.append(m.group(1))

    parts.extend(__rx_abbreviated.findall(identifier))

    if not strip_trailing_lowercase:
        if m := __rx_trailing_lowercase.search(identifier):
            parts.append(m.group(1))

    return ''.join(parts)



def ellipsize(text: str,
             maxlen: int,
             position: EllipsizePosition = EllipsizePosition.END,
             ) -> str:
    '''
    Replace portions of an text with an ellipsis character (`…`) as needed to
    fit within a specified size constraint.

    @param text
        Text to ellipsize

    @param maxlen
        Maximum size of the abbreviated result.

    @param position
        Where to insert the ellipsis if required

    @return
        Abbreviated result

    '''

    if len(text) <= maxlen:
        return text

    match(position):
        case EllipsizePosition.START:
            return ELLIPSIS + text[-maxlen+len(ELLIPSIS):]

        case EllipsizePosition.MIDDLE:
            cutoff_start = (maxlen - len(ELLIPSIS)) // 2
            cutoff_end = cutoff_start + len(ELLIPSIS)
            return text[:cutoff_start] + ELLIPSIS + text[-(maxlen-cutoff_end):]

        case EllipsizePosition.END:
            return text[:maxlen-len(ELLIPSIS)] + ELLIPSIS

        case _:
            raise TypeError(
                "Expected `position` argument of type EllipsizePosition, got %s: %r"%(
                    type(position).__name__,
                    position,
                )
            )
