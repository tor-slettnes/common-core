// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-clientlistener.c++
/// @brief Listen to log events from server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-grpc-clientlistener.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"

#include "cc/platform/multilogger/multilogger.grpc.pb.h"

namespace multilogger::grpc
{
    //--------------------------------------------------------------------------
    // ClientListener

    ClientListener::ClientListener(
        const std::unique_ptr<::cc::platform::multilogger::MultiLogger::Stub> &stub,
        const ListenerSpec &request)
        : reader_(
            stub->Listen(
                &this->context_,
                protobuf::encoded<::cc::platform::multilogger::ListenerSpec>(request)))
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

    std::optional<core::types::Loggable::ptr> ClientListener::get()
    {
        cc::platform::multilogger::Loggable msg;
        if (this->reader_->Read(&msg))
        {
            return protobuf::decode_loggable(msg, this->context().peer());
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

}  // namespace multilogger::grpc
