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
        TextPlain,
        ApplicationJson
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

        const T& get() const { return m_value; }

    private:
        T m_value;
    };
    struct Request
    {
        enum class Method
        {
            Get,
            Post,
            Put,
            Delete,
            Patch,
            Head,
            Options,
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
            Continue           = 100,
            SwitchingProtocols = 101,
            Processing         = 102,
            EarlyHints         = 103,

            // Successful responses (200–299)
            Ok                          = 200,
            Created                     = 201,
            Accepted                    = 202,
            NonAuthoritativeInformation = 203,
            NoContent                   = 204,
            ResetContent                = 205,
            PartialContent              = 206,
            MultiStatus                 = 207,
            AlreadyReported             = 208,
            ImUsed                      = 226,

            // Redirection messages (300–399)
            MultipleChoices   = 300,
            MovedPermanently  = 301,
            Found             = 302,
            SeeOther          = 303,
            NotModified       = 304,
            UseProxy          = 305,
            TemporaryRedirect = 307,
            PermanentRedirect = 308,

            // Client error responses (400–499)
            BadRequest                  = 400,
            Unauthorized                = 401,
            PaymentRequired             = 402,
            Forbidden                   = 403,
            NotFound                    = 404,
            MethodNotAllowed            = 405,
            NotAcceptable               = 406,
            ProxyAuthenticationRequired = 407,
            RequestTimeout              = 408,
            Conflict                    = 409,
            Gone                        = 410,
            LengthRequired              = 411,
            PreconditionFailed          = 412,
            PayloadTooLarge             = 413,
            UriTooLong                  = 414,
            UnsupportedMediaType        = 415,
            RangeNotSatisfiable         = 416,
            ExpectationFailed           = 417,
            ImaTeapot                   = 418, // Easter egg (RFC 2324)
            MisdirectedRequest          = 421,
            UnprocessableEntity         = 422,
            Locked                      = 423,
            FailedDependency            = 424,
            TooEarly                    = 425,
            UpgradeRequired             = 426,
            PreconditionRequired        = 428,
            TooManyRequests             = 429,
            RequestHeaderFieldsTooLarge = 431,
            UnavailableForLegalReasons  = 451,

            // Server error responses (500–599)
            InternalServerError           = 500,
            NotImplemented                = 501,
            BadGateway                    = 502,
            ServiceUnavailable            = 503,
            GatewayTimeout                = 504,
            HttpVersionNotSupported       = 505,
            VariantAlsoNegotiates         = 506,
            InsufficientStorage           = 507,
            LoopDetected                  = 508,
            NotExtended                   = 510,
            NetworkAuthenticationRequired = 511
        };

        NotDefaultConstructible<Status> status_code;
        std::string                     body{};

        NotDefaultConstructible<ContentType> content_type;
    };

    std::string_view                 ParseContentType(rest::ContentType content_type);
    std::optional<rest::ContentType> ParseContentType(std::string_view content_type);

} // namespace rest
