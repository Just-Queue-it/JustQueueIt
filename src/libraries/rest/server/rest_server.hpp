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

#include <libraries/rest/router/rest_router.hpp>

#include <memory>

namespace rest
{
    struct ServerLifetime;

    class StopHandler
    {
    public:
        explicit StopHandler(std::shared_ptr<ServerLifetime> ctx);
        ~StopHandler() noexcept;
        void Stop() const;

        void Wait() const;

    private:
        std::shared_ptr<ServerLifetime> m_ctx;
    };

    struct ServerConfig
    {
        std::string address = "127.0.0.1";
        uint16_t    port    = 8080;
        size_t      threads = 1;
    };

    StopHandler StartServer(rest::Router&& router, const ServerConfig& config);
} // namespace rest
