// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-message-writer.c++
/// @brief Server-side log reader that forwards log events to client over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-zmq-message-writer.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-inline.h++"

namespace multilogger::zmq
{
    MessageWriter::MessageWriter(
        const std::shared_ptr<API> &provider,
        const std::shared_ptr<core::zmq::Publisher> &publisher)
        : Super(publisher),
          provider(provider),
          signal_handle(TYPE_NAME_FULL(This)),
          listener_spec({
              .sink_id = "zmq-publisher",
          })
    {
    }

    void MessageWriter::initialize()
    {
        Super::initialize();
        using namespace std::placeholders;
        multilogger::signal_log_item.connect(
            this->signal_handle,
            std::bind(&This::on_log_item, this, _1));

        this->provider->start_listening(this->listener_spec);
    }

    void MessageWriter::deinitialize()
    {
        logf_debug("Stopping listener");
        this->provider->stop_listening(false);

        logf_debug("Disconnecting handler %r from signal", this->signal_handle);
        multilogger::signal_log_item.disconnect(
            this->signal_handle);

        logf_debug("Super::deinitialize()");

        Super::deinitialize();
        logf_debug("deinitialize() done");
    }

    void MessageWriter::on_log_item(const core::types::Loggable::ptr &item)
    {
        try
        {
            this->forward(
                protobuf::encoded_shared<cc::platform::multilogger::Loggable>(item));
        }
        catch (...)
        {
        }
    }

}  // namespace multilogger::zmq
