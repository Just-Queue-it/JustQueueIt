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

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>

#include <libraries/backend/data_storage/interface/data_storage.hpp>

struct MockDataStorage final : public trompeloeil::mock_interface<backend::DataStorage>
{
    IMPLEMENT_MOCK1(CreateTask);
    IMPLEMENT_MOCK1(DeleteTask);
    IMPLEMENT_CONST_MOCK1(GetTask);
    IMPLEMENT_CONST_MOCK0(GetTasks);
};
