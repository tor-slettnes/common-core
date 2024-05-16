#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file import_proto.py
## @brief Import a generated ProtoBuf module into a namespace container
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import importlib, types

default_suffix = '_pb2'

def import_proto(module_name: str,
                 target_scope: object,
                 namespace: str|None = None):

    '''Import a generated ProtoBuf module (normally with suffix `_pb2`) into a
    local namespace derived from its `package` name.

    Inputs:
    @param[in] module_name
        Module name.  The base name portion (following any leading path) can be
        the stem of the original `.proto` file name (without the extension), or
        it can be the name of the corresponding generated Python module (with a
        `_pb2` suffix added).

    @param[in] target_scope
        Where to create the target namespace. This could be a dictionary
        (e.g., `globals()`), class, class instance, or module.

    @param[in] namespace
        What namespace to create or update.  By default, the ProtoBuf `package`
        name is used.  Leading elements of this namespace are reused/updated if
        they already exist as one of the above types, otherwise new empty
        container objects are created recursively.
    '''

    candidates = [module_name]
    if not module_name.endswith(default_suffix):
        candidates.insert(0, module_name + default_suffix)

    for candidate in candidates:
        try:
            module = importlib.import_module(candidate)
            break
        except ImportError as e:
            failure = e
    else:
        raise failure from None

    scope = target_scope

    if namespace is None:
        namespace = module.DESCRIPTOR.package

    if namespace:
        for element in namespace.split("."):
            scope = add_container(scope, element)

    populate_container(scope, module)
    return module


def add_container(scope: object, name: str) -> object:
    '''Return an existing or new container within the specified scope.
    If it exists it should be a dictionary, class, class instance, or module.
    Otherwise, a new empty class is instantiated and added to the scope.
    In either case, the resulting container is returned.
    '''

    if isinstance(scope, dict):
        try:
            container = scope[name]
        except KeyError:
            cls = type(name, (), {})
            container = scope[name] = cls()

    else:
        try:
            container = getattr(scope, name)
        except AttributeError:
            cls = type(name, (), {})
            container = cls()
            setattr(scope, name, container)

    # if not isinstance(container, type):
    #     container = vars(container)

    return container


def populate_container(scope: object,
                       module: types.ModuleType) -> None:
    '''Add symbols from `module` into the specified scope, which should be
    a dictionary, class, class instance, or module.
    '''

    items = [(name, value)
             for (name, value) in vars(module).items()
             if not name.startswith('_')]

    if isinstance(scope, dict):
        scope.update(items)

    else:
        for item in items:
            setattr(scope, *item)
