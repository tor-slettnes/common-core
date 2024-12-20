// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-clientlistener.c++
/// @brief Listen to log events from server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-grpc-clientlistener.h++"
#include "protobuf-logger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"

#include "logger.grpc.pb.h"

namespace logger::grpc
{
    //--------------------------------------------------------------------------
    // ClientListener

    ClientListener::ClientListener(
        const std::unique_ptr<::cc::logger::Logger::Stub> &stub,
        const ListenerSpec &request)
        : reader_(stub->listen(&this->context_,
                               protobuf::encoded<::cc::logger::ListenerSpec>(request)))
    {
        logf_debug("Created grpc::ClientListener(%s)", request);
    }

    ClientListener::~ClientListener()
    {
        this->close();
    }

    void ClientListener::close()
    {
        if (this->reader_)
        {
            this->context().TryCancel();
        }
    }

    std::optional<core::status::Event::ptr> ClientListener::get()
    {
        cc::status::Event msg;
        if (this->reader_->Read(&msg))
        {
            return protobuf::decoded<core::status::Event::ptr>(msg);
        }
        else
        {
            this->reader_->Finish();
            this->reader_.reset();
            return {};
        }
    }

    ::grpc::ClientContext &ClientListener::context()
    {
        return this->context_;
    }

}  // namespace logger::grpc
