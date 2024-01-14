Common Core Demo Application
----------------------------

This demo application is provided to illustrate the morphology of a typical service-based applications based on the Common Core application framework. It may be used as a template for creating new applications, or as a reference implementation of current best practices.

For illustration, our demo comprises two distinct implementations of the same Application Programming Interface (API), described below. Each approach showcases different building blocks within our codebase, ranging from basic C++ utilites to client/server communication over gRPC.


Application Programming Interface
---------------------------------

Our demo API comprises just a couple of seemingly random methods:

* `say_hello(Greeting)` to issue a greeting to whomever may be listening

* `get_current_time() -> TimeData` returns the current time

* `start_ticking()` to start issuing time notifications at regular intervals (1x/second)

* `stop_ticking()` to stop issuing time notifications updates

The C++ flavor of this API is declared in [demo-api.h++](cpp/api/demo-api.h++).


Event propagation
-----------------

Notifications are propagated internally within a process using an adaptation of the [signal/slot pattern](https://en.wikipedia.org/wiki/Signals_and_slots) pattern, which essentially boils down to a fancy way of doing callbacks. A _signal_ essentially represents the interface between the producers and consumers of a prescribed event. In this way it is similar to a _topic_ in the Pub/Sub pattern, albeit adapted for intra-process communication. Its two most important methods are:

 - `connect(callback_method)` to register a callback handler (a.k.a. "slot")

 - `emit()` to propagate an event to the registered callback handlers

Our demo defines two signals in [signals.h++](cpp/api/demo-signals.h++) to support the functionality described above:

- `cc::demo::signal_time` is emitted at periodic intervals once `start_ticking()` has been invoked, and

- `cc::demo::signal_greeting` is emitted whenever someone (a thread or an external client) invokes `say_hello()`.


Implementations
---------------

### Linkable C++ libraries

Three different C++ implementations are included here, illustrating different building blocks within the Common Core codebase.  Each approach implements the same abstract API, defined in [demo-api.h++](cpp/api/demo-api.h++):

1. A [C++ native library](cpp/impl/native/README.md) that directly implements the underlying functionality within a single process. Notifications are emitted directly via the signals described above.

2. A [gRPC adapter library](cpp/impl/grpc/README.md), comprising a client/server pair where the client implements the same C++ API by forwarding invocations as RPC calls to the server. Signals emitted in the server space are captured in a [queue](cpp/impl/grpc/server/demo-grpc-signalqueue.h++), and from there streamed back to the client where they are read and then re-emitted locally as signals.

3. A [DDS adapter library](cpp/impl/dds/README.md), similarly comprising a client/server pair. Similar to gRPC, a dedicated [RPC client](cpp/impl/dds/rpc-client) implements the API by forwarding requests to a corresponding [RPC server](cpp/impl/dds/rpc-server) where it is fulfilled natively. However, in this case signals are captured in the server and propagated to the client via a [DDS Publisher](cpp/impl/dds/dds-publisher); the client receives these and re-emits signals locally within a [DDS Subscriber](cpp/impl/dds/dds-subscriber). module.

#### Interchangability

Since these two implementations derive from the same abstract API, they are interchangable from the perspective of the client/application.  In fact, neither the abstract API nor the *native* flavor has any gRPC or RTI/DDS dependencies; it is perfectly conceivable to replace one with the other, or something else altogether.

### Python modules

Coming soon.


Communications interface
------------------------

As implemented here, `demoserver` and its peers communicate over a [Data Distribution Service](https://en.wikipedia.org/wiki/Data_Distribution_Service) (DDS) bus. Data types used on this bus are described the [Interface Definition Language](https://en.wikipedia.org/wiki/Interface_description_language) (IDL), located in the [`idl` folder](idl).


Executables
-----------

The above C++ libraries are then linked into the following executables:

* [demo-tool](cpp/utils/tool) is a command-line utility that provides access to the demo API via either of the above implementations. A command-line switch selects whether to load the native implementation internally or connect to an external server over DDS.

* [demoserver](cpp/daemon) is a server executable (daemon) that serves requests from and produces notifications to its clients, such as this `demotool`.


Directory Structure
-------------------

Our demo application is organized into the following parts:

1. [C++ Application](cpp/README.md)
   * [API](cpp/api/README.md)
   * [Implementations](cpp/impl/README.md)
   * [Server Executable (Daemon)](cpp/daemon)
   * [Command Line Tool](cpp/utils/tool)

2. [ProtoBuf/gRPC modules](proto), describing the message types and service methods that are being used to communicate over gRPC. These are converted to C++ source code via the CMake recipe [BuildProtodefs.cmake](../../../cmake/BuildProtodefs.cmake), and can similarly be converted into source code for other languages such as C#.

3. [Application Settings](settings)


Questions? Comments? Requests?
------------------------------

Please get in touch:

> Tor Slettnes  
> tor@slett.net  
> +1.925.8888.TOR
