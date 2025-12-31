/// -*- c++ -*-
//==============================================================================
/// @file listable.h++
/// @brief Base to represent object as a tagged value lsit
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "streamable.h++"
#include "variant-value.h++"
#include "variant-list.h++"
#include "variant-tvlist.h++"
#include "variant-kvmap.h++"

namespace core::types
{
    //--------------------------------------------------------------------------
    /// @brief
    ///    Mix-in class to convert object to a TaggedValueList (e.g. for
    ///    logging, JSON representation)

    class Listable : public Streamable
    {
        // friend Value &operator<<(Value &value, const Listable &obj);
        // friend Value &operator<<(Value &value, const std::shared_ptr<Listable> &ptr);
        // friend ValueList &operator<<(ValueList &list, const Listable &obj);
        // friend TaggedValueList &operator<<(TaggedValueList &tvlist, const Listable &obj);
        // friend KeyValueMap &operator<<(KeyValueMap &kvmap, const Listable &obj);

    public:
        void to_stream(std::ostream &stream) const override;
        [[nodiscard]] TaggedValueList as_tvlist() const;
        [[nodiscard]] KeyValueMap as_kvmap() const;
        [[nodiscard]] ValueList as_valuelist() const;

        virtual void to_tvlist(TaggedValueList *tvlist) const = 0;
    };

}  // namespace core::types
