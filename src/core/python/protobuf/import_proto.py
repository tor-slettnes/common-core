#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file import_proto.py
## @brief Import a generated ProtoBuf module into a namespace container
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
import importlib, types, sys

default_suffix = '_pb2'

def import_common_protos(target_scope: object,
                         namespace: str|None = None):
    '''Import well-known ProtoBuf defintions from Google as well as Common Core
    shared defintions into `target_scope` (e.g., `globals()`).

    If `namespace` is provided all symbols will appear there, otherwise they
    will appear in namespaces corresponding to the `package` declaration from
    the respective `.proto` files.

    '''
    import_wellknown_protos(target_scope, namespace)
    import_core_protos(target_scope, namespace)


def import_wellknown_protos (target_scope: object,
                             namespace: str|None = None):
    '''Import well-known .proto files into `target_scope` (e.g., `globals()`).

    If `namespace` is provided all symbols will appear there, otherwise they
    will appear in namespaces corresponding to the `package` declaration from
    the respective `.proto` files; in this case, `google.protobuf`

    '''
    import_proto('google.protobuf.empty', target_scope, namespace)
    import_proto('google.protobuf.wrappers', target_scope, namespace)
    import_proto('google.protobuf.duration', target_scope, namespace)
    import_proto('google.protobuf.timestamp', target_scope, namespace)
    import_proto('google.protobuf.struct', target_scope, namespace)


def import_core_protos(target_scope: object,
                       namespace: str|None = None):
    '''Import Common Core shared ProtoBuf definitions into `target_scope`
    (e.g., `globals()`).

    If `namespace` is provided all symbols will appear there, otherwise they
    will appear in namespaces corresponding to the `package` declaration from
    the respective `.proto` files (starting with `cc.`).
    '''

    import_proto('variant', target_scope, namespace)
    import_proto('status', target_scope, namespace)
    import_proto('signal', target_scope, namespace)
    import_proto('request_reply', target_scope, namespace)


def import_proto(module_name: str,
                 target_scope: object,
                 namespace: str|None = None):

    '''Import a generated ProtoBuf module.  Their symbols will appear in
    namespace containers that match the `package` declarations in their
    respective `.proto` files, which are created as class objects if missing.

    Inputs:
    @param[in] module_name
        Module name.  The base name portion (following any leading path) can be
        the stem of the original `.proto` file name (without the extension), or
        it can be the name of the corresponding generated Python module (with a
        `_pb2` suffix added).  If the name is unqualified (without any leading
        package name) it will be loaded from the `generated` namespace.

    @param[in] target_scope
        Where to create the target namespace. This could be a dictionary
        (e.g., `globals()`), class, class instance, or module.

    @param[in] namespace
        What namespace to create or update.  By default, the ProtoBuf `package`
        name is used.  Leading elements of this namespace are reused/updated if
        they already exist as one of the above types, otherwise new empty
        container objects are created recursively.

    Example:

    * Import `cc.generated.variant_pb2` into the namespace `cc.variant` per
      the `package` declaration from `variant.proto`:

      ```
      import_proto('variant', globals())
      pi_value = cc.variant.Value(value_real = math.pi)
      ```

    '''

    module_path = (module_name if "." in module_name
                   else "...generated." + module_name)

    if not module_path.endswith(default_suffix):
        module_path += default_suffix

    module = importlib.import_module(module_path, __spec__.name)
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
