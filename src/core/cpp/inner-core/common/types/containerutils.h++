/// -*- c++ -*-
//==============================================================================
/// @file vectorutils.h++
/// @brief Specialized container wrappers and operators
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <vector>

namespace core::types
{
    template <class T, class BaseContainer = std::vector<T>>
    class Container : public BaseContainer
    {
    public:
        template <class...Args>
        Container(Args&&...args)
            : BaseContainer(std::forward<Args>(args)...)
        {
        }

        Container &extend(const BaseContainer &other)
        {
            this->insert(this->end(), other.begin(), other.end());
            return *this;
        }

        Container &extend(BaseContainer &&other)
        {
            this->reserve(this->size() + other.size());
            for (T &item : other)
            {
                this->push_back(std::move(item));
            }
            return *this;
        }

        template <class InputIt = typename BaseContainer::iterator>
        T &extend(const InputIt &begin, const InputIt &end)
        {
            this->insert(this->end(), begin, end);
            return *this;
        }

        Container &operator=(BaseContainer &&other)
        {
            return this->extend(std::forward<BaseContainer>(other));
        }

        Container &operator+=(BaseContainer &&other)
        {
            return this->extend(std::forward<BaseContainer>(other));
        }

        Container &operator+=(T &&other)
        {
            return this->push_back(std::move(other));
        }
    };
}  // namespace core::types

namespace std
{
    template <class T>
    std::vector<T> operator+(const std::vector<T> &first,
                             const std::vector<T> &second)
    {
        std::vector<T> result;
        result.reserve(first.size() + second.size());
        result.insert(first.end(), first.begin(), first.end());
        result.insert(first.end(), second.begin(), second.end());
        return result;
    }
}  // namespace std
