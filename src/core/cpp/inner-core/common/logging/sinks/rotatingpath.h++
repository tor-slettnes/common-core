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
#include "thread/blockingqueue.h++"

#include <thread>
#include <memory>

namespace core::logging
{
    const std::string SETTING_NAME_TEMPLATE = "name template";
    const std::string SETTING_LOCAL_TIME = "local time";
    const std::string SETTING_COMPRESS_INACTIVE = "compress after use";
    const std::string SETTING_ROTATION = "rotate after";
    const std::string SETTING_EXPIRATION = "expire after";
    const std::string SETTING_LOG_FOLDER = "log folder";

    const std::string DEFAULT_NAME_TEMPLATE = "{executable}-{isodate}-{hour}{minute}{zoneoffset}";
    const bool DEFAULT_LOCAL_TIME = true;
    const bool DEFAULT_COMPRESS_INACTIVE = true;
    const dt::DateTimeInterval DEFAULT_ROTATION = {.unit = dt::TimeUnit::HOUR, .count = 6};
    const dt::DateTimeInterval DEFAULT_EXPIRATION = {.unit = dt::TimeUnit::YEAR, .count = 1};

    class RotatingPath
    {
        using This = RotatingPath;

    protected:
        RotatingPath(const std::string &sink_name,
                     const fs::path &default_suffix);

        virtual ~RotatingPath();

    protected:
        void load_rotation(const types::KeyValueMap &settings);
        std::unordered_map<std::string, std::string> expansions() const;

    public:
        std::string sink_name() const;
        fs::path default_suffix() const;

        std::string filename_template() const;
        void set_filename_template(const std::string &name_template);

        fs::path log_folder() const;
        void set_log_folder(const fs::path &path);

        fs::path current_path() const;
        fs::path current_suffix() const;

        bool use_local_time() const;
        void set_use_local_time(bool use_local_time);

        bool compress_inactive() const;
        void set_compress_inactive(bool use_local_time);

        dt::DateTimeInterval rotation_interval() const;
        void set_rotation_interval(const dt::DateTimeInterval &interval);

        dt::DateTimeInterval expiration_interval() const;
        void set_expiration_interval(const dt::DateTimeInterval &interval);

    protected:
        virtual void open_file(const dt::TimePoint &tp);
        virtual void close_file() {}

        virtual void rotate(const dt::TimePoint &tp);
        void check_rotation(const dt::TimePoint &tp);
        dt::TimePoint current_rotation() const;
        void update_current_path(const dt::TimePoint &tp,
                                 bool create_directory = true);
        fs::path construct_path(const dt::TimePoint &tp) const;

        void check_expiration(const dt::TimePoint &tp);
        void check_expiration(const dt::TimePoint &expiration_time,
                              const fs::path &path);

        void compress_all_inactive();
        void compress(const fs::path &logfile);

    public:
        static fs::path default_root_folder;

    private:
        std::string sink_name_;
        fs::path default_suffix_;
        std::string name_template_;
        fs::path log_folder_;
        bool use_local_time_;
        bool compress_inactive_;
        dt::DateTimeInterval rotation_interval_;
        dt::DateTimeInterval expiration_interval_;
        std::unordered_map<std::string, std::string> expansions_;
        dt::TimePoint current_rotation_;
        fs::path current_path_;
    };
}  // namespace core::logging
