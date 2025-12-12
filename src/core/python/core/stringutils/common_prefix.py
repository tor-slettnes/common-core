'''
stringutils.py - miscellaneous string utilities
'''

from typing import Sequence

def common_prefix(symbols: Sequence[str],
                  delimiter: str = '_') -> tuple[int, str]:
    '''
    Find the largest leading substring comprising components separated by
    `delimiter` that is common to all strings in `symbols`.

    @param symbols
        Input strings from which a common prefix is to be deduced

    @param delimiter
        Component separator

    @return
        A 2-element tuple comprising the length and the contents of the common
        suffix

    Example:

    ```
    >>> cc.core.stringutils.common_prefix(['MY_PREFIX_TWO', 'MY_PREFIX_THREE'], '_')
    (10, 'MY_PREFIX_')
    ```
    '''

    if symbols:
        split_symbols = [symbol.split(delimiter) for symbol in symbols]
        prefix_parts = split_symbols.pop(0)
        prefix_length = 0

        for symbol_parts in split_symbols:
            prefix_parts = [
                prefix_part
                for (prefix_part, symbol_part) in zip(prefix_parts, symbol_parts)
                if prefix_part == symbol_part
            ]


        if prefix_parts:
            prefix_length = sum([(len(part)+len(delimiter))
                                 for part in prefix_parts])

        return prefix_length, delimiter.join(prefix_parts)
    else:
        return 0, ""

