/// -*- c++ -*-
//==============================================================================
/// @file dds-publisher.h++
/// @brief A publishing entity/producer in a DDS environment
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "dds-endpoint.h++"
#include "thread/signaltemplate.h++"
#include "logging/logging.h++"

#include <dds/pub/ddspub.hpp>
#include <dds/domain/DomainParticipant.hpp>

namespace core::dds
{
    //==========================================================================
    // @class PubSubChannnel

    class Publisher : public Endpoint,
                      public ::dds::pub::Publisher
    {
        using Super = Endpoint;
        using This = Publisher;

    protected:
        using DataWriterQos = ::dds::pub::qos::DataWriterQos;

    public:
        template <class T>
        using DataWriterPtr = std::shared_ptr<::dds::pub::DataWriter<T>>;

    public:
        Publisher(const std::string &channel_name, int domain_id);

    public:
        //======================================================================
        /// @brief Create a new DDS writer instance on this publisher channel
        /// @param[in] topic_name
        ///     DDS publication topic.  By default we use the fully qualified
        ///     name of the data type.
        /// @param[in] reliable
        ///     Whether to assign the RELIABLE or BEST_EFFORT QoS to this topic
        /// @param[in] sync_latest
        ///     Whether to assign the TRANSIENT_LOCAL or VOLATILE QoS to this topic

        template <class T>
        std::shared_ptr<::dds::pub::DataWriter<T>>
        create_writer(const std::string &topic_name,
                      bool reliable = true,
                      bool sync_latest = false)
        {
            logf_debug("create_writer(topic_name=%r)", topic_name);
            ::dds::topic::Topic<T> topic(this->get_participant(), topic_name);
            auto qos = this->pubsub_policy<::dds::pub::qos::DataWriterQos>(
                reliable,
                sync_latest);

            return std::make_shared<::dds::pub::DataWriter<T>>(*this, topic, qos);
        }

        //======================================================================
        /// @brief Helper method to forward encoded messages received as signals
        /// @tparam T
        ///     DDS Data Type
        /// @param[in] writer
        ///     DDS writer.
        /// @param[in] update
        ///     The payload to be published

        template <class T>
        void publish(std::shared_ptr<::dds::pub::DataWriter<T>> &writer, const T &update)
        {
            logf_trace("Publishing: %s", update);
            writer->write(update);
        }

        //======================================================================
        /// @brief Helper method to forward encoded messages received as signals
        /// @param[in] writer
        ///     DDS writer.
        /// @param[in] mapping_action
        ///     Mapping action, informing the receiver whether this is an
        ///     addition/update or removal.
        /// @param[in] update
        ///     The payload to publish

        template <class T>
        void publish_change(std::shared_ptr<::dds::pub::DataWriter<T>> &writer,
                            core::signal::MappingAction mapping_action,
                            const T &update)
        {
            switch (mapping_action)
            {
            case core::signal::MAP_NONE:
                break;

            case core::signal::MAP_ADDITION:
            case core::signal::MAP_UPDATE:
                this->publish<T>(writer, update);
                break;

            case core::signal::MAP_REMOVAL:
                logf_trace("Disposing: %s", writer->lookup_instance(update));
                writer->dispose_instance(writer->lookup_instance(update));
                break;
            }
        }
    };

}  // namespace core::dds
