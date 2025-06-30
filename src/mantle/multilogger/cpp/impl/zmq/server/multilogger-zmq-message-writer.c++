// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-message-writer.c++
/// @brief Server-side log reader that forwards log events to client over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-zmq-message-writer.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-inline.h++"
#include "status/exceptions.h++"

namespace multilogger::zmq
{
    MessageWriter::MessageWriter(
        const std::shared_ptr<API> &provider,
        const std::shared_ptr<core::zmq::Publisher> &publisher)
        : Super(publisher),
          provider(provider),
          keep_writing(false),
          listener_spec({
              .sink_id = "zmq-publisher",
          })
    {
    }

    MessageWriter::~MessageWriter()
    {
        this->stop();
        if (std::thread t = std::move(this->worker_thread); t.joinable())
        {
            t.detach();
        }
    }

    void MessageWriter::initialize()
    {
        Super::initialize();
        this->start();
    }

    void MessageWriter::deinitialize()
    {
        this->stop();
        Super::deinitialize();
    }

    void MessageWriter::start()
    {
        if (!this->keep_writing)
        {
            this->keep_writing = true;
            this->listener = this->provider->listen(this->listener_spec);
            this->worker_thread = std::thread(&This::worker, this);
        }
    }

    void MessageWriter::stop()
    {
        if (this->keep_writing)
        {
            this->keep_writing = false;
            this->listener->close();
        }
    }

    void MessageWriter::worker()
    {
        try
        {
            while (this->keep_writing)
            {
                if (const auto &item = this->listener->get())
                {
                    this->forward(
                        protobuf::encoded_shared<cc::platform::multilogger::Loggable>(
                            item.value()));
                }
            }
        }
        catch (...)
        {
            logf_notice("ZMQ message writer failed to send log item; exiting: %s",
                        std::current_exception());
        }
    }

}  // namespace multilogger::zmq
