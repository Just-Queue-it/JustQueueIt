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

#include <libraries/backend/data_storage/interface/data_storage.hpp>
#include <libraries/backend/data_storage/interface/data_storage_mock.hpp>
#include <libraries/backend/tasks_manager/tasks_manager.hpp>

TEST_CASE("TasksManager forwards calls to storage")
{
    auto                  mock = std::make_shared<MockDataStorage>();
    backend::TasksManager manager{mock};
    trompeloeil::sequence s{};

    backend::TaskPayload payload{.name = "name", .description = "description"};
    backend::Task        task{.id = 123, .payload = {.name = "name2", .description = "description2"}};

    SUBCASE("CreateTask")
    {
        REQUIRE_CALL(*mock, CreateTask(payload)).RETURN(task).IN_SEQUENCE(s);

        manager.CreateTask(payload);
    }

    SUBCASE("GetTasks")
    {
        const auto res = std::vector{task};
        REQUIRE_CALL(*mock, GetTasks()).RETURN(res).IN_SEQUENCE(s);

        REQUIRE(manager.GetTasks() == res);
    }

    SUBCASE("GetTask")
    {
        REQUIRE_CALL(*mock, GetTask(0)).RETURN(task).IN_SEQUENCE(s);

        REQUIRE(manager.GetTask(0) == task);
    }

    SUBCASE("DeleteTask")
    {
        REQUIRE_CALL(*mock, DeleteTask(0)).IN_SEQUENCE(s);
        manager.DeleteTask(0);
    }
}
