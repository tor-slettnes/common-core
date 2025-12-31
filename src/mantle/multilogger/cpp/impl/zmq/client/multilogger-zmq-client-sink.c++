// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client-sink.c++
/// @brief Log sink that forwards messages to MultiLogger over ZMQ
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-zmq-client-sink.h++"
#include "multilogger-zmq-common.h++"
#include "protobuf-multilogger-types.h++"

namespace multilogger::zmq
{
    ClientSink::ClientSink(const std::string &sink_id,
                           const std::shared_ptr<core::zmq::Publisher> &publisher)
        : Super(sink_id),
          host_(DEFAULT_HOST),
          external_publisher_(bool(publisher)),
          publisher_(publisher)
    {
    }

    void ClientSink::load_settings(const core::types::KeyValueMap &settings)
    {
        Sink::load_settings(settings);
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
        Super::open();
        if (!this->publisher_)
        {
            this->publisher_ = std::make_shared<core::zmq::Publisher>(
                this->host(),
                SUBMIT_CHANNEL,
                core::zmq::Endpoint::Role::SATELLITE);
            this->publisher_->initialize();
        }

        if (!this->writer_)
        {
            this->writer_ = ClientWriter::create_shared(this->publisher_);
            this->writer_->initialize();
        }
    }

    void ClientSink::close()
    {
        if (this->writer_)
        {
            this->writer_->deinitialize();
            this->writer_.reset();
        }

        if (this->publisher_ && !this->external_publisher_)
        {
            this->publisher_->deinitialize();
            this->publisher_.reset();
        }

        Super::close();
    }

    bool ClientSink::handle_item(const core::types::Loggable::ptr &loggable)
    {
        if (this->writer_)
        {
            this->writer_->write(loggable);
            return true;
        }
        else
        {
            return false;
        }
    }
}  // namespace multilogger::zmq
