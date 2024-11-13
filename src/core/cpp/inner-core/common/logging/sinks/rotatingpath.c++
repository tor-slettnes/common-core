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
                               const std::string &default_suffix)
        : sink_name_(sink_name),
          default_suffix_(default_suffix),
          name_template_(DEFAULT_NAME_TEMPLATE),
          rotation_interval_(DEFAULT_ROTATION),
          use_local_time_(DEFAULT_LOCAL_TIME),
          log_folder_(platform::path->log_folder()),
          exec_name_(platform::path->exec_name())
    {
    }

    RotatingPath::~RotatingPath()
    {
        this->close_file();
    }

    void RotatingPath::load_rotation(const types::KeyValueMap &settings)
    {
        if (const types::Value &value = settings.get(SETTING_NAME_TEMPLATE))
        {
            this->set_filename_template(value.as_string());
        }

        if (auto interval = settings.get_as<dt::DateTimeInterval>(SETTING_ROTATION))
        {
            this->set_rotation_interval(interval.value());
        }

        if (const types::Value &value = settings.get(SETTING_LOCAL_TIME))
        {
            this->set_use_local_time(value.as_bool());
        }
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
        core::dt::TimePoint aligned = this->last_aligned(tp);
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
            starttime,                                            // tp
            0,                                                    // decimals
            str::expand(this->filename_template(), expansions));  // format

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

    std::string RotatingPath::filename_template() const
    {
        return this->name_template_;
    }

    void RotatingPath::set_filename_template(const std::string &name_template)
    {
        this->name_template_ = name_template;
    }

    bool RotatingPath::use_local_time() const
    {
        return this->use_local_time_;
    }

    void RotatingPath::set_use_local_time(bool use_local_time)
    {
        this->use_local_time_ = use_local_time;
    }

    dt::DateTimeInterval RotatingPath::rotation_interval() const
    {
        return this->rotation_interval_;
    }

    void RotatingPath::set_rotation_interval(const dt::DateTimeInterval &interval)
    {
        this->rotation_interval_ = interval;
    }

    dt::TimePoint RotatingPath::current_rotation() const
    {
        return this->current_rotation_;
    }

    dt::TimePoint RotatingPath::last_aligned(const dt::TimePoint &tp) const
    {
        return dt::last_aligned(
            tp,
            this->rotation_interval(),
            this->use_local_time());
    }

    fs::path RotatingPath::current_path() const
    {
        return this->current_path_;
    }
}  // namespace core::logging
