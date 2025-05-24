// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Parse commmand line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "settings/settings.h++"
#include "multilogger-grpc-clientsink.h++"

namespace switchboard
{
    Options::Options()
        : Super(),
          enable_grpc(false),
          enable_dds(false)
    {
        this->describe("Switchboard Service.");
    }

    void Options::add_options()
    {
        Super::add_options();

#if USE_GRPC
        this->add_flag(
            {"--grpc", "--enable-grpc"},
            "Enable gRPC service",
            &this->enable_grpc,
            core::settings->get("enable grpc", true).as_bool());
#endif

#if USE_DDS
        this->add_flag(
            {"--dds", "--enable-dds"},
            "Enable DDS service",
            &this->enable_dds,
            core::settings->get("enable dds", true).as_bool());
#endif
    }

    std::unique_ptr<Options> options;
}  // namespace switchboard
