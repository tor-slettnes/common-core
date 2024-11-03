/// -*- c++ -*-
//==============================================================================
/// @file rotatingpath.c++
/// @brief Swap log files based on name template
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rotatingpath.h++"
#include "string/expand.h++"
#include "chrono/date-time.h++"
#include "platform/path.h++"

namespace core::logging
{
    RotatingPath::RotatingPath(const std::string &sink_name,
                               const std::string &path_template,
                               const std::string &default_suffix,
                               const dt::DateTimeInterval &rotation_interval,
                               bool use_local_time)
        : sink_name_(sink_name),
          path_template_(path_template),
          default_suffix_(default_suffix),
          rotation_interval_(rotation_interval),
          use_local_time_(use_local_time),
          log_folder_(platform::path->log_folder()),
          exec_name_(platform::path->exec_name())
    {
    }

    RotatingPath::~RotatingPath()
    {
        this->close_file();
    }


    void RotatingPath::open_file(const dt::TimePoint &tp)
    {
        this->update_current_path(tp);
    }

    void RotatingPath::rotate(const dt::TimePoint &tp)
    {
        this->close_file();
        this->open_file(tp);
    }

    void RotatingPath::check_rotation(const dt::TimePoint &tp)
    {
        dt::TimePoint aligned = dt::last_aligned(
            tp,
            this->rotation_interval(),
            this->use_local_time());

        if (aligned > this->current_rotation())
        {
            this->rotate(aligned);
        }
    }

    void RotatingPath::update_current_path(const dt::TimePoint &starttime,
                                           bool create_directory)
    {
        this->current_path_ = this->construct_path(starttime);
        this->current_rotation_ = starttime;

        if (create_directory)
        {
            fs::create_directories(this->current_path_.parent_path());
        }
    }

    fs::path RotatingPath::construct_path(const dt::TimePoint &starttime) const
    {
        static const std::unordered_map<std::string, std::string> expansions = {
            {"executable", platform::path->exec_name()},
            {"sink", this->sink_name()},
            {"isodate", "%F"},
            {"isotime", "%T"},
            {"year", "%Y"},
            {"month", "%m"},
            {"week", "%U"},
            {"day", "%d"},
            {"hour", "%H"},
            {"minute", "%M"},
            {"second", "%S"},
            {"zonename", "%Z"},
            {"zoneoffset", "%z"},
        };

        std::string log_name = dt::to_string(
            starttime,                                        // tp
            0,                                                // decimals
            str::expand(this->path_template(), expansions));  // format

        fs::path log_file = platform::path->extended_filename(
            log_name,
            this->default_suffix_,
            true);

        return fs::weakly_canonical(this->log_folder_ / log_file);
    }

    std::string RotatingPath::sink_name() const
    {
        return this->sink_name_;
    }

    std::string RotatingPath::path_template() const
    {
        return this->path_template_;
    }

    dt::DateTimeInterval RotatingPath::rotation_interval() const
    {
        return this->rotation_interval_;
    }

    dt::TimePoint RotatingPath::current_rotation() const
    {
        return this->current_rotation_;
    }

    bool RotatingPath::use_local_time() const
    {
        return this->use_local_time_;
    }

    fs::path RotatingPath::current_path() const
    {
        return this->current_path_;
    }
}  // namespace core::logging
