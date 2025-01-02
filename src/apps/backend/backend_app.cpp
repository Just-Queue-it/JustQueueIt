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

#include <libraries/backend/data_storage/in_memory_storage/in_memory_storage.hpp>
#include <libraries/backend/server/server.hpp>
#include <libraries/backend/tasks_manager/tasks_manager.hpp>

int main()
{
    backend::TasksManager tasks_manager{std::make_shared<backend::data_storage::InMemoryStorage>()};
    const auto            server = backend::StartServer(tasks_manager, rest::ServerConfig{.port = 8080});
    server.Wait();
    return 0;
}
