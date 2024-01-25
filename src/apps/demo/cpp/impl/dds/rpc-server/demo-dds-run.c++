// -*- c++ -*-
//==============================================================================
/// @file demo-dds-server.c++
/// @brief C++ demo - Launch DDS server implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-dds-run.h++"
#include "dds-server.h++"          // RPC service
#include "demo-dds-publisher.h++"  // DDS publisher
#include "demo-dds-service.h++"    // RPC service

namespace cc::demo::dds
{
    void run_dds_service(
        std::shared_ptr<cc::demo::API> api_provider,
        const std::string &identity,
        int domain_id)
    {
        // A server provides the execution environment for one or more RPC services
        auto server = cc::dds::Server();

        // Instantiate Publisher to relay asynchronous events over DDS
        auto dds_publisher = cc::demo::dds::Publisher::create_shared(identity, domain_id);

        // Instantiate DDS RPC Service
        auto rpc_service = cc::demo::dds::Service(server, domain_id, api_provider);

        //======================================================================
        // Initialize

        logf_debug("Initializing Demo DDS publisher %s", *dds_publisher);
        dds_publisher->initialize();

        logf_debug("Initializing Demo DDS RPC service: %s", rpc_service);
        rpc_service.initialize();

        logf_debug("Initializing Demo DDS RPC server");
        server.initialize();

        //======================================================================
        // Run

        logf_info("Demo DDS service is ready in domain %d", domain_id);
        server.run();
        logf_info("Demo DDS service is shutting down");

        //======================================================================
        // Shut down

        logf_debug("Deinitializing Demo DDS RPC server");
        server.deinitialize();

        logf_debug("Deinitializing Demo DDS RPC service: %s", rpc_service);
        rpc_service.deinitialize();

        logf_debug("Deinitializing Demo DDS publisher %s", *dds_publisher);
        dds_publisher->deinitialize();

        logf_debug("Demo DDS service ended");
    }
}  // namespace cc::demo::dds
