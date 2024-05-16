/// -*- c++ -*-
//==============================================================================
/// @file rotatingpath.c++
/// @brief Swap log files based on name template
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rotatingpath.h++"
#include "string/expand.h++"
#include "platform/path.h++"

namespace cc::logging
{
    RotatingPath::RotatingPath(const std::string &path_template,
                               const std::string &default_suffix,
                               const dt::Duration &rotation_interval)
        : path_template_(path_template),
          default_suffix_(default_suffix),
          rotation_interval_(rotation_interval),
          log_folder_(platform::path->log_folder()),
          exec_name_(platform::path->exec_name())
    {
    }

    std::string RotatingPath::path_template() const
    {
        return this->path_template_;
    }

    void RotatingPath::check_rotation(const dt::TimePoint &tp)
    {
        dt::TimePoint aligned = dt::last_aligned(tp, this->rotation_interval_);
        if (aligned > this->current_rotation())
        {
            this->rotate(aligned);
        }
    }

    void RotatingPath::rotate(const dt::TimePoint &starttime)
    {
        this->update_current_path(starttime);
    }

    dt::TimePoint RotatingPath::current_rotation() const
    {
        return this->current_rotation_;
    }

    fs::path RotatingPath::current_path() const
    {
        return this->current_path_;
    }

    fs::path RotatingPath::construct_path(const dt::TimePoint &starttime) const
    {
        static const std::unordered_map<std::string, std::string> expansions = {
            {"execname", platform::path->exec_name()},
            {"isodate", "%F"},
            {"isotime", "%T"},
            {"year", "%Y"},
            {"month", "%m"},
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

    void RotatingPath::update_current_path(const dt::TimePoint &starttime)
    {
        this->current_path_ = this->construct_path(starttime);
        this->current_rotation_ = starttime;
    }

}  // namespace cc::logging
