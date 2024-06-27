Common Core Demo Application
----------------------------

This demo application is provided to illustrate the morphology of a typical
service-based applications based on the Common Core application framework. It
may be used as a template for creating new applications, or as a reference
implementation of current best practices.

For illustration, our demo comprises two distinct implementations of the same
Application Programming Interface (API), described below. Each approach
showcases different building blocks within our codebase, ranging from basic C++
utilites to client/server communication over gRPC.


Application Programming Interface
---------------------------------

Our demo API comprises just a couple of seemingly random methods:

* `say_hello(Greeting)` to issue a greeting to whomever may be listening

* `get_current_time() -> TimeData` returns the current time

* `start_ticking()` to start issuing time notifications at regular intervals
  (1x/second)

* `stop_ticking()` to stop issuing time notifications updates

The C++ flavor of this API is declared in [demo-api.h++](cpp/api/demo-api.h++).


Event propagation
-----------------

Notifications are propagated internally within a process using an adaptation of
the [signal/slot pattern](https://en.wikipedia.org/wiki/Signals_and_slots)
pattern, which essentially boils down to a fancy way of doing callbacks. A
_signal_ essentially represents the interface between the producers and
consumers of a prescribed event. In this way it is similar to a _topic_ in the
Pub/Sub pattern, albeit adapted for intra-process communication. Its two most
important methods are:

 - `connect(callback_method)` to register a callback handler (a.k.a. "slot")

 - `emit()` to propagate an event to the registered callback handlers

Our demo defines two signals in [demo-signals.h++](cpp/api/demo-signals.h++) to
support the functionality described above:

- `demo::signal_time` is emitted at periodic intervals once
  `start_ticking()` has been invoked, and

- `demo::signal_greeting` is emitted whenever someone (a thread or an
  external client) invokes `say_hello()`.


Implementations
---------------

Four different implementtations of this API are included here, illustrating
different building blocks within the Common Core codebase.  Each implementation
includes a linkable library with a C++ class derived from the above API, more
specifically as it is defined in [demo-api.h++](cpp/api/demo-api.h++):

1. A [native library](cpp/impl/native/README.md) that directly implements
   the underlying functionality within a single process. Notifications are
   emitted directly via the signals described above.

2. A [gRPC adapter](cpp/impl/grpc/README.md), comprising a client/server pair
   where the client implements the API by forwarding invocations as RPC calls to
   the server. In turn, the server handles the call by invoking the same API
   method locally, and returns the result back to the client.

   Signals emitted in the server space are captured in a
   [queue](cpp/impl/grpc/server/demo-grpc-signalqueue.h++), and from there
   streamed back to the client where they are read and then re-emitted locally
   as signals.

   The gRPC service definition as well as the required ProtoBuf message
   structures can be found in the [proto](proto) folder, in
   [demo_service.proto](proto/demo_service.proto) and
   [demo_types.proto](proto/demo_service.proto), respectively.

   For more information about using gRPC and Protocol Buffers, see
   [grpc.io](https://grpc.io) and [protobuf.dev](https://protobuf.dev),
   respectively.

3. A [DDS adapter](cpp/impl/dds/README.md), similarly comprising a client/server
   pair. Similar to gRPC, a dedicated [RPC client](cpp/impl/dds/rpc-client)
   implements the API by forwarding requests to a corresponding [RPC
   server](cpp/impl/dds/rpc-server) where it is handled natively.

   In this case signals are captured in the server and propagated to the client
   via a [DDS Publisher](cpp/impl/dds/dds-publisher). The client subscribes to
   these events within its [DDS Subscriber](cpp/impl/dds/dds-subscriber)
   module. Once received, the corresponding signals are emitted within the
   client process.

   Interface Definition Language files containing the DDS service defintions
   along with associated data types can be found in the [idl](idl) folder.  DDS
   and IDL are documented in more detail at
   [OMG](https://www.omg.org/omg-dds-portal/). (If you use a search engine,
   please specify `OMG IDL`, as simply searching for `IDL` will yield a lot of
   false leads).

4. A [ZeroMQ adapter](cpp/impl/zmq). Like above, a [demo
   client](cpp/impl/zmq/zmq-client) establishes a connection to a remote [demo
   server](cpp/impl/zmq/zmq-server).  Also as with DDS, events are captured on
   the server side and passed to the client with a [demo
   publisher](cpp/impl/zmq/zmq-publisher), where a corresponding [demo
   subscriber](cpp/impl/zmq/zmq-subscriber) reads and then forwards these as
   signals within the local process space.

   One twist in this case is that ZeroMQ does not prescribe any particular
   message structure, and indeed treats payloads as opaque blobs.  Put
   differently, we need to provide our own message encoding.  Well, didn't we
   already do that? Since we already adopted a couple of structure declaration
   languages above, why not simply reuse one of those?

   Indeed, for this purpose we'll reuse the ProtoBuf message types from
   [demo_types.proto](proto/demo_types.proto), only this time we will make use
   of our custom [ZMQ ProtoBuf
   wrappers](../../core/cpp/outer-core/messaging/zeromq/zmq-protobuf/README.md)to
   implement the RPC portion.


#### Interchangability

Since our implementations all derive from the same abstract API, they are
interchangable from the perspective of the client/application.  In fact, neither
the abstract API nor the *native* flavor has any dependencies on gRPC, DDS, or
ZeroMQ; any of these implementation libraries can be excluded or replaced with
something else altogether.

### Python modules

Coming soon.


Communications interface
------------------------

As implemented here, `demoserver` and its peers communicate over a [Data
Distribution Service](https://en.wikipedia.org/wiki/Data_Distribution_Service)
(DDS) bus. Data types used on this bus are described the [Interface Definition
Language](https://en.wikipedia.org/wiki/Interface_description_language) (IDL),
located in the [`idl` folder](idl).


Executables
-----------

The above C++ libraries are then linked into the following executables:

* [demo-tool](cpp/utils/tool) is a command-line utility that provides access to
  the demo API via either of the above implementations. A command-line switch
  selects whether to load the native implementation internally or connect to an
  external server over DDS.

* [demoserver](cpp/daemon) is a server executable (daemon) that serves requests
  from and produces notifications to its clients, such as this `demotool`.


Directory Structure
-------------------

Our demo application is organized into the following parts:

1. [C++ Application](cpp/README.md)
   * [API](cpp/api/README.md)
   * [Implementations](cpp/impl/README.md), contianing the native library as well as client and server libraries for each messaging platform.
   * [Server Executable (Daemon)](cpp/daemon), supporting all of the above
     messaging interfaces
   * [Command Line Tools](cpp/utils), one for each messaging platform

2. [Python Client Modules](python). These can be used interactively, or used
   as modules into client applications.

3. [ProtoBuf/gRPC modules](proto), describing the message types we use to
   communicate over either gRPC or ZeroMQ, as well as the gRPC service
   definition. These are converted to C++ and Python source code via the CMake
   recipe [BuildProtodefs.cmake](../../../cmake/BuildProtodefs.cmake), and can
   just as easily be converted into source code for other languages such as C#.

4. [IDL](idl), containing Interface Defintion Language files that describe the
   data types and RPC interfaces used for communicating over DDS.  These are
   converted to C++ source code via the CMake recipe
   [BuildIDL.cmake](../../../cmake/BuildIDL.cmake), and can similarly be
   converted into source code for other languages such as C#. They are also
   optionally converted to an XML file, which can be loaded into RTI's Python
   support library.

5. [Settings files](settings) in `.json` format. These match the name of the
   executables (i.e., `demoserver.json`, `demo-grpc-tool.json`), and are thus
   loaded automatically into a global `settings` object within the corresponding
   processes when launched.


Questions? Comments? Requests?
------------------------------

Please get in touch:

> Tor Slettnes  
> tor@slett.net  
> +1.925.8888.TOR
