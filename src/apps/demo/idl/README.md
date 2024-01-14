Common Core Apps Demo - IDLs
============================

This folder contains [Interface Definition Language](https://en.wikipedia.org/wiki/Interface_description_language) (IDL) files that describes interface between `demoserver` and its peers.


Data types
----------

At its core, IDL describes the data types that are shared between applications over the DDS data bus. The syntax is similar to that of C-based languages, and includes:

 * the actual payload, defined in `struct` or `union` blocks,
 * type aliases, such as `typedef DECLARATION TypeAlias` keyword
 * enumerations, defined using `enum SYMBOL {...};`.
 * shared constants, using as `const TYPE SYMBOL = VALUE;`
 * namespace encapuslations using the `module NAME {...};`

In our case, these data types are defined in [demo-types.idl](demo-types.idl).


Pub/Sub
-------

DDS is at its core a Pub/Sub based system, so the above data types essentially boil down to describing the payload of the published messages.  In our demo, these are:

* `struct CC::Demo::Greeting`, and
* `struct CC::Demo::TimeData`.

It also contains nested data structures and enumerations used by these.  Additional nested types come from the file [common_types.idl](../../../shared/idl/common-types.idl), which is included here via the `#include` precprocessor directive.

The [demo-types.idl](demo-types.idl) file also contains constants that define associated publication topics; in our demo these are
* `CC::Demo::GREETING_TOPIC` and
* `CC::Demo::TIMEDATA_TOPIC`.


RPC Interface
-------------

A second file, [demo-interface.idl](demo-interface.idl) defines the RPC interface between the `DemoService` server and its clients. An `interface` block contains abstract method signatures, again with a C-like syntax:

```
@DDSService
interface DemoService
{
    void say_hello(Greeting greeting);
    TimeData get_current_time()
    void start_ticking();
    void stop_ticking();
}
```

### Generated code

Just as in the case of data structures, these interface declarations are translated into language-specific source code via the `rtiddsgen` tool.  These comprise:

- A service class (`CC::Demo::DemoService`) containing pure virtual methods, meant to be overridden in our application, and

- A client stub (`CC::Demo::DemoServiceClient`) providing corresponding methods that can be invoked in the client application.  Its input parameters and return types are those defined in the `demo-types.idl` file.

### Service Identifier

In the DDS realm, RPC is a higher-level feature which relies on Pub/Sub for the underlying data exchange. As such, a pair of request/reply topics are used to establish communication betwen the Client/Server pair, and their respective constructors take the _prefix_ for these topics as input. For this purpose this exmaple defines the symbol:

* `CC::Demo::SERVICE_ID`.

The actual topic names are then formed by the DDS implementation by respectively appending `"Request"` and `"Reply"` to the above prefix.



Code generation
---------------

The IDL files in this folder are used as inputs for the RTI-provided generator tool `rtiddsgen` to generate language-specific source code.  Specifically, the [CMakeLists.txt](CMakeLists.txt) file includes [cmake/BuildIDL.cmake](../../../../cmake/BuildIDL.cmake) to generate C++ language bindings from these `.idl`s.

