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
    constexpr auto LOG_STYLES_SETTING = "log styles";

    //==========================================================================
    // StreamSink methods

    StreamSink::StreamSink(std::ostream &stream)
        : MessageSink(),
          stream(stream)
    {
#ifndef _WIN32
        this->load_styles(core::settings->get(LOG_STYLES_SETTING).as_kvmap());
#endif
    }

    StreamSink::StreamSink(std::ostream &&stream)
        : StreamSink(stream)
    {
    }

    void StreamSink::capture_message(const Message::ptr &msg)
    {
        const StyleMap *styles = this->styles.get_ptr(msg->level());

        // auto lck = std::lock_guard(this->mtx);

        if (styles)
        {
            this->stream << styles->get(INTRO);
        }

        dt::tp_to_stream(this->stream, msg->timepoint(), true, 3, "%F|%T");

        this->stream << "|"
                     << std::setfill(' ') << std::setw(8)
                     << msg->level() << std::setw(0) << "|";

        this->send_preamble(this->stream, msg);

        if (styles)
        {
            this->stream << styles->get(RESET)
                         << styles->get(TEXT)
                         << msg->text()
                         << styles->get(RESET);
        }
        else
        {
            this->stream << msg->text();
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
            for (const auto &[level, levelname] : status::LevelNames)
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
