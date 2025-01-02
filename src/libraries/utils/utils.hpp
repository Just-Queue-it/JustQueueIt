// Copyright (C) 2024-2025 Aleksey Loginov
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Home page: https://github.com/Just-Queue-it/JustQueueIt/

#pragma once

#include <tuple>
#include <type_traits>

#define ENSURE_MSG(expr, msg)          \
    if (!(expr))                       \
    {                                  \
        throw std::runtime_error(msg); \
    }
#define ENSURE(expr) ENSURE_MSG(expr, #expr)


namespace utils
{
    struct ConvertibleToAny
    {
        ConvertibleToAny() = default;

        template<typename T>
        operator T&() const;

        template<typename T>
        operator const T &() const;

        template<typename T>
        operator T&&() const;
    };

    namespace details
    {
        template<template<typename...> typename Base>
        struct Traits
        {
            template<typename... Types>
            constexpr static std::tuple<Types...> ExtractParams(const Base<Types...>*);
            constexpr static std::false_type      ExtractParams(...);
        };
    } // namespace details

    template<typename T, template<typename...> typename Base>
    concept IsBaseOf = !std::is_same_v<decltype(details::Traits<Base>::ExtractParams(std::declval<std::decay_t<T>*>())), std::false_type>;
} // namespace utils
