// -*- c++ -*-
//==============================================================================
/// @file relay-publisher.c++
/// @brief Relay publisher abstract base
/// @author Tor Slettnes
//==============================================================================

#include "relay-publisher.h++"

namespace relay
{
    Publisher::Publisher(std::size_t queue_size)
        : writer_queue_(queue_size)
    {
    }

    void Publisher::initialize()
    {
        this->start_writer();
    }

    void Publisher::deinitialize()
    {
        this->stop_writer();
    }

    void Publisher::publish(const std::string &topic,
                            const core::types::Value &payload)
    {
        this->writer_queue_.put({topic, payload});
    }

    void Publisher::start_writer()
    {
        if (!this->writer_thread_.joinable())
        {
            this->writer_thread_ = std::thread(&This::write_worker, this);
        }
    }

    void Publisher::stop_writer()
    {
        if (this->writer_thread_.joinable())
        {
            this->writer_queue_.close();
            this->writer_thread_.join();
        }
    }

    void Publisher::write_worker()
    {
        while (auto item = this->writer_queue_.get())
        {
            if (!this->write(item->first, item->second))
            {
                break;
            }
        }
    }

}  // namespace relay
