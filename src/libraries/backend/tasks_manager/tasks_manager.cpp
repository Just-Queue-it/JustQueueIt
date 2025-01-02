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

#include "tasks_manager.hpp"

#include <libraries/backend/data_storage/interface/data_storage.hpp>

#include <utility>

namespace backend
{
    TasksManager::TasksManager(std::shared_ptr<DataStorage> storage)
        : m_storage{std::move(storage)}
    {
    }

    Task TasksManager::CreateTask(const TaskPayload& payload) const
    {
        return m_storage->CreateTask(payload);
    }

    std::vector<Task> TasksManager::GetTasks() const
    {
        return m_storage->GetTasks();
    }

    std::optional<Task> TasksManager::GetTask(size_t id) const
    {
        return m_storage->GetTask(id);
    }

    void TasksManager::DeleteTask(size_t id) const
    {
        m_storage->DeleteTask(id);
    }

} // namespace backend
