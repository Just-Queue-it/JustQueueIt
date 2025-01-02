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

#include <libraries/rest/core/rest_core.hpp>
#include <libraries/utils/function_traits.hpp>
#include <libraries/utils/utils.hpp>
#include <rfl/json.hpp>

#include <regex>
#include <unordered_map>
#include <vector>

namespace rest
{
    template<typename T>
    std::string Serialize(const T& v, rest::ContentType content_type)
    {
        switch (content_type)
        {
        case rest::ContentType::APPLICATION_JSON:
            return rfl::json::write(v);
        case rest::ContentType::TEXT_PLAIN:
            throw std::runtime_error("Unsupported accept content type");
        }
        ENSURE_MSG(false, "Invalid content type");
    }

    template<typename T>
    T DeSerialize(const std::string& v, rest::ContentType content_type)
    {
        switch (content_type)
        {
        case rest::ContentType::APPLICATION_JSON:
            return rfl::json::read<T>(v).value();
        case rest::ContentType::TEXT_PLAIN:
            throw std::runtime_error("Unsupported request content type");
        }
        ENSURE_MSG(false, "Invalid content type");
    }

    template<typename T>
    concept Serializable = requires(const T& v) { Serialize(v, {}); };

    template<typename T>
    concept Deserializable = requires(const T& v) { DeSerialize<T>("", {}); };

    class Router
    {
    public:
        using Params            = std::unordered_map<std::string, std::string>;
        using HandlerWithParams = std::function<Response(const Request&, const Params&)>;

        template<Serializable T>
        struct SerializableResponse
        {
            const NotDefaultConstructible<Response::Status>             status_code;
            const std::variant<NotDefaultConstructible<T>, std::string> body;
        };

        Router() = default;

        /**
         * @brief Adds a new route to the router
         * @param path The URL path pattern (e.g., "/users/{:id}")
         * @param method The HTTP method to handle
         * @param handler The callback to handle matching requests
         * @throws std::regex_error If the path pattern is invalid
         */
        template<std::invocable<utils::ConvertibleToAny, Params> THandler>
        void AddRoute(const std::string& path, Request::Method method, THandler&& handler)
        {
            using Traits        = typename utils::FunctionTraits<THandler>;
            using FirstArgument = std::decay_t<typename Traits::template Argument<0>>;
            using Result        = std::decay_t<typename Traits::Result>;
            if constexpr (std::same_as<FirstArgument, Request>)
            {
                static_assert(std::same_as<Response, Result>);
                return AddRouteImpl(path, method, std::forward<THandler>(handler));
            }
            else
            {
                static_assert(Deserializable<FirstArgument>);
                if constexpr (utils::IsBaseOf<Result, SerializableResponse>)
                {
                    return AddRoute(path, method, [handler = std::forward<THandler>(handler)](const Request& req, const Params& params) {
                        std::optional<FirstArgument> body{};
                        try
                        {
                            if constexpr (std::same_as<FirstArgument, None>)
                                body.emplace();
                            else
                                body = DeSerialize<FirstArgument>(req.body, req.content_type);
                        }
                        catch (const std::exception& e)
                        {
                            return Response{.status_code = Response::Status::BAD_REQUEST, .body = e.what(), .content_type = ContentType::TEXT_PLAIN};
                        }

                        const auto res = handler(body.value(), params);
                        if (const auto body_str = std::get_if<std::string>(&res.body))
                            return Response{.status_code = res.status_code, .body = *body_str, .content_type = ContentType::TEXT_PLAIN};

                        std::string body_str{};
                        try
                        {
                            body_str = Serialize(std::get<0>(res.body).Get(), req.accept_content_type);
                        }
                        catch (const std::exception& e)
                        {
                            return Response{.status_code = Response::Status::BAD_REQUEST, .body = e.what(), .content_type = ContentType::TEXT_PLAIN};
                        }
                        return Response{.status_code = res.status_code, .body = std::move(body_str), .content_type = req.accept_content_type};
                    });
                }
                else
                {
                    static_assert(Deserializable<Result>);
                    return AddRoute(path, method, [handler = std::forward<THandler>(handler)](const FirstArgument& req, const Params& params) {
                        return rest::Router::SerializableResponse<Result>{.status_code = rest::Response::Status::OK, .body = handler(req, params)};
                    });
                }
            }
        }

        /**
         * @brief Routes an incoming request to the appropriate handler
         * @param req The incoming HTTP request
         * @return HTTP response from the matching handler
         */
        [[nodiscard]] Response Route(const Request& req) const;

    private:
        void AddRouteImpl(const std::string& path, Request::Method method, std::function<Response(const Request&, const Params&)> handler);

        struct RouteInfo
        {
            std::regex                                             pattern{};
            std::vector<std::string>                               parameter_names{}; // List of parameter names
            std::unordered_map<Request::Method, HandlerWithParams> handlers{};
        };

        std::unordered_map<std::string, RouteInfo> m_routes;
    };
} // namespace rest
