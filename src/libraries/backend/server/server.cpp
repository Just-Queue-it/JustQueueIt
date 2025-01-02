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

#include "server.hpp"

#include <libraries/rest/router/rest_router.hpp>

namespace backend
{
    namespace
    {
        std::optional<size_t> SafeStoull(const std::string& str)
        {
            try
            {
                return std::stoull(str);
            }
            catch (...)
            {
                return {};
            }
        }
    } // namespace
    rest::StopHandler StartServer(const TasksManager& tasks_manager, const rest::ServerConfig& config)
    {
        rest::Router router{};

        router.AddRoute("/tasks", rest::Request::Method::GET, [tasks_manager](const rest::None&, const rest::Router::Params&) {
            return tasks_manager.GetTasks();
        });

        router.AddRoute("/tasks", rest::Request::Method::POST, [tasks_manager](const TaskPayload& task, const rest::Router::Params&) {
            return tasks_manager.CreateTask(task);
        });

        router.AddRoute("/tasks/{:id}", rest::Request::Method::GET, [tasks_manager](const rest::None&, const rest::Router::Params& params) {
            const auto id = SafeStoull(params.at("id"));
            if (!id)
                throw std::runtime_error{"Can't parse id of task"};
            if (const auto task = tasks_manager.GetTask(id.value()))
                return rest::Router::SerializableResponse<Task>{.status_code = rest::Response::Status::OK, .body = task.value()};
            return rest::Router::SerializableResponse<Task>{.status_code = rest::Response::Status::NOT_FOUND, .body = "Not found"};
        });

        router.AddRoute("/tasks/{:id}", rest::Request::Method::DELETE_, [tasks_manager](const rest::None&, const rest::Router::Params& params) {
            const auto id = SafeStoull(params.at("id"));
            if (!id)
                throw std::runtime_error{"Can't parse id of task"};
            tasks_manager.DeleteTask(id.value());
            return rest::None{};
        });

        return rest::StartServer(std::move(router), config);
    }
} // namespace backend
