// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-run.c++
/// @brief C++ switchboard - Launch DDS server implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switchboard-dds-run.h++"
#include "dds-server.h++"                 // RPC service
#include "switchboard-dds-publisher.h++"  // DDS publisher
#include "switchboard-dds-service.h++"    // RPC service

namespace switchboard::dds
{
    void run_dds_service(
        std::shared_ptr<switchboard::Provider> api_provider,
        const std::string &identity,
        int domain_id)
    {
        // A server provides the execution environment for one or more RPC services
        auto server = core::dds::Server();

        // Instantiate Publisher to relay asynchronous events over DDS
        auto dds_publisher = switchboard::dds::Publisher::create_shared(identity, domain_id);

        // Instantiate DDS RPC Service
        auto rpc_service = switchboard::dds::Service(server, domain_id, api_provider);

        //======================================================================
        // Initialize

        logf_info("Initializing Switchboard DDS publisher %s", *dds_publisher);
        dds_publisher->initialize();

        logf_info("Initializing Switchboard DDS RPC service: %s", rpc_service);
        rpc_service.initialize();

        logf_info("Initializing Switchboard DDS RPC server");
        server.initialize();

        //======================================================================
        // Run

        logf_notice("Switchboard DDS service is ready in domain %d", domain_id);
        server.run();
        logf_notice("Switchboard DDS service is shutting down");

        //======================================================================
        // Shut down

        logf_info("Deinitializing Switchboard DDS RPC server");
        server.deinitialize();

        logf_info("Deinitializing Switchboard DDS RPC service: %s", rpc_service);
        rpc_service.deinitialize();

        logf_info("Deinitializing Switchboard DDS publisher %s", *dds_publisher);
        dds_publisher->deinitialize();

        logf_info("Switchboard DDS service ended");
    }
}  // namespace switchboard::dds
