/// -*- c++ -*-
//==============================================================================
/// @file zmq-filter.h++
/// @brief ZMQ subscription filter insertion/extraction
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/bytevector.h++"

#include <memory>
#include <vector>

namespace core::zmq
{
    class Filter : public types::ByteVector
    {
        using This = Filter;
        using Super = types::ByteVector;

    public:
        using Super::Super;

        static Filter create_from_data(void *data, std::size_t size);
        static Filter create_from_topic(const std::string &topic);
        static std::vector<types::Byte> encoded_size(std::size_t size);

    private:
        static std::size_t encoded_size_size(std::size_t size);
        static std::size_t extract_filter_size(const void *data,
                                               std::size_t data_size,
                                               std::size_t *preamble_size = nullptr,
                                               std::size_t *topic_size = nullptr);

    public:
        std::string topic() const;
    };

}  // namespace core::zmq
