// -*- c++ -*-
//==============================================================================
/// @file server.c++
/// @brief Parse commmand line options for server applications
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "server.h++"
#include "buildinfo/buildinfo.h++"
#include "platform/path.h++"
#include "platform/process.h++"
#include "platform/systemservice.h++"
#include "io/cutils.h++"

#include <errno.h>   // errno
#include <stdio.h>   // freopen(), stdin, stdout, stderr
#include <string.h>  // strerror()
#include <fcntl.h>   // O_RDONLY, O_WRONLY
#include <fstream>
#include <unistd.h>

namespace core::argparse
{
    //==========================================================================
    // ServerOptions methods

    ServerOptions::ServerOptions()
        : Super()
    {
    }

    void ServerOptions::add_options()
    {
        Super::add_options();
        this->add_flag(
            {"-D", "--daemon"},
            "Detach from controlling terminal and run in background.",
            &this->daemon);

        this->add_opt<fs::path>(
            {"--pid-file", "--pidfile"},
            "PIDFILE",
            "Write the server's process ID to the specified file.",
            &this->pidfile);

        this->add_void(
            {"--install-service"},
            "Install system service, then quit.",
            std::bind(&platform::SystemServiceProvider::install, platform::system_service));

        this->add_opt(
            {"-b", "--bind"},
            "ADDRESS",
            "Listener address in the form [INTERFACE][:PORT]. "
            "Default values are found in the endpoint settings file. "
            "To bind only to the loopback (localhost) "
            "interface use \"localhost\".",
            &this->bind_address);
    }

    void ServerOptions::enact()
    {
        if (this->daemon)
        {
            this->daemonize();
        }
        else if (!this->pidfile.empty())
        {
            std::ofstream pidstream = this->openpidfile(this->pidfile);
            pidstream << platform::process->process_id() << std::endl;
        }
        CommonOptions::enact();
        logf_notice("Started %s version %s",
                    platform::path->exec_name(),
                    PROJECT_VERSION);
    }

    void ServerOptions::daemonize()
    {
        std::ofstream pidstream = this->openpidfile(this->pidfile);
        platform::PID mychild = platform::process->fork_process();

        if (mychild > 0)
        {
            /// Parent saves PID file and exits
            if (pidstream.is_open())
            {
                pidstream << mychild << std::endl;
            }
            exit(0);
        }
        else if (mychild == 0)
        {
            // Child closes stdin, stdout, stderr
            io::checkstatus(std::freopen(platform::path->devnull().c_str(), "r", stdin));
            io::checkstatus(std::freopen(platform::path->devnull().c_str(), "w", stdout));
            io::checkstatus(std::freopen(platform::path->devnull().c_str(), "w", stderr));
        }
        else
        {
            this->fail("Could not fork() child process: "s + strerror(errno));
        }
    }

    std::ofstream ServerOptions::openpidfile(const fs::path &pidfile)
    {
        std::ofstream pidstream;
        if (!pidfile.empty())
        {
            pidstream.open(pidfile);
            if (pidstream.fail())
            {
                this->fail("Unable to open PID file \"" + pidfile.string() +
                           "\": " + strerror(errno));
                exit(-1);
            }
        }
        return pidstream;
    }

}  // namespace core::argparse
