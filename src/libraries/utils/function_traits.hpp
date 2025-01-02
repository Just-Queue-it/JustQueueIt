
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

namespace utils
{
    // Lambda
    template<typename T>
    struct FunctionTraits : FunctionTraits<decltype(&T::operator())>
    {
    };

    // Operator of lambda
    template<class T, class R, class... Args>
    struct FunctionTraits<R (T::*)(Args...) const> : FunctionTraits<R (*)(Args...)>
    {
    };

    // Operator of lambda with mutable
    template<class T, class R, class... Args>
    struct FunctionTraits<R (T::*)(Args...)> : FunctionTraits<R (*)(Args...)>
    {
    };

    // Classical global function no args
    template<class R>
    struct FunctionTraits<R (*)()>
    {
        using result = R;
    };

    // Classical global function
    template<class R, class... Args>
    struct FunctionTraits<R (*)(Args...)>
    {
        using result    = R;
        using arguments = std::tuple<std::decay_t<Args>...>;

        template<std::size_t I = 0>
            requires (sizeof...(Args) > I)
        using argument = typename std::tuple_element_t<I, arguments>;
    };

    template<typename T, std::size_t I = 0>
    using DecayedFunctionArgument = typename FunctionTraits<T>::template argument<I>;

    template<typename Fn, typename... Args>
    using DecayedInvokeResult = std::decay_t<std::invoke_result_t<Fn, Args...>>;

} // namespace utils
