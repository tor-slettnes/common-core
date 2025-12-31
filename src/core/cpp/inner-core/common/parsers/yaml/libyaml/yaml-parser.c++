/// -*- c++ -*-
//==============================================================================
/// @file yaml-parser.c++
/// @brief Wrapper for LibYaml `yaml_parser`
/// @author Tor Slettnes
//==============================================================================

#include "yaml-parser.h++"
#include "platform/process.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include "io/cutils.h++"

#include <stdio.h>
#include <cerrno>
#include <system_error>

namespace core::yaml
{
    YamlParser::YamlParser()
    {
        yaml_parser_initialize(&this->parser);
    }

    YamlParser::~YamlParser()
    {
        yaml_parser_delete(&this->parser);
    }

    types::Value YamlParser::parse_text(const std::string_view &text)
    {
        yaml_parser_set_input_string(
            &this->parser,
            reinterpret_cast<const unsigned char *>(text.data()),
            text.size());
        return this->read_all().front();
    }

    types::Value YamlParser::parse_file(const fs::path &path)
    {
        if (FILE *fp = io::checkstatus(::fopen(path.c_str(), "r")))
        {
            yaml_parser_set_input_file(&this->parser, fp);
            types::Value result = this->read_all().front();
            ::fclose(fp);
            return result;
        }
        else
        {
            throw exception::FilesystemError(errno, path.string());
        }
    }

    types::Value YamlParser::parse_stream(std::istream &stream)
    {
        yaml_parser_set_input(&this->parser, This::read_handler, &stream);
        return this->read_all().front();
    }

    int YamlParser::read_handler(void *data,
                                 unsigned char *buffer,
                                 size_t size,
                                 size_t *size_read)
    {
        std::istream *stream = reinterpret_cast<std::istream *>(data);
        assertf(stream, "YAML read handler received nullptr where input stream expected");
        stream->read(reinterpret_cast<char *>(buffer), size);
        *size_read = stream->gcount();
        return !stream->fail();
    }

    yaml_event_t YamlParser::next_event()
    {
        yaml_event_t event;
        if (yaml_parser_parse(&this->parser, &event))
        {
            return event;
        }
        else
        {
            throw exception::FailedPostcondition(
                this->parser.problem,
                {
                    {"offset", this->parser.problem_offset},
                });
        }
    }

    types::ValueList YamlParser::read_all()
    {
        types::ValueList docs;
        if (this->expect_next_event_type(
                {YAML_STREAM_START_EVENT},
                {YAML_STREAM_END_EVENT}))
        {
            while (auto doc = this->read_document())
            {
                docs.push_back(doc);
            }
        }
        return docs;
    }

    std::optional<types::Value> YamlParser::read_document()
    {
        if (this->expect_next_event_type(
                {YAML_DOCUMENT_START_EVENT},
                {YAML_DOCUMENT_END_EVENT, YAML_STREAM_END_EVENT}))
        {
            return this->read_value();
        }
        else
        {
            return {};
        }
    }

    std::optional<std::string> YamlParser::read_key()
    {
        std::optional<std::string> key;
        yaml_event_t event = this->next_event();
        if (this->expect_event_type(
                event,
                {YAML_SCALAR_EVENT},
                {YAML_MAPPING_END_EVENT}))
        {
            key = {
                reinterpret_cast<const char *>(event.data.scalar.value),
                event.data.scalar.length,
            };
        }
        yaml_event_delete(&event);
        return key;
    }

    std::optional<types::Value> YamlParser::read_value()
    {
        yaml_event_t event = this->next_event();
        auto opt_value = this->process_event(event);
        yaml_event_delete(&event);
        return opt_value;
    }

    bool YamlParser::expect_next_event_type(
        const EventTypeSet &expected_types,
        const EventTypeSet &end_types)
    {
        yaml_event_t event = this->next_event();
        bool expected = this->expect_event_type(event, expected_types, end_types);
        yaml_event_delete(&event);
        return expected;
    }

    bool YamlParser::expect_event_type(
        const yaml_event_t &event,
        const EventTypeSet &expected_types,
        const EventTypeSet &end_types)
    {
        yaml_event_type_t event_type = event.type;

        if (expected_types.count(event_type))
        {
            return true;
        }
        else if (end_types.empty() || end_types.count(event_type))
        {
            return false;
        }
        else
        {
            throwf(exception::RuntimeError,
                   "YAML parser got unexpected event type %d at position %d (expected one of: %d)",
                   event_type,
                   this->input_position(),
                   expected_types);
        }
    }

    std::optional<types::Value> YamlParser::process_event(yaml_event_t event)
    {
        switch (event.type)
        {
        case YAML_ALIAS_EVENT:
            return this->process_alias(event);

        case YAML_SCALAR_EVENT:
            return this->process_scalar(event);

        case YAML_SEQUENCE_START_EVENT:
            return this->process_sequence(event);

        case YAML_MAPPING_START_EVENT:
            return this->process_mapping(event);

        default:
            return {};
        }
    }

    types::Value YamlParser::process_alias(const yaml_event_t &event)
    {
        try
        {
            return this->anchors.at(reinterpret_cast<char *>(event.data.alias.anchor));
        }
        catch (const std::out_of_range &)
        {
            logf_warning(
                "YAML input contains reference to non-existing anchor %r at position %d",
                event.data.alias.anchor,
                this->input_position());
            return {};
        }
    }

    types::Value YamlParser::process_scalar(const yaml_event_t &event)
    {
        std::string stringvalue(
            reinterpret_cast<const char *>(event.data.scalar.value),
            event.data.scalar.length);

        types::Value value = (event.data.scalar.style == YAML_PLAIN_SCALAR_STYLE)
                                 ? types::Value::from_literal(stringvalue)
                                 : types::Value(stringvalue);

        return this->record_value(
            event.data.scalar.anchor,
            event.data.scalar.tag,
            value);
    }

    types::Value YamlParser::process_sequence(const yaml_event_t &event)
    {
        auto list = std::make_shared<types::ValueList>();
        while (auto value = this->read_value())
        {
            list->push_back(*value);
        }
        return this->record_value(
            event.data.sequence_start.anchor,
            event.data.sequence_start.tag,
            list);
    }

    types::Value YamlParser::process_mapping(const yaml_event_t &event)
    {
        auto map = std::make_shared<types::KeyValueMap>();
        while (auto key = this->read_key())
        {
            if (auto value = this->read_value())
            {
                map->insert_or_assign(*key, *value);
            }
            else
            {
                logf_warning("Skipping YAML mapping key at position %d with no value: %r",
                             this->input_position(),
                             *key);
            }
        }
        return this->record_value(
            event.data.mapping_start.anchor,
            event.data.mapping_start.tag,
            map);
    }

    types::Value YamlParser::record_value(
        const yaml_char_t *anchor,
        const yaml_char_t *tag,
        const types::Value &value)
    {
        if (anchor)
        {
            this->anchors.insert_or_assign(
                reinterpret_cast<const char *>(anchor),
                value);
        }

        return value;
    }

    std::size_t YamlParser::input_position() const
    {
        return std::distance(this->parser.buffer.start,
                             this->parser.buffer.pointer);
    }

}  // namespace core::yaml
