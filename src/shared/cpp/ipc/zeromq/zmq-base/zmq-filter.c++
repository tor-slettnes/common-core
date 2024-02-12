/// -*- c++ -*-
//==============================================================================
/// @file zmq-filter.c++
/// @brief ZMQ subscription filter insertion/extraction
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-filter.h++"

#include <string.h>  // memcpy

namespace shared::zmq
{
    Filter Filter::create_from_data(void *data, std::size_t size)
    {
        Filter filter;
        if (std::size_t filter_size = Filter::extract_filter_size(data, size))
        {
            memcpy(filter.data(), data, filter_size);
        }
        return filter;
    }

    Filter Filter::create_from_topic(const std::string &topic)
    {
        Filter filter;

        std::vector<types::Byte> preamble = Filter::encoded_size(topic.size());
        filter.reserve(preamble.size() + topic.size());
        filter.assign(preamble.begin(), preamble.end());
        filter.insert(filter.end(), topic.begin(), topic.end());
        return filter;
    }

    std::vector<types::Byte> Filter::encoded_size(std::size_t size)
    {
        std::vector<types::Byte> encoding;
        encoding.reserve(Filter::encoded_size_size(size));

        for (bool more = true; more; size >>= 7)
        {
            more = (size > 0x7F);
            encoding.push_back((size & 0x7F) | (more ? 0x80 : 0x00));
        }
        return encoding;
    }

    std::size_t Filter::encoded_size_size(std::size_t size)
    {
        std::size_t encoding_size = 1;
        for (std::size_t n = size; n > 0x7F; n >>= 7)
        {
            encoding_size++;
        }
        return encoding_size;
    }

    std::size_t Filter::extract_filter_size(const void *data,
                                            std::size_t data_size,
                                            std::size_t *preamble_size_ptr,
                                            std::size_t *topic_size_ptr)
    {
        std::size_t preamble_size = 0;
        std::size_t topic_size = 0;
        auto *byte = reinterpret_cast<const types::Byte *>(data);

        for (bool more = true;
             more && (preamble_size < data_size);
             preamble_size++, byte++)
        {
            topic_size = (topic_size << 7) | (*byte & 0x7F);
            more = (*byte & 0x80) != 0;
        }

        if (preamble_size_ptr)
        {
            *preamble_size_ptr = preamble_size;
        }
        if (topic_size_ptr)
        {
            *topic_size_ptr = topic_size;
        }

        if (preamble_size + topic_size <= data_size)
        {
            return preamble_size + topic_size;
        }
        else
        {
            return 0;
        }
    }

    std::string Filter::topic() const
    {
        std::size_t startpos = 0;
        std::size_t length = 0;
        if (Filter::extract_filter_size(this->data(), this->size(), &startpos, &length))
        {
            return std::string(this->begin() + startpos,
                               this->begin() + startpos + length);
        }
        else
        {
            return {};
        }
    }

}  // namespace shared::zmq
