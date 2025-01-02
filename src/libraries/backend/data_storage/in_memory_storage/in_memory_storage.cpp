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

#include "in_memory_storage.hpp"

#include <algorithm>

namespace backend::data_storage
{
    InMemoryStorage::InMemoryStorage()  = default;
    InMemoryStorage::~InMemoryStorage() = default;

    Task InMemoryStorage::CreateTask(const TaskPayload& payload)
    {
        std::lock_guard _{m_mutex};
        m_tasks.emplace_back(Task{.id = m_id++, .payload = payload});
        return m_tasks.back();
    }

    void InMemoryStorage::DeleteTask(size_t index)
    {
        std::lock_guard _{m_mutex};
        const auto      itr = std::ranges::lower_bound(m_tasks, index, std::ranges::less{}, &Task::id);
        if (itr == m_tasks.end() || itr->id != index)
            return;

        m_tasks.erase(itr);
    }

    std::optional<Task> InMemoryStorage::GetTask(size_t index) const
    {
        std::lock_guard _{m_mutex};
        const auto      itr = std::ranges::lower_bound(m_tasks, index, std::ranges::less{}, &Task::id);
        if (itr == m_tasks.end() || itr->id != index)
            return {};

        return *itr;
    }

    std::vector<Task> InMemoryStorage::GetTasks() const
    {
        std::shared_lock _{m_mutex};
        return m_tasks;
    }

} // namespace backend::data_storage
