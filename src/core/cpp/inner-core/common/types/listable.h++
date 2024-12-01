/// -*- c++ -*-
//==============================================================================
/// @file listable.h++
/// @brief Base to represent object as a tagged value lsit
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "streamable.h++"
#include "value.h++"

namespace core::types
{
    //--------------------------------------------------------------------------
    /// @brief
    ///    Mix-in class to convert object to a TaggedValueList (e.g. for
    ///    logging, JSON representation)

    class Listable : public Streamable
    {
        friend TaggedValueList &operator<<(TaggedValueList &tvlist,
                                           const Listable &obj);

    public:
        void to_stream(std::ostream &stream) const override;
        [[nodiscard]] TaggedValueList as_tvlist() const;
        [[nodiscard]] KeyValueMap as_kvmap() const;
        [[nodiscard]] ValueList as_valuelist() const;

    protected:
        virtual void to_tvlist(TaggedValueList *tvlist) const = 0;
    };

}  // namespace core::types
