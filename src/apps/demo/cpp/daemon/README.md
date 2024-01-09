Common Core Apps Example - Server Executable
=============================================

The `exampleserver` executable provides backend/indirect access to our example app's core functionality on behalf of its its clients, such the provided [example-tool utility](../utils/tool).  It does so by linking in the following libraries:

* [cc_example_native](../impl/native/README.md), the core library, which is the ultimate endpoint for requests,

* [cc_example_dds_publisher](../impl/dds/dds-publisher), containing the DDS publisher,

* [cc_example_dds_server](../impl/dds/rpc-server), containing the DDS server interface (derived from code generated from [example-interface.idl](../../idl/example-interface.idl)).


### Start services

If deployed on actual system computer this step would be done automatically by the operating system as part of its startup sequence. However, for our interactive use we need to start these manually.

### That's it.

The server is now ready to serve incoming requests. Refer to the additional README files for the [exsample-tool](../utils/tool/README.md) utilty for additional instructions on how to proceed with the actual tests.

