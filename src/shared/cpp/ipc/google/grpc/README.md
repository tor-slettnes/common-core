C++ gRPC service utilities
==========================

This contains custom additions to [gRPC](https://grpc.io/).

All of these modules are implemented in the `cc::grpc::` namespace.

Modules
-------
 * `grpc-basewrapper` -- Common functionality: settings, service address lookup, ...
   * `grpc-serverwrapper` -- Server-side additions: map exceptions to gRPC failure status, log, ...
      * `grpc-signalserver` -- Connects to signals on server and stream events to client via `Signal()` class
   * `grpc-clientwrapper` -- Client-side additions: server availability, check return status, ...
      * `grpc-signalclient` -- Reads `Signal()` streams and emits corresponding `Signal<>` in client
 * `grpc-signalqueue` -- Derived from `BlockingQueue()` to propagate signals to client sessions
 * `grpc-status` -- Derived from `grpc::Status` to provide additional error details
