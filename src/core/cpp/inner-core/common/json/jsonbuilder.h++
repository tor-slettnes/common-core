/// -*- c++ -*-
//==============================================================================
/// @file jsonbuilder.h++
/// @brief Convert a Value instance to JSON
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "string/misc.h++"
#include "status/exceptions.h++"

namespace core::json
{
    class JsonBuilder
    {
        using This = JsonBuilder;

    public:
        static std::string to_string(const types::Value &value,
                                     bool pretty = false);

        static std::ostream &to_stream(std::ostream &&stream,
                                       const types::Value &value,
                                       bool pretty = false,
                                       const std::string &indent = "");

        static std::ostream &to_stream(std::ostream &stream,
                                       const types::Value &value,
                                       bool pretty = false,
                                       const std::string &indent = "");

    public:
        static std::ostream &to_stream(std::ostream &stream,
                                       const types::KeyValueMap &kvmap,
                                       bool pretty,
                                       const std::string &indent);

        static std::ostream &to_stream(std::ostream &stream,
                                       const types::ValueList &list,
                                       bool pretty,
                                       const std::string &indent);

        static std::ostream &to_stream(std::ostream &stream,
                                       const types::TaggedValueList &tvlist,
                                       bool pretty,
                                       const std::string &indent);
    };

}  // namespace core::json
