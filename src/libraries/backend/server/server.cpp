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
    rest::StopHandler StartServer(const TasksManager& tasks_manager, const rest::ServerConfig& config)
    {
        rest::Router router{};

        router.AddRoute("/tasks", rest::Request::Method::Get, [tasks_manager](const rest::None&, const rest::Router::Params&) {
            return tasks_manager.GetTasks();
        });

        router.AddRoute("/tasks", rest::Request::Method::Post, [tasks_manager](const TaskPayload& task, const rest::Router::Params&) {
            return tasks_manager.CreateTask(task);
        });

        router.AddRoute("/tasks/{:id}", rest::Request::Method::Get, [tasks_manager](const rest::None&, const rest::Router::Params& params) {
            auto task = tasks_manager.GetTask(std::stoull(params.at("id")));
            return rest::Router::SerializableResponse<std::optional<Task>>{.status_code = task ? rest::Response::Status::Ok : rest::Response::Status::NoContent, .body = std::move(task)};
        });

        router.AddRoute("/tasks/{:id}", rest::Request::Method::Delete, [tasks_manager](const rest::None&, const rest::Router::Params& params) {
            tasks_manager.DeleteTask(std::stoull(params.at("id")));
            return rest::None{};
        });

        return rest::StartServer(std::move(router), config);
    }
} // namespace backend
