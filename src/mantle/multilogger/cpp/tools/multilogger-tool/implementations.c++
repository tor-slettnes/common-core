// -*- c++ -*-
//==============================================================================
/// @file implementations.c++
/// @brief Enumerate available MultiLogger API implementations
/// @author Tor Slettnes
//==============================================================================

#include "implementations.h++"

namespace multilogger
{
    core::types::SymbolMap<Implementation> implementation_names =
        {
            {Implementation::GRPC, "grpc"},
            {Implementation::ZMQ, "zmq"},
    };

    std::ostream &operator<<(std::ostream &stream, const Implementation &impl)
    {
        return implementation_names.to_stream(stream, impl);
    }

    // Convert to the above enumeration from a string
    std::istream &operator>>(std::istream &stream, Implementation &impl)
    {
        return implementation_names.from_stream(
            stream,                // stream
            &impl,                 // key
            Implementation::GRPC,  // fallback
            true);                 // flag_unknown
    }

}  // namespace multilogger
