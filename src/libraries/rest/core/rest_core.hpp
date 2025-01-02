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

#include <optional>
#include <string>

namespace rest
{
    struct None
    {
    };

    enum class ContentType
    {
        TEXT_PLAIN,
        APPLICATION_JSON
    };

    template<typename T>
    class NotDefaultConstructible
    {
    public:
        NotDefaultConstructible() = delete;
        NotDefaultConstructible(const T& v)
            : m_value(v)
        {
        }
        NotDefaultConstructible(T&& v)
            : m_value(std::move(v))
        {
        }

        auto operator==(const T& rhs) const { return m_value == rhs; }
        auto operator<=>(const T& rhs) const { return m_value <=> rhs; }

        auto operator<=>(const NotDefaultConstructible& rhs) const = default;

        operator const T &() const { return m_value; }

        const T& Get() const { return m_value; }

    private:
        T m_value;
    };
    struct Request
    {
        enum class Method
        {
            GET,
            POST,
            PUT,
            DELETE,
            PATCH,
            HEAD,
            OPTIONS,
        };

        const NotDefaultConstructible<Method> method;
        const std::string                     path{};
        const std::string                     body{};

        const NotDefaultConstructible<ContentType> content_type;
        const ContentType                          accept_content_type = content_type;
    };

    struct Response
    {
        enum class Status
        {
            CONTINUE            = 100,
            SWITCHING_PROTOCOLS = 101,
            PROCESSING          = 102,
            EARLY_HINTS         = 103,

            // Successful responses (200–299)
            OK                            = 200,
            CREATED                       = 201,
            ACCEPTED                      = 202,
            NON_AUTHORITATIVE_INFORMATION = 203,
            NO_CONTENT                    = 204,
            RESET_CONTENT                 = 205,
            PARTIAL_CONTENT               = 206,
            MULTI_STATUS                  = 207,
            ALREADY_REPORTED              = 208,
            IM_USED                       = 226,

            // Redirection messages (300–399)
            MULTIPLE_CHOICES   = 300,
            MOVED_PERMANENTLY  = 301,
            FOUND              = 302,
            SEE_OTHER          = 303,
            NOT_MODIFIED       = 304,
            USE_PROXY          = 305,
            TEMPORARY_REDIRECT = 307,
            PERMANENT_REDIRECT = 308,

            // Client error responses (400–499)
            BAD_REQUEST                     = 400,
            UNAUTHORIZED                    = 401,
            PAYMENT_REQUIRED                = 402,
            FORBIDDEN                       = 403,
            NOT_FOUND                       = 404,
            METHOD_NOT_ALLOWED              = 405,
            NOT_ACCEPTABLE                  = 406,
            PROXY_AUTHENTICATION_REQUIRED   = 407,
            REQUEST_TIMEOUT                 = 408,
            CONFLICT                        = 409,
            GONE                            = 410,
            LENGTH_REQUIRED                 = 411,
            PRECONDITION_FAILED             = 412,
            PAYLOAD_TOO_LARGE               = 413,
            URI_TOO_LONG                    = 414,
            UNSUPPORTED_MEDIA_TYPE          = 415,
            RANGE_NOT_SATISFIABLE           = 416,
            EXPECTATION_FAILED              = 417,
            IMA_TEAPOT                      = 418, // Easter egg (RFC 2324)
            MISDIRECTED_REQUEST             = 421,
            UNPROCESSABLE_ENTITY            = 422,
            LOCKED                          = 423,
            FAILED_DEPENDENCY               = 424,
            TOO_EARLY                       = 425,
            UPGRADE_REQUIRED                = 426,
            PRECONDITION_REQUIRED           = 428,
            TOO_MANY_REQUESTS               = 429,
            REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
            UNAVAILABLE_FOR_LEGAL_REASONS   = 451,

            // Server error responses (500–599)
            INTERNAL_SERVER_ERROR           = 500,
            NOT_IMPLEMENTED                 = 501,
            BAD_GATEWAY                     = 502,
            SERVICE_UNAVAILABLE             = 503,
            GATEWAY_TIMEOUT                 = 504,
            HTTP_VERSION_NOT_SUPPORTED      = 505,
            VARIANT_ALSO_NEGOTIATES         = 506,
            INSUFFICIENT_STORAGE            = 507,
            LOOP_DETECTED                   = 508,
            NOT_EXTENDED                    = 510,
            NETWORK_AUTHENTICATION_REQUIRED = 511
        };

        NotDefaultConstructible<Status> status_code;
        std::string                     body{};

        NotDefaultConstructible<ContentType> content_type;
    };

    std::string_view                 ParseContentType(rest::ContentType content_type);
    std::optional<rest::ContentType> ParseContentType(std::string_view content_type);

} // namespace rest
