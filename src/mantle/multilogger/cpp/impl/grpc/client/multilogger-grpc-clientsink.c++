/// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-clientsink.c++
/// @brief Log to remote MultiLogger service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-grpc-clientsink.h++"

namespace multilogger::grpc
{
    const std::string SETTING_HOST = "host";
    const std::string DEFAULT_HOST = "localhost";

    ClientSink::ClientSink(const std::string &sink_id)
        : Super(sink_id),
          host_(DEFAULT_HOST)
    {
    }

    void ClientSink::load_settings(const core::types::KeyValueMap &settings)
    {
        Super::load_settings(settings);
        this->load_client_settings(settings);
    }

    void ClientSink::load_client_settings(const core::types::KeyValueMap &settings)
    {
        if (const core::types::Value &value = settings.get(SETTING_HOST))
        {
            this->set_host(value.as_string());
        }
    }

    std::string ClientSink::host() const
    {
        return this->host_;
    }

    void ClientSink::set_host(const std::string &address)
    {
        this->host_ = address;
    }

    void ClientSink::open()
    {
        if (!this->client_)
        {
            this->client_ = grpc::LogClient::create_shared(
                this->host(),   // host
                true);          // wait_for_ready

            this->client_->open_writer();
        }
        Super::open();
    }

    void ClientSink::close()
    {
        Super::close();
        if (std::shared_ptr<LogClient> client = this->client_)
        {
            client->close_writer();
            this->client_.reset();
        }
    }

    bool ClientSink::handle_item(const core::types::Loggable::ptr &loggable)
    {
        if (std::shared_ptr<LogClient> client = this->client_)
        {
            return client->write_item(loggable);
        }
        else
        {
            return false;
        }
    }

}  // namespace multilogger::grpc
