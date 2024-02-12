/// -*- c++ -*-
//==============================================================================
/// @file grpc-signalstream-server.h++
/// @brief Common Core service provider with signal emitting capability
/// @author Tor Slettnes <tor@slett.net>
///
/// Class templates for gRPC services (client and server), including:
///  * Settings store in YourServiceName.json, using JsonCpp as backend
///  * Status/error code wrappers
//==============================================================================

#pragma once
#include "grpc-serverwrapper.h++"
#include "grpc-signalqueue.h++"

#include "platform/symbols.h++"
#include "thread/blockingqueue.h++"
#include "thread/signaltemplate.h++"

namespace shared::grpc
{
    //==========================================================================
    /// @class SignalWrapper<ServerType, SignalType, SignalQueueType>
    /// @brief Server wrapper with signalling interface
    /// @tparam ServiceType
    ///     gRPC service type
    /// @tparam SignalType
    ///     Signal class, derfined in .proto file

    template <class ServiceType>
    class SignalServer : public ServerWrapper<ServiceType>
    {
        using Super = ServerWrapper<ServiceType>;

        template <class DataType, class MessageType>
        using Encoder = std::function<void(DataType, MessageType *)>;

        template <class DataType, class MessageType, class KeyType = std::string>
        using MappingEncoder = std::function<
            void(signal::MappingChange, KeyType, DataType, MessageType *)>;

    protected:
        using Super::Super;

    protected:
        template <class DataType, class MessageType>
        ::grpc::Status connect_stream(
            signal::Signal<DataType> *signal,
            const Encoder<DataType, MessageType> &encoder,
            ::grpc::ServerContext *cxt,
            ::grpc::ServerWriter<MessageType> *writer)
        {
            std::string slot_id = platform::symbols->uuid();

            try
            {
                // Create a new blocking queue and connect `signal` to its
                // `put()` method.  As signals are emitted, they are thus
                // encoded and added to the queue.
                types::BlockingQueue<MessageType> queue;

                signal->connect(
                    slot_id,                   // id
                    [&](const DataType &data)  // callback
                    {
                        MessageType msg;
                        encoder(data, &msg);
                        queue.put(std::move(msg));
                    });

                // As messages are pulled from this queue,
                // feed them back to the client.
                this->stream_from_queue(&queue, cxt, writer);

                // Disconnect from the signal.
                signal->disconnect(slot_id);
                return grpc::Status::OK;
            }
            catch (...)
            {
                signal->disconnect(slot_id);
                return this->failure(std::current_exception(),
                                     str::format("streaming to %s", cxt->peer()));
            }
        }

        template <class DataType, class MessageType, class KeyType = std::string>
        ::grpc::Status connect_stream(
            signal::MappingSignal<DataType> *mappingsignal,
            const MappingEncoder<DataType, MessageType, KeyType> &encoder,
            ::grpc::ServerContext *cxt,
            ::grpc::ServerWriter<MessageType> *writer)
        {
            std::string slot_id = platform::symbols->uuid();

            try
            {
                // Create a new blocking queue and connect `mappingsignal` to its
                // `put()` method.  As signals are emitted, they are thus
                // encoded and added to the queue.
                types::BlockingQueue<MessageType> queue;

                mappingsignal->connect(
                    slot_id,  // id
                    [&](signal::MappingChange change,
                        const KeyType &key,
                        const DataType &data)  // callback
                    {
                        MessageType msg;
                        encoder(change, key, data, &msg);
                        queue.put(std::move(msg));
                    });

                // As messages are pulled from this queue,
                // feed them back to the client.
                this->stream_from_queue(&queue, cxt, writer);

                // Disconnect from the signal.
                mappingsignal->disconnect(slot_id);
                return grpc::Status::OK;
            }
            catch (...)
            {
                mappingsignal->disconnect(slot_id);
                return this->failure(std::current_exception(),
                                     str::format("streaming to %s", cxt->peer()));
            }
        }

    private:
        template <class MessageType>
        void stream_from_queue(types::BlockingQueue<MessageType> *queue,
                               ::grpc::ServerContext *cxt,
                               ::grpc::ServerWriter<MessageType> *writer)
        {
            while (true)
            {
                std::optional<MessageType> msg = queue->get();
                if (cxt->IsCancelled())
                {
                    break;
                }

                if (msg)
                {
                    logf_trace("Sending to %s: %s", cxt->peer(), *msg);
                    writer->Write(*msg);
                }
            }
        }
    };
}  // namespace shared::grpc
