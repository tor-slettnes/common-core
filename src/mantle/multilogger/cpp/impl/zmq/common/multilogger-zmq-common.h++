// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-common.h++
/// @brief Multilogger ZeroMQ implementation - common declarations
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace multilogger::zmq
{
    constexpr auto SUBMIT_CHANNEL = "Multilogger Submitter";
    constexpr auto MONITOR_CHANNEL = "Multilogger Monitor";

    // Interface name to use for RPC calls
    constexpr auto SERVICE_CHANNEL = "Multilogger Service";
    constexpr auto MULTILOGGER_INTERFACE = "Multilogger";

    // Method names within this interface
    constexpr auto METHOD_ADD_SINK = "add_sink";
    constexpr auto METHOD_REMOVE_SINK = "remove_sink";
    constexpr auto METHOD_GET_SINK = "get_sink";
    constexpr auto METHOD_GET_ALL_SINKS = "get_all_sinks";
    constexpr auto METHOD_LIST_SINKS = "list_sinks";
    constexpr auto METHOD_LIST_SINK_TYPES = "list_sink_types";
    constexpr auto METHOD_LIST_MESSAGE_FIELDS = "list_message_fields";
    constexpr auto METHOD_LIST_ERROR_FIELDS = "list_error_fields";

}  // namespace multilogger::zmq
