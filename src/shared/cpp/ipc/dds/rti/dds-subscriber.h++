/// -*- c++ -*-
//==============================================================================
/// @file dds-subscriber.h++
/// @brief A subscribing entity/consumer in a DDS environment
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
// #include "dds-topic.h++"
#include "dds-endpoint.h++"
#include "thread/signaltemplate.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>

#include <thread>

namespace cc::dds
{
    //==========================================================================
    // @class PubSubChannnel

    class Subscriber : public Endpoint,
                       public ::dds::sub::Subscriber
    {
        using This = Subscriber;
        using Super = Endpoint;

    protected:
        using DataReaderQos = ::dds::sub::qos::DataReaderQos;

        template <class T>
        using Handler = std::function<void(cc::signal::MappingChange, const T &)>;

        template <class T>
        using DataReaderRef = std::shared_ptr<::dds::sub::DataReader<T>>;

    public:
        Subscriber(const std::string &channel_name, int domain_id);
        ~Subscriber();

        void initialize() override;
        void deinitialize() override;

        void start_listening();
        void stop_listening();

        void listen();

    public:
        //======================================================================
        /// @brief Create a new DDS reader instance on this subscriber channel
        /// @param[in] topic_name
        ///     DDS publication topic.  By default we use the fully qualified
        ///     name of the data type.
        /// @param[in] reliable
        ///     Whether to assign the RELIABLE or BEST_EFFORT QoS to this topic
        /// @param[in] sync_latest
        ///     Whether to assign the TRANSIENT_LOCAL or VOLATILE QoS to this topic

        template <class T>
        inline DataReaderRef<T>
        create_reader(const std::string &topic_name,
                      bool reliable = false,
                      bool sync_latest = false)
        {
            ::dds::topic::Topic<T> topic(this->get_participant(), topic_name);

            auto qos = this->pubsub_policy<::dds::sub::qos::DataReaderQos>(
                reliable,
                sync_latest);

            return std::make_shared<::dds::sub::DataReader<T>>(*this, topic, qos);
        }

        /// @brief Create a new DDS reader instance with a callback handler
        /// @param[in] topic_name
        ///     DDS publication topic.  By default we use the fully qualified
        ///     name of the data type.
        /// @param[in] handler
        ///     Handler to be invoked when a publication is received
        /// @param[in] reliable
        ///     Whether to assign the RELIABLE or BEST_EFFORT QoS to this topic
        /// @param[in] sync_latest
        ///     Whether to assign the TRANSIENT_LOCAL or VOLATILE QoS to this topic

        template <class T>
        inline DataReaderRef<T>
        create_reader(const std::string &topic_name,
                      const Handler<T> &handler,
                      bool reliable = true,
                      bool sync_latest = false)
        {
            auto reader = this->create_reader<T>(topic_name, reliable, sync_latest);

            // ::dds::core::cond::StatusCondition status_condition(*reader);
            // status_condition.enabled_statuses(
            //     ::dds::core::status::StatusMask::data_available());
            // status_condition.extensions().handler(
            //     std::bind(This::read_samples<T>, reader, handler));

            // this->waitset += status_condition;

            this->waitset += ::dds::sub::cond::ReadCondition(
                *reader,
                ::dds::sub::status::DataState::any(),
                std::bind(This::read_samples<T>, reader, handler));

            return reader;
        }

        /// @brief Create a new DDS reader that emits a signal
        /// @param[in] signal
        ///     Signal to be emitted when a publication is received
        /// @param[in] topic_name
        ///     DDS publication topic.  By default we use the fully qualified
        ///     name of the data type.
        /// @param[in] reliable
        ///     Whether to assign the RELIABLE or BEST_EFFORT QoS to this topic
        /// @param[in] sync_latest
        ///     Whether to assign the TRANSIENT_LOCAL or VOLATILE QoS to this topic

        template <class T>
        inline DataReaderRef<T>
        create_signal_reader(cc::signal::Signal<T> *signal,
                             const std::string &topic_name = TYPE_NAME_BASE(T),
                             bool reliable = true,
                             bool sync_latest = false)
        {
            return this->create_reader<T>(
                topic_name,
                std::bind(&cc::signal::Signal<T>::emit, signal, std::placeholders::_2),
                reliable,
                sync_latest);
        }

    private:
        template <class T>
        static void
        read_samples(const DataReaderRef<T> &reader,
                     const Handler<T> &handler)
        {
            ::dds::sub::LoanedSamples<T> samples = reader->take();
            logf_trace("Took %d samples", std::distance(samples.begin(), samples.end()));
            for (const auto &sample : samples)
            {
                if (sample.info().valid())
                {
                    handler(cc::signal::MAP_UPDATE, sample.data());
                }
                else if (sample.info().state().instance_state() ==
                         ::dds::sub::status::InstanceState::not_alive_disposed())
                {
                    T key_holder;
                    reader->key_value(key_holder, sample.info().instance_handle());
                    handler(cc::signal::MAP_REMOVAL, std::move(key_holder));
                }
                else
                {
                    logf_notice("Received invalid sample: %s", sample);
                }
            }
        }

    protected:
        // ::dds::sub::Subscriber subscriber;
        ::dds::core::cond::WaitSet waitset;
        std::thread listen_thread;
        bool keep_listening;
    };
}  // namespace cc::dds
