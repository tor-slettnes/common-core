'''
Merge two dictionaries recursively
'''


def recursive_merge(base: dict, update: dict) -> dict:
    '''
    Recursively merge `update` into `base`.  Preference is given to keys
    already present in `base`.

    Returns the updated base.
    '''

    for (key, value) in update.items():
        try:
            basevalue = base[key]

        except KeyError:
            base[key] = value

        else:
            if isinstance(value, dict) and isinstance(basevalue, dict):
                recursive_merge(basevalue, value)

    return base
