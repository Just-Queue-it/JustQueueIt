# Copyright (C) 2024-2025 Aleksey Loginov
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Home page: https://github.com/Just-Queue-it/JustQueueIt/

add_subdirectory(in_memory_storage)
add_subdirectory(interface)

tq_add_test_executable_in_ut_folder(
    TARGET_NAME
        storages_ut
    PRIVATE
        in_memory_storage
)
