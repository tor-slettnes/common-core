/// -*- c++ -*-
//==============================================================================
/// @file dds-publisher.h++
/// @brief A publishing entity/producer in a DDS environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-channel.h++"
#include "thread/signaltemplate.h++"
#include "logging/logging.h++"

#include <dds/pub/ddspub.hpp>
#include <dds/domain/DomainParticipant.hpp>

namespace cc::dds
{
    //==========================================================================
    // @class DDS_PubSubChannnel

    class DDS_Publisher : public DDS_Channel,
                          public ::dds::pub::Publisher
    {
        using Super = DDS_Channel;
        using This = DDS_Publisher;

    protected:
        using DataWriterQos = ::dds::pub::qos::DataWriterQos;

    public:
        template <class T>
        using DataWriterRef = std::shared_ptr<::dds::pub::DataWriter<T>>;

    public:
        DDS_Publisher(const std::string &type,
                      const std::string &name,
                      int domain_id);

    public:
        //======================================================================
        /// @fn create_writer
        /// @brief Create a new DDS writer instance on this publisher channel
        /// @param[in] topic_name
        ///     DDS publication topic.  By default we use the fully qualified
        ///     name of the data type.
        /// @param[in] reliable
        ///     Whether to assign the RELIABLE or BEST_EFFORT QoS to this topic
        /// @param[in] sync_latest
        ///     Whether to assign the TRANSIENT_LOCAL or VOLATILE QoS to this topic

        template <class T>
        DataWriterRef<T>
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
        void publish(DataWriterRef<T> &writer, const T &update)
        {
            logf_trace("Publishing: %s", update);
            writer->write(update);
        }

        //======================================================================
        /// @brief Helper method to forward encoded messages received as signals
        /// @param[in] writer
        ///     DDS writer.
        /// @param[in] change
        ///     Mapping change, informing the receiver whether this is an
        ///     addition/update or removal.
        /// @param[in] update
        ///     The payload to publish

        template <class T>
        void publish_change(DataWriterRef<T> &writer,
                            cc::signal::MappingChange change,
                            const T &update)
        {
            switch (change)
            {
            case cc::signal::MAP_NONE:
                break;

            case cc::signal::MAP_ADDITION:
            case cc::signal::MAP_UPDATE:
                this->publish<T>(writer, update);
                break;

            case cc::signal::MAP_REMOVAL:
                logf_trace("Disposing: %s", writer->lookup_instance(update));
                writer->dispose_instance(writer->lookup_instance(update));
                break;
            }
        }
    };

}  // namespace cc::dds
