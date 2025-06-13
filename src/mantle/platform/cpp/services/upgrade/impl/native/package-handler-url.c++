// -*- c++ -*-
//==============================================================================
/// @file package-handler-url.c++
/// @brief Upgrade from a HTTP source
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-handler-url.h++"
#include "http-client.h++"
#include "status/exceptions.h++"
#include "platform/process.h++"

#include <future>

namespace upgrade::native
{
    URLPackageHandler::URLPackageHandler(const core::SettingsStore::ptr &settings)
        : Super(settings)
    {
    }

    void URLPackageHandler::unpack(const PackageSource &source,
                                   const fs::path &staging_folder)
    {
        if (source.empty())
        {
            throw core::exception::MissingArgument("Missing package location");
        }

        this->emit_upgrade_progress(UpgradeProgress::STATE_UNPACKING);
        this->unpack_url(source.url(), staging_folder);
    }

    void URLPackageHandler::unpack_url(const URL &url,
                                       const fs::path &staging_folder) const
    {
        core::platform::Pipe pipe = core::platform::process->create_pipe();
        core::http::HTTPClient http;

        core::http::HTTPClient::ReceiveFunction receiver = std::bind(
            &core::platform::ProcessProvider::write_fd,  // method
            core::platform::process.get(),               // object
            pipe.at(core::platform::OUTPUT),             // fd
            std::placeholders::_1,
            std::placeholders::_2);

        // [=](const char *data, std::size_t size) {
        //     core::platform::process->write_fd(pipe.at(core::platform::OUTPUT),
        //                                       data,
        //                                       size);
        // };

        std::future<void> future = std::async(
            &This::unpack_from_fd,
            this,
            pipe.at(core::platform::INPUT),
            staging_folder);

        std::exception_ptr eptr;

        try
        {
            http.get(url,       // location
                     nullptr,   // content_type
                     {},        // header_receiver
                     receiver,  // content_receiver
                     true);     // fail_on_error

            core::platform::process->close_fd(pipe.at(core::platform::OUTPUT));
            future.wait();
        }
        catch (...)
        {
            core::platform::process->close_pipe(pipe);
            throw;
        }
    }

    core::http::HTTPClient::ReceiveFunction URLPackageHandler::piper(
        core::platform::FileDescriptor fd)
    {
        return [=](const char *data, std::size_t size) -> void {
            core::platform::process->write_fd(fd, data, size);
        };
    }

}  // namespace upgrade::native
