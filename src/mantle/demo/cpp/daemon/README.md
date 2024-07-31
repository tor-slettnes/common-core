Common Core Demo - Server Executable
====================================

The `demoserver` executable provides backend/indirect access to the demo app's functionality on behalf of its its clients, such the provided [demo-grpc-tool](../utils/grpc-tool) and [demo-dds-tool](../utils/dds-tool) utilities.  It does so by linking in the following libraries:

* [cc_demo_native](../impl/native/README.md), the core library, which is the ultimate endpoint for requests,

* [cc_demo_grpc_service](../impl/grpc/server), containng the gRPC server, which in turn implements the gRPC interface declared in [demo.proto](../../proto/demo.proto).

* [cc_dds_publisher](../impl/dds/dds-publisher), containing the DDS publisher,

* [cc_dds_server](../impl/dds/rpc-server), containing the DDS server interface (derived from code generated from [demo-interface.idl](../../idl/demo-interface.idl)).


### Start services

If deployed on actual system computer this step would be done automatically by the operating system as part of its startup sequence. However, for our interactive use we need to start these manually.

### That's it.

The server is now ready to serve incoming requests. Refer to the additional README files for the [demo-dds-tool](../utils/dds-tool/README.md) utilty for additional instructions on how to proceed with the actual tests.

