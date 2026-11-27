// -*- c++ -*-
//==============================================================================
/// @file udev-monitor.h++
/// @brief Monitor and emit UDev events
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "udev-enumerator.h++"

#include <thread>

namespace cc::core::udev
{
    class Monitor : public Enumerator
    {
        using This = Monitor;
        using Super = Enumerator;

    public:
        Monitor(const std::string &device_type);
        ~Monitor();

        void start();
        void stop();

    private:
        void init_monitor();
        void enumerate_and_emit();
        void emit_device_info(const std::shared_ptr<DeviceInfo> &device_info);
        void worker();

    protected:
        struct udev_monitor *monitor;
        int fd;
        bool running;
        std::thread worker_thread;
    };
}  // namespace cc::core::udev
