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

#include <libraries/backend/interface/task/task.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace backend
{
    struct DataStorage;
} // namespace backend

namespace backend
{
    class TasksManager
    {
    public:
        explicit TasksManager(std::shared_ptr<DataStorage> storage);

        Task                CreateTask(const TaskPayload& payload) const;
        std::vector<Task>   GetTasks() const;
        std::optional<Task> GetTask(size_t id) const;
        void                DeleteTask(size_t id) const;

    private:
        std::shared_ptr<DataStorage> m_storage{};
    };
} // namespace backend
