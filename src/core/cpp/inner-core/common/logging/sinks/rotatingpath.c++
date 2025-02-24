/// -*- c++ -*-
//==============================================================================
/// @file rotatingpath.c++
/// @brief Swap log files based on name template
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rotatingpath.h++"
#include "string/expand.h++"
#include "chrono/date-time.h++"
#include "status/exceptions.h++"
#include "platform/path.h++"
#include "platform/host.h++"
#include "platform/init.h++"
#include "io/gzip/writer.h++"
#include "logging/logging.h++"

#include <fstream>

namespace core::logging
{
    constexpr auto COMPRESSION_SUFFIX = ".gz";

    RotatingPath::RotatingPath(const std::string &sink_name,
                               const fs::path &default_suffix)
        : sink_name_(sink_name),
          default_suffix_(default_suffix),
          name_template_(DEFAULT_NAME_TEMPLATE),
          log_folder_(This::default_root_folder),
          use_local_time_(DEFAULT_LOCAL_TIME),
          compress_inactive_(DEFAULT_COMPRESS_INACTIVE),
          rotation_interval_(DEFAULT_ROTATION),
          expiration_interval_(DEFAULT_EXPIRATION),
          expansions_({
              {"executable", platform::path->exec_name()},
              {"hostname", platform::host->get_host_name()},
              {"sink_id", this->sink_name()},
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
          })
    {
    }

    RotatingPath::~RotatingPath()
    {
        this->close_file();
    }

    void RotatingPath::load_rotation(const types::KeyValueMap &settings)
    {
        if (const types::Value &log_folder = settings.get(SETTING_LOG_FOLDER))
        {
            this->set_log_folder(log_folder.as_string());
        }

        if (const types::Value &name_template = settings.get(SETTING_NAME_TEMPLATE))
        {
            this->set_filename_template(name_template.as_string());
        }

        if (const types::Value &use_local_time = settings.get(SETTING_LOCAL_TIME))
        {
            this->set_use_local_time(use_local_time.as_bool());
        }

        if (const types::Value &compress_inactive = settings.get(SETTING_COMPRESS_INACTIVE))
        {
            this->set_compress_inactive(compress_inactive.as_bool());
        }

        if (auto rotation = settings.try_get_as<dt::DateTimeInterval>(SETTING_ROTATION))
        {
            this->set_rotation_interval(rotation.value());
        }

        if (auto expiration = settings.try_get_as<dt::DateTimeInterval>(SETTING_EXPIRATION))
        {
            this->set_expiration_interval(expiration.value());
        }
    }

    std::unordered_map<std::string, std::string> RotatingPath::expansions() const
    {
        return this->expansions_;
    }

    std::string RotatingPath::sink_name() const
    {
        return this->sink_name_;
    }

    fs::path RotatingPath::default_suffix() const
    {
        return this->default_suffix_;
    }

    std::string RotatingPath::filename_template() const
    {
        return this->name_template_;
    }

    void RotatingPath::set_filename_template(const std::string &name_template)
    {
        this->name_template_ = name_template;
    }

    fs::path RotatingPath::current_path() const
    {
        return this->current_path_;
    }

    fs::path RotatingPath::current_suffix() const
    {
        return this->current_path().extension();
    }

    fs::path RotatingPath::log_folder() const
    {
        return this->log_folder_;
    }

    void RotatingPath::set_log_folder(const fs::path &path)
    {
        this->log_folder_ = path;
    }

    bool RotatingPath::use_local_time() const
    {
        return this->use_local_time_;
    }

    void RotatingPath::set_use_local_time(bool use_local_time)
    {
        this->use_local_time_ = use_local_time;
    }

    bool RotatingPath::compress_inactive() const
    {
        return this->compress_inactive_;
    }

    void RotatingPath::set_compress_inactive(bool compress_inactive)
    {
        this->compress_inactive_ = compress_inactive;
    }

    dt::DateTimeInterval RotatingPath::rotation_interval() const
    {
        return this->rotation_interval_;
    }

    void RotatingPath::set_rotation_interval(const dt::DateTimeInterval &interval)
    {
        this->rotation_interval_ = interval;
    }

    dt::DateTimeInterval RotatingPath::expiration_interval() const
    {
        return this->expiration_interval_;
    }

    void RotatingPath::set_expiration_interval(const dt::DateTimeInterval &interval)
    {
        this->expiration_interval_ = interval;
    }

    void RotatingPath::open_file(const dt::TimePoint &tp)
    {
        this->update_current_path(tp);
        auto future = std::async([=] {
            this->check_expiration(tp);
            this->compress_all_inactive();
        });
    }

    void RotatingPath::rotate(const dt::TimePoint &tp)
    {
        this->close_file();
        this->open_file(tp);
    }

    void RotatingPath::check_rotation(const dt::TimePoint &tp)
    {
        if (this->rotation_interval())
        {
            core::dt::TimePoint last_aligned = dt::last_aligned(
                tp,
                this->rotation_interval(),
                this->use_local_time());

            if (last_aligned > this->current_rotation())
            {
                this->rotate(last_aligned);
            }
        }
    }

    dt::TimePoint RotatingPath::current_rotation() const
    {
        return this->current_rotation_;
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
        std::string log_name = dt::to_string(
            starttime,                                                    // tp
            0,                                                            // decimals
            str::expand(this->filename_template(), this->expansions()));  // format

        fs::path log_file = platform::path->extended_filename(
            log_name,
            this->default_suffix(),
            true);

        return fs::weakly_canonical(this->log_folder() / log_file);
    }

    void RotatingPath::check_expiration(const dt::TimePoint &tp)
    {
        if (this->expiration_interval())
        {
            core::dt::TimePoint expiration_time = dt::last_aligned(
                tp,
                this->expiration_interval(),
                this->use_local_time());

            fs::path pattern = fs::path("*") += this->current_suffix();

            try
            {
                for (const fs::path &candidate_path : platform::path->locate(
                         {pattern},
                         this->log_folder()))
                {
                    this->check_expiration(expiration_time, candidate_path);
                }
            }
            catch (const fs::filesystem_error &e)
            {
                logf_warning("Failed to expire old logs: ", e);
            }
        }
    }

    void RotatingPath::check_expiration(const dt::TimePoint &expiration_time,
                                        const fs::path &path)
    {
        if (auto stats = platform::path->try_get_stats(path))
        {
            if (stats->modify_time < expiration_time)
            {
                logf_info("Expiring log file %s", path);
                fs::remove(path);
            }
        }
    }

    void RotatingPath::compress_all_inactive()
    {
        if (this->compress_inactive())
        {
            fs::path pattern = fs::path("*") += this->current_suffix();

            for (const fs::path &candidate_path : platform::path->locate(
                     {pattern},
                     this->log_folder()))
            {
                if (candidate_path != this->current_path())
                {
                    logf_debug("Compressing log file: %s", candidate_path);
                    this->compress(candidate_path);
                }
            }
        }
    }

    void RotatingPath::compress(const fs::path &logfile)
    {
        if ((logfile.extension() != COMPRESSION_SUFFIX) && fs::exists(logfile))
        {
            try
            {
                std::ifstream is(logfile);

                fs::path outfile = logfile;
                outfile += COMPRESSION_SUFFIX;
                core::io::GZipOutputStream os(outfile);
                os << is.rdbuf();
                fs::remove(logfile);
            }
            catch (...)
            {
                logf_warning("Unable to compress log file %r: %s",
                             logfile,
                             std::current_exception());
            }
        }
    }

    fs::path RotatingPath::default_root_folder;

}  // namespace core::logging
