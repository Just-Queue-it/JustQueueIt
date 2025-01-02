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

#include "rest_server.hpp"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>

#include <iostream>
#include <thread>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
using tcp       = boost::asio::ip::tcp; // NOLINT

namespace rest
{
    namespace
    {
        void LogError(const std::exception_ptr& e)
        {
            if (e)
            {
                try
                {
                    std::rethrow_exception(e);
                }
                catch (const boost::system::system_error& err)
                {
                    if (err.code() == boost::beast::http::error::end_of_stream)
                        return;
                    std::cerr << "Error in session: " << err.what() << "\n";
                }
                catch (std::exception& e)
                {
                    std::cerr << "Error in session: " << e.what() << "\n";
                }
            }
        };

        struct ServerContext
        {
            explicit ServerContext(Router&& router)
                : router(std::move(router))
            {
            }

            Router           router;
            std::atomic_bool started{};
        };

        std::optional<rest::Request::Method> ParseMethod(http::verb method)
        {
            switch (method)
            {
            case http::verb::unknown:
            case http::verb::get: return rest::Request::Method::GET;
            case http::verb::post: return rest::Request::Method::POST;
            case http::verb::put: return rest::Request::Method::PUT;
            case http::verb::delete_: return rest::Request::Method::DELETE;
            case http::verb::patch: return rest::Request::Method::PATCH;
            case http::verb::head: return rest::Request::Method::HEAD;
            case http::verb::options: return rest::Request::Method::OPTIONS;
            default: return {};
            }
        }

        boost::beast::http::response<boost::beast::http::string_body> CreateResponse(const rest::Response& response)
        {
            boost::beast::http::response<boost::beast::http::string_body> res;
            res.result(static_cast<uint16_t>(response.status_code.Get()));
            res.set(http::field::server, "JustQueueIt");
            res.set(http::field::content_type, ParseContentType(response.content_type));
            res.body() = response.body;
            res.prepare_payload();
            return res;
        }

        Response PrepareResponse(const http::request<http::string_body>& req, const Router& router)
        {
            const auto method = ParseMethod(req.method());
            if (!method)
                return Response{.status_code = Response::Status::METHOD_NOT_ALLOWED, .body = "Unsupported or unknown method", .content_type = rest::ContentType::TEXT_PLAIN};

            const auto content_type = ParseContentType(req[http::field::content_type]);
            if (!content_type)
                return Response{.status_code = Response::Status::BAD_REQUEST, .body = "Unsupported or unknown content type", .content_type = rest::ContentType::TEXT_PLAIN};

            const auto accept_content_type = req[http::field::accept] != "*/*" ? ParseContentType(req[http::field::accept]) : content_type;
            if (!accept_content_type)
                return Response{.status_code = Response::Status::BAD_REQUEST, .body = "Unsupported or unknown accept content type", .content_type = rest::ContentType::TEXT_PLAIN};

            return router.Route({.method = method.value(), .path = req.target(), .body = req.body(), .content_type = content_type.value(), .accept_content_type = accept_content_type.value()});
        }

        net::awaitable<void> DoSession(beast::tcp_stream stream, std::shared_ptr<ServerContext> ctx)
        {
            // This buffer is required to persist across reads
            beast::flat_buffer buffer;
            while (true)
            {
                stream.expires_after(std::chrono::seconds(30));

                http::request<http::string_body> req;
                co_await http::async_read(stream, buffer, req);

                bool keep_alive = req.keep_alive();

                auto response = CreateResponse(PrepareResponse(req, ctx->router));
                response.keep_alive(keep_alive);
                co_await beast::async_write(stream, boost::beast::http::message_generator{std::move(response)});

                // Send a TCP shutdown
                if (keep_alive)
                    continue;

                stream.socket().shutdown(net::ip::tcp::socket::shutdown_send);
                break;
            }
        }

        net::awaitable<void> DoListen(net::ip::tcp::endpoint endpoint, std::shared_ptr<ServerContext> ctx)
        {
            auto acceptor = net::use_awaitable.as_default_on(tcp::acceptor(co_await net::this_coro::executor));
            acceptor.open(endpoint.protocol());

            // Allow address reuse
            acceptor.set_option(net::socket_base::reuse_address(true));

            // Bind to the server address
            acceptor.bind(endpoint);

            // Start listening for connections
            acceptor.listen(net::socket_base::max_listen_connections);

            ctx->started.store(true);
            ctx->started.notify_all();

            for (;;)
            {
                boost::asio::co_spawn(
                    acceptor.get_executor(),
                    DoSession(boost::beast::tcp_stream(co_await acceptor.async_accept()), ctx),
                    &LogError);
            }
        }
    } // namespace

    struct ServerLifetime
    {
        explicit ServerLifetime(int threads)
            : ioc{threads}
        {
        }

        boost::asio::io_context  ioc;
        std::vector<std::thread> threads{};
    };

    StopHandler::StopHandler(std::shared_ptr<ServerLifetime> ctx)
        : m_ctx{std::move(ctx)}
    {
    }

    StopHandler::~StopHandler() noexcept
    {
        Stop();
    }

    void StopHandler::Stop() const
    {
        if (!m_ctx->ioc.stopped())
        {
            m_ctx->ioc.stop();
            Wait();
        }
    }

    void StopHandler::Wait() const
    {
        for (auto& t : m_ctx->threads)
            if (t.joinable())
                t.join();
    }

    StopHandler StartServer(Router&& router, const ServerConfig& config)
    {
        auto server_ctx = std::make_shared<ServerContext>(std::move(router));

        const auto max_threads     = std::max(size_t{1}, config.threads);
        auto       server_lifetime = std::make_shared<ServerLifetime>(max_threads);

        net::co_spawn(server_lifetime->ioc, DoListen(net::ip::tcp::endpoint{net::ip::make_address(config.address), config.port}, server_ctx), &LogError);

        for (size_t threads = 0; threads < max_threads; ++threads)
        {
            server_lifetime->threads.emplace_back([server_lifetime] {
                server_lifetime->ioc.run();
            });
        }

        server_ctx->started.wait(false);

        return StopHandler{std::move(server_lifetime)};
    }
} // namespace rest
