/// -*- c++ -*-
//==============================================================================
/// @file rotatingpath.h++
/// @brief Swap log files based on name template
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "chrono/date-time.h++"
#include "types/filesystem.h++"
#include "types/value.h++"

namespace core::logging
{
    const std::string SETTING_NAME_TEMPLATE = "name template";
    const std::string SETTING_ROTATION = "rotation";
    const std::string SETTING_LOCAL_TIME = "local time";

    const std::string DEFAULT_NAME_TEMPLATE = "{executable}-{isodate}-{hour}{minute}{zoneoffset}";
    const bool DEFAULT_LOCAL_TIME = true;
    const dt::DateTimeInterval DEFAULT_ROTATION = {.unit = dt::TimeUnit::HOUR, .count = 6};

    class RotatingPath
    {
        using This = RotatingPath;

    protected:
        RotatingPath(const std::string &sink_name,
                     const std::string &default_suffix);

        virtual ~RotatingPath();

    protected:
        void load_rotation(const types::KeyValueMap &settings);

    protected:
        virtual void open_file(const dt::TimePoint &tp);
        virtual void close_file() {}
        virtual void rotate(const dt::TimePoint &tp);
        void check_rotation(const dt::TimePoint &tp);
        void update_current_path(const dt::TimePoint &tp,
                                 bool create_directory = true);
        fs::path construct_path(const dt::TimePoint &tp) const;

    public:
        std::string sink_name() const;

        std::string filename_template() const;
        void set_filename_template(const std::string &name_template);

        bool use_local_time() const;
        void set_use_local_time(bool use_local_time);

        dt::DateTimeInterval rotation_interval() const;
        void set_rotation_interval(const dt::DateTimeInterval &interval);

        dt::TimePoint current_rotation() const;
        dt::TimePoint last_aligned(const dt::TimePoint &tp) const;
        fs::path current_path() const;

    private:
        std::string sink_name_;
        std::string default_suffix_;
        std::string name_template_;
        dt::DateTimeInterval rotation_interval_;
        bool use_local_time_;
        fs::path log_folder_;
        std::string exec_name_;
        dt::TimePoint current_rotation_;
        fs::path current_path_;
    };
}  // namespace core::logging
