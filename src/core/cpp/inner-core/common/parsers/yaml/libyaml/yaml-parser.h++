/// -*- c++ -*-
//==============================================================================
/// @file yaml-parser.h++
/// @brief Wrapper for LibYaml `yaml_parser`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include "types/filesystem.h++"

#include <yaml.h>

#include <iostream>
#include <unordered_set>

namespace core::yaml
{
    class YamlParser
    {
        using This = YamlParser;
        using EventTypeSet = std::unordered_set<yaml_event_type_t>;

    public:
        YamlParser();
        ~YamlParser();

        types::Value parse_text(const std::string_view &text);
        types::Value parse_file(const fs::path &path);
        types::Value parse_stream(std::istream &stream);

    private:
        static int read_handler(void *data,
                                unsigned char *buffer,
                                size_t size,
                                size_t *size_read);

        yaml_event_t next_event();
        types::ValueList read_all();
        std::optional<types::Value> read_document();
        std::optional<std::string> read_key();
        std::optional<types::Value> read_value();

        bool expect_next_event_type(
            const EventTypeSet &expected_types,
            const EventTypeSet &end_types = {});

        bool expect_event_type(
            const yaml_event_t &event,
            const EventTypeSet &expected_types,
            const EventTypeSet &end_types = {});

        std::optional<types::Value> process_event(yaml_event_t event);
        types::Value process_alias(const yaml_event_t &event);
        types::Value process_scalar(const yaml_event_t &event);
        types::Value process_sequence(const yaml_event_t &event);
        types::Value process_mapping(const yaml_event_t &event);
        types::Value record_value(
            const yaml_char_t *anchor,
            const yaml_char_t *tag,
            const types::Value &value);
        std::size_t input_position() const;

    private:
        using AnchorName = std::string;
        std::unordered_map<AnchorName, types::Value> anchors;

        yaml_parser_t parser;
    };
} // namespace core::yaml
