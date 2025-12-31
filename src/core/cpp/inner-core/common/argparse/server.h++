// -*- c++ -*-
//==============================================================================
/// @file server.h++
/// @brief Parse commmand line options for server applications
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "common.h++"

namespace core::argparse
{
    //==========================================================================
    /// Option parser for server applications
    class ServerOptions : public CommonOptions
    {
        using Super = CommonOptions;

    protected:
        ServerOptions();

    public:
        void add_options() override;
        void enact() override;
        void daemonize();

    private:
        std::ofstream openpidfile(const fs::path &path);

    public:
        bool install_service = false;
        bool daemon = false;
        fs::path default_pid_file;
        fs::path pidfile;
        std::string bind_address;
    };

}  // namespace core::argparse
