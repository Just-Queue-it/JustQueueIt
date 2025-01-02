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

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>

#include <boost/beast.hpp>
#include <libraries/rest/server/rest_server.hpp>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
using tcp       = net::ip::tcp; // NOLINT

auto MakeRequest(const std::string& path, const rest::ServerConfig& config, http::verb method = http::verb::get, const std::string& content_type = "text/plain", const std::string& accept_content_type = "text/plain")
{
    net::io_context ioc;

    // These objects perform our I/O
    tcp::resolver     resolver(ioc);
    beast::tcp_stream stream(ioc);

    // Look up the domain name
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(config.address), config.port);

    // Make the connection on the IP address we get from a lookup
    stream.connect(endpoint);

    // Set up an HTTP GET request message
    http::request<http::string_body> req{method, path, 10};
    req.set(http::field::content_type, content_type);
    req.set(http::field::accept, accept_content_type);

    // Send the HTTP request to the remote host
    http::write(stream, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::string_body> res;

    // Receive the HTTP response
    http::read(stream, buffer, res);
    return res;
}

struct Routes
{
    MAKE_MOCK2(Method, (rest::Response)(rest::Request, rest::Router::Params));
};

TEST_CASE("BackendServer provides correct api")
{
    Routes mock;
    auto   router = rest::Router{};
    router.AddRoute("/test", rest::Request::Method::GET, [&mock](const rest::Request& req, const rest::Router::Params& params) { return mock.Method(req, params); });

    const auto config     = rest::ServerConfig();
    auto       stop_token = rest::StartServer(std::move(router), config);

    SUBCASE("get /invalid")
    {
        const auto resp = MakeRequest("/invalid", config);
        CHECK(resp.result() == http::status::not_found);
        CHECK(resp.body() == "Not found");
    }

    SUBCASE("get /test")
    {
        REQUIRE_CALL(mock, Method(trompeloeil::_, trompeloeil::_)).RETURN(rest::Response{.status_code = rest::Response::Status::OK, .body = "test", .content_type = rest::ContentType::APPLICATION_JSON});

        const auto resp = MakeRequest("/test", config);
        CHECK(resp.result() == http::status::ok);
        CHECK(resp.body() == "test");
        CHECK(resp[http::field::content_type] == rest::ParseContentType(rest::ContentType::APPLICATION_JSON));
    }
    SUBCASE("invalid method")
    {
        const auto resp = MakeRequest("/test", config, http::verb::lock);
        CHECK(resp.result() == http::status::method_not_allowed);
        CHECK(resp.body() == "Unsupported or unknown method");
    }
    SUBCASE("invalid content-type")
    {
        const auto resp = MakeRequest("/test", config, http::verb::get, "123asf");
        CHECK(resp.result() == http::status::bad_request);
        CHECK(resp.body() == "Unsupported or unknown content type");
    }
    SUBCASE("invalid accept-content-type")
    {
        const auto resp = MakeRequest("/test", config, http::verb::get, "text/plain", "123asf");
        CHECK(resp.result() == http::status::bad_request);
        CHECK(resp.body() == "Unsupported or unknown accept content type");
    }

    stop_token.Stop();
}
