C++ gRPC service utilities
==========================

This contains custom additions to [gRPC](https://grpc.io/).

All of these modules are implemented in the `cc::grpc::` namespace.

Modules
-------
 * `grpc-base` -- Common functionality: settings, service address lookup, ...
   * `grpc-clientbase` -- Client funcationality: host lookup, server/request status, 
      * `grpc-clientwrapper` -- Client template for specific service
        * `grpc-signalclient` -- Client template with signal listener
   * `grpc-requesthandler` -- Server-side request handler
      * `grpc-signalservice` -- Server-side request handler with `Signal()` support
   * `grpc-clientwrapper` -- Client-side additions: server availability, check return status, ...
`Signal<>` in client
 * `grpc-clientreceiver` - Stream messages from server in background
 * `grpc-serverstreamer` - BlockingQueue for streaming messages to client
 * `grpc-signalqueue` -- BlockingQueue for streaming `Signal()` messages to client
 * `grpc-serverbuilder` -- Specialized `::grpc::ServerBuilder()` with `add_service()` 
 * `grpc-status` -- Derived from `grpc::Status` to provide additional error details
