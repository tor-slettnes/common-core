/// -*- c++ -*-
//==============================================================================
/// @file streamsink.c++
/// @brief Log sink for stream-based backend, (stdout/stderr, file, ...)
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "streamsink.h++"
#include "chrono/date-time.h++"
#include "types/value.h++"
#include "string/misc.h++"
#include "settings/settings.h++"

#include <iomanip>
#include <ostream>
#include <string>

namespace core::logging
{
    constexpr auto SETTING_LOG_STYLES = "log styles";

    //==========================================================================
    // StreamSink methods

    StreamSink::StreamSink(const std::string &sink_id,
                           std::ostream &stream)
        : LogSink(sink_id, {}, MESSAGE_CONTRACT),
          stream(stream)
    {
    }

    StreamSink::StreamSink(const std::string &sink_id,
                           std::ostream &&stream)
        : StreamSink(sink_id, stream)
    {
    }

    void StreamSink::load_settings(const types::KeyValueMap &settings)
    {
        Super::load_settings(settings);
        this->load_message_format(settings);

#ifndef _WIN32
        this->load_styles(core::settings->get(SETTING_LOG_STYLES).as_kvmap());
#endif
    }

    bool StreamSink::is_applicable(const types::Loggable &item) const
    {
        return this->is_valid_message(item) && LogSink::is_applicable(item);
    }

    void StreamSink::capture_event(const status::Event::ptr &event)
    {
        const StyleMap *styles = this->styles.get_ptr(event->level());

        // auto lck = std::lock_guard(this->mtx);

        if (styles)
        {
            this->stream << styles->get(INTRO);
        }

        this->send_preamble(this->stream, event);

        if (styles)
        {
            this->stream << styles->get(RESET)
                         << styles->get(TEXT)
                         << event->text()
                         << styles->get(RESET);
        }
        else
        {
            this->stream << event->text();
        }

        this->stream << std::endl;
    }

    void StreamSink::load_styles(const types::KeyValueMap &stylemap)
    {
        this->load_styles(stylemap, INTRO, "intro");
        this->load_styles(stylemap, TEXT, "text");
        this->load_styles(stylemap, RESET, "reset");
    }

    void StreamSink::load_styles(const types::KeyValueMap &stylemap,
                                 MessagePart part,
                                 const std::string &partname)
    {
        if (const types::Value &styles = stylemap.get(partname))
        {
            for (const auto &[level, levelname] : status::level_names)
            {
                std::string style =
                    styles.is_kvmap() ? styles.as_kvmap().get(str::tolower(levelname)).as_string()
                                      : styles.as_string();

                if (!style.empty())
                {
                    this->styles[level][part] = str::unescaped(style);
                }
            }
        }
    }
}  // namespace core::logging
