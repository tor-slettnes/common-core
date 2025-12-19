// -*- c++ -*-
//==============================================================================
/// @file multilogger-types.c++
/// @brief Logging service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-types.h++"
#include "types/symbolmap.h++"

namespace multilogger
{
    std::ostream &operator<<(std::ostream &stream, const SinkSpec &spec)
    {
        core::types::TaggedValueList tvlist;
        tvlist.emplace_back("sink_id", spec.sink_id);
        tvlist.emplace_back("sink_type", spec.sink_type);
        // tvlist.emplace_back("permanent", spec.permanent);

        tvlist.append_if(spec.contract_id.has_value(),
                         "contract_id",
                         spec.contract_id.value_or(""));

        tvlist.append_if(spec.min_level != core::status::Level::NONE,
                         "min_level",
                         core::str::convert_from(spec.min_level));

        tvlist.append_if(!spec.filename_template.empty(),
                         "filename_template",
                         spec.filename_template);

        tvlist.append_if(spec.rotation_interval,
                         "rotation_interval",
                         spec.rotation_interval);

        tvlist.emplace_back("use_local_time",
                            spec.use_local_time);

        tvlist.emplace_back("compress_after_use",
                            spec.compress_after_use);

        tvlist.append_if(spec.expiration_interval,
                         "expiration_interval",
                         spec.expiration_interval);

        if (!spec.columns.empty())
        {
            core::types::ValueList columns;
            columns << spec.columns;
            tvlist.append({"columns", columns});
        }

        return stream << tvlist;
    }

    std::ostream &operator<<(std::ostream &stream, const ListenerSpec &spec)
    {
        core::types::TaggedValueList tvlist;
        tvlist.emplace_back("sink_id", spec.sink_id);

        tvlist.append_if(spec.min_level != core::status::Level::NONE,
                         "min_level",
                         core::str::convert_from(spec.min_level));

        tvlist.append_if(spec.contract_id.has_value(),
                         "contract_id",
                         spec.contract_id.value_or(""));
        tvlist.to_stream(stream);
        return stream;
    }

}  // namespace multilogger
