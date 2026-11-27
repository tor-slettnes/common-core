// -*- c++ -*-
//==============================================================================
/// @file udev-monitor.c++
/// @brief Monitor and emit UDev events
/// @author Tor Slettnes
//==============================================================================

#include "udev-monitor.h++"
#include "udev-signals.h++"
#include "io/cutils.h++"

#include <poll.h>

namespace cc::core::udev
{
    Monitor::Monitor(const std::string &device_type)
        : Enumerator(device_type),
          monitor(udev_monitor_new_from_netlink(this->udev, "udev")),
          fd(-1),
          running(false)
    {
    }

    Monitor::~Monitor()
    {
        this->stop();
        udev_monitor_unref(this->monitor);
    }

    void Monitor::start()
    {
        if (!this->running)
        {
            this->stop();
            this->init_monitor();
            this->enumerate_and_emit();

            this->running = true;
            this->worker_thread = std::thread(&This::worker, this);
        }
    }

    void Monitor::stop()
    {
        this->running = false;
        if (this->worker_thread.joinable())
        {
            this->worker_thread.join();
        }
    }

    void Monitor::init_monitor()
    {
        udev_monitor_filter_add_match_subsystem_devtype(
            this->monitor,
            this->device_type.c_str(),
            NULL);

        udev_monitor_enable_receiving(this->monitor);
        this->fd = udev_monitor_get_fd(this->monitor);
    }

    void Monitor::enumerate_and_emit()
    {
        for (const auto &device_info : this->enumerate())
        {
            this->emit_device_info(device_info);
        }
    }

    void Monitor::emit_device_info(
        const std::shared_ptr<DeviceInfo> &device_info)
    {
        signal_device.emit(device_info->devpath(), device_info);
    }

    void Monitor::worker()
    {
        struct pollfd pfd;
        pfd.events = POLLIN;
        pfd.revents = 0;
        pfd.fd = this->fd;

        while (this->running)
        {
            core::io::checkstatus(
                ::poll(&pfd, 1, 1000),
                "Polling for UDev events");

            if (pfd.revents & POLLIN)
            {
                if (struct udev_device *device = udev_monitor_receive_device(
                        this->monitor))
                {
                    this->emit_device_info(DeviceInfo::create_shared(device));
                }
            }
            else if (pfd.revents & POLLERR)
            {
                throw std::system_error(
                    errno,
                    std::system_category(),
                    "UDev poll");
            }
        }
    }

}  // namespace cc::core::udev
