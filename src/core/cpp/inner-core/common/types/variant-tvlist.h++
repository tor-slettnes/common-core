/// -*- c++ -*-
//==============================================================================
/// @file variant-tvlist.h++
/// @brief Tag/Value list
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "variant-value.h++"
#include "types/streamable.h++"
#include "types/create-shared.h++"

namespace core::types
{
    class TaggedValueList : public std::vector<TaggedValue>,
                            public Streamable,
                            public enable_create_shared<TaggedValueList>
    {
        using Super = std::vector<TaggedValue>;
        using AppendResult = std::pair<TaggedValueList::iterator, bool>;

    public:
        using ptr = std::shared_ptr<TaggedValueList>;

    protected:
        // Inherit constructors from std::vector<>
        using Super::Super;

    public:
        TaggedValueList::const_iterator find(
            const Tag &tag,
            bool ignoreCase = false) const noexcept;

        TaggedValueList::iterator find(
            const Tag &tag,
            bool ignoreCase = false) noexcept;

        // No-ops for compatibility with std::vector<>;
        // void reserve(std::size_t capacity) {}
        // void shrink_to_fit() {}

        // Inherit .at(position) from std::deque
        using Super::at;

        // Additional signature to obtain the first encountered value with a specific tag.
        Value &at(const Tag &tag);
        const Value &at(const Tag &tag) const;

        // Inherit [] operator from std::deque
        using Super::operator[];

        // Additional signature to obtain reference to the first encountered value with
        // a specific tag, inserting a new element if necessary.
        Value &operator[](const Tag &tag) noexcept;

        /// @brief Indicate whether any tags are present.
        bool tagged() const noexcept;

        /// @brief Indicate whether every item in this list contains a unique tag
        bool mappable() const noexcept;

        /// Get the first value
        const Value &front(const Value &fallback = {}) const noexcept;

        /// Get the last value
        const Value &back(const Value &fallback = {}) const noexcept;

        /// Get the first value with the specified tag, or a default value if not found.
        Value get(
            const Tag &tag,
            const Value &fallback = {},
            bool ignoreCase = false) const noexcept;

        Value get_any(
            const std::vector<std::string> &candidates,
            const Value &fallback = {},
            bool ignoreCase = false) const noexcept;

        /// Get the value at the specified index, or a default value if not in range
        Value get(uint index, const Value &fallback = {}) const noexcept;
        Value get(int index, const Value &fallback = {}) const noexcept;

        /// Get the value at the specified index, or a default value if not in range
        std::optional<Value> try_get(const Tag &tag, bool ignoreCase) const noexcept;
        std::optional<Value> try_get(uint index) const noexcept;
        std::optional<Value> try_get(int index) const noexcept;

        /// Return the tag for each item
        TagList tags() const noexcept;

        /// Return the value for each item
        ValueList values() const noexcept;

        // Convert this list into a Key/Value map
        KeyValueMap as_kvmap() const noexcept;

        // Convert this list into a Key/Value map
        KeyValueMapPtr as_kvmap_ptr() const noexcept;

        /// Return a copy where items with empty tags are filtered out
        TaggedValueList filtered_tags() const noexcept;

        /// Return a copy where items with empty values are filtered out
        TaggedValueList filtered_values() const noexcept;

        /// @brief
        ///    Add tagged values from an existing TaggedValueList instance
        /// @param[in] other
        ///    TaggedValueList instance from which to import
        /// @return
        ///     A reference to this updated instance.
        TaggedValueList &extend(const TaggedValueList &other);
        TaggedValueList &extend(TaggedValueList &&other);

        /// @brief
        ///    Replace tagged values in this list with ones from another
        /// @param[in] other
        ///    TaggedValueList instance from which to import
        /// @return
        ///     A reference to this updated instance.
        TaggedValueList &update(const TaggedValueList &other);
        TaggedValueList &update(TaggedValueList &&other);

        /// @brief
        ///    Extract items from an existing TaggedValueList and insert here.
        /// @param[in] other
        ///    TaggedValueList instance from which to extract items.
        /// @return
        ///    A reference to this instance.
        /// @note
        ///    TaggedValue instances are extracted from `other` and
        ///    appended to this list only if not already present.
        TaggedValueList &merge(TaggedValueList &other);
        TaggedValueList &merge(TaggedValueList &&other);

        /// @brief
        ///    Push an element at the end of the list if a boolean condition is satisfied
        /// @param[in] tv
        ///    Tagged Value to push

        TaggedValueList::iterator append(const TaggedValue &tv);
        TaggedValueList::iterator append(const Value &value);
        TaggedValueList::iterator append(const Tag &tag, const Value &value);

        AppendResult append_if(bool condition, const TaggedValue &tv);
        AppendResult append_if(bool condition, const Value &value);
        AppendResult append_if(bool condition, const Tag &tag, const Value &value);

        AppendResult append_if_value(const TaggedValue &tv);
        AppendResult append_if_value(const Value &value);
        AppendResult append_if_value(const Tag &tag, const Value &value);

    public:
        void to_stream(std::ostream &stream) const override;
        void to_stream(std::ostream &stream,
                       const std::string &prefix,
                       const std::string &infix,
                       const std::string &postfix) const;

        template <class T>
        std::optional<T> try_get_as(const std::string &key, bool ignore_case = false) const
        {
            return this->get(key, {}, ignore_case).try_convert_to<T>();
        }

        template <class T>
        std::optional<T> try_get_as(const uint index) const
        {
            return this->get(index).try_convert_to<T>();
        }

        template <class T>
        static std::shared_ptr<TaggedValueList> create_shared_from(const T &obj)
        {
            auto tvlist = std::make_shared<TaggedValueList>();
            (*tvlist) << obj;
            return tvlist;
        }

        template <class T>
        static TaggedValueList create_from(const T &obj)
        {
            TaggedValueList tvlist;
            tvlist << obj;
            return tvlist;
        }
    };

    //--------------------------------------------------------------------------
    // Non-member functions

    template <class T>
    TaggedValueList &operator<<(TaggedValueList &tvlist, const T &inputs)
    {
        tvlist.reserve(tvlist.size() + inputs.size());
        for (const auto &[tag, value] : inputs)
        {
            auto &tv = tvlist.emplace_back();
            tv.first = tag;
            tv.second << value;
        }
        return tvlist;
    }

}  // namespace core::types
