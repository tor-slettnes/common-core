/// -*- c++ -*-
//==============================================================================
/// @file rotatingpath.h++
/// @brief Swap log files based on name template
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "chrono/date-time.h++"
#include "types/filesystem.h++"

namespace core::logging
{
    class RotatingPath
    {
        using This = RotatingPath;

    protected:
        RotatingPath(const std::string &sink_name,
                     const std::string &path_template,
                     const std::string &default_suffix,
                     const dt::DateTimeInterval &rotation_interval,
                     bool local_time = true);

        virtual ~RotatingPath();

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
        std::string path_template() const;
        dt::DateTimeInterval rotation_interval() const;
        dt::TimePoint current_rotation() const;
        bool use_local_time() const;

        fs::path current_path() const;

    private:
        std::string sink_name_;
        std::string path_template_;
        std::string default_suffix_;
        dt::DateTimeInterval rotation_interval_;
        bool use_local_time_;
        fs::path log_folder_;
        std::string exec_name_;
        dt::TimePoint current_rotation_;
        fs::path current_path_;
    };
}  // namespace core::logging
