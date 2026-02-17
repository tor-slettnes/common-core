'''
Extract the recursive delta between two dictionaries.
'''


def recursive_delta(complete: dict, base: dict) -> dict:
    '''
    Return a new dictionary built by recursively inserting those keys that
    are in `complete` but not in `base`.
    '''

    delta = {}
    for (key, value) in complete.items():
        basevalue = base.get(key)
        if isinstance(value, dict) and isinstance(basevalue, dict):
            if deltavalue := recursive_delta(value, basevalue):
                delta[key] = deltavalue
        elif value != basevalue:
            delta[key] = value
    return delta
