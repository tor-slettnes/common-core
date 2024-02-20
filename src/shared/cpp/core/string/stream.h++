/// -*- c++ -*-
//==============================================================================
/// @file stream.h++
/// @brief Output representation for miscellaneous STL types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "format.h++"

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <optional>
#include <memory>

using namespace std::literals::string_literals;  // ""s

namespace shared::stream
{
    template <class Sequence>
    std::ostream &write_sequence(std::ostream &stream, const Sequence &seq)
    {
        stream << "{";
        std::string sep = "";
        for (const auto &item : seq)
        {
            shared::str::format(stream, "%s%r", sep, item);
            sep = ", ";
        }
        stream << "}";
        return stream;
    }

    template <class Map>
    std::ostream &write_map(std::ostream &stream, const Map &map)
    {
        stream << "{";
        std::string sep = "";
        for (const auto &[key, value] : map)
        {
            shared::str::format(stream, "%s%r: %r", sep, key, value);
            sep = ", ";
        }
        stream << "}";
        return stream;
    }

    template <class Dereferenced>
    std::ostream &write_dereferenced(std::ostream &stream, const Dereferenced &value)
    {
        if (value)
        {
            shared::str::format(stream, "{%r}", *value);
        }
        else
        {
            stream << "{}";
        }
        return stream;
    }
}  // namespace shared::stream

namespace std
{
    template <class T1, class T2>
    std::ostream &operator<<(std::ostream &stream, const std::pair<T1, T2> &pair)
    {
        shared::str::format(stream, "{%r: %r}", pair.first, pair.second);
        return stream;
    }

    template <class T>
    std::ostream &operator<<(std::ostream &stream, const std::vector<T> &vector)
    {
        return shared::stream::write_sequence(stream, vector);
    }

    template <class T>
    std::ostream &operator<<(std::ostream &stream, const std::list<T> &list)
    {
        return shared::stream::write_sequence(stream, list);
    }

    template <class K, class V>
    std::ostream &operator<<(std::ostream &stream, const std::map<K, V> &map)
    {
        return shared::stream::write_map(stream, map);
    }

    template <class K, class V>
    std::ostream &operator<<(std::ostream &stream, const std::unordered_map<K, V> &map)
    {
        return shared::stream::write_map(stream, map);
    }

    template <class V>
    std::ostream &operator<<(std::ostream &stream, const std::set<V> &set)
    {
        return shared::stream::write_sequence(stream, set);
    }

    template <class V>
    std::ostream &operator<<(std::ostream &stream, const std::unordered_set<V> &set)
    {
        return shared::stream::write_sequence(stream, set);
    }

    template <class K, class V>
    std::ostream &operator<<(std::ostream &stream, const std::multimap<K, V> &map)
    {
        return shared::stream::write_map(stream, map);
    }

    template <class K, class V>
    std::ostream &operator<<(std::ostream &stream, const std::unordered_multimap<K, V> &map)
    {
        return shared::stream::write_map(stream, map);
    }

    template <class V>
    std::ostream &operator<<(std::ostream &stream, const std::multiset<V> &set)
    {
        return shared::stream::write_sequence(stream, set);
    }

    template <class V>
    std::ostream &operator<<(std::ostream &stream, const std::unordered_multiset<V> &set)
    {
        return shared::stream::write_sequence(stream, set);
    }

    template <class V>
    std::ostream &operator<<(std::ostream &stream, const std::optional<V> &opt)
    {
        return shared::stream::write_dereferenced(stream, opt);
    }

    template <class V>
    std::ostream &operator<<(std::ostream &stream, const std::shared_ptr<V> &ptr)
    {
        return shared::stream::write_dereferenced(stream, ptr);
    }

    template <class V>
    std::ostream &operator<<(std::ostream &stream, const std::unique_ptr<V> &ptr)
    {
        return shared::stream::write_dereferenced(stream, ptr);
    }

    template <class V>
    std::ostream &operator<<(std::ostream &stream, const std::weak_ptr<V> &weakptr)
    {
        return shared::stream::write_dereferenced(stream, weakptr.lock());
    }

}  // namespace std
