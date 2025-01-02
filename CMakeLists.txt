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

cmake_minimum_required(VERSION 3.14)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds are not supported. "
      "Please read the BUILDING document before trying to build this project. "
      "You may need to delete 'CMakeCache.txt' and 'CMakeFiles/' first.")
endif()

project(
  JustQueueIt
  VERSION 0.0.1
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/Just-Queue-it/JustQueueIt"
  LANGUAGES C CXX)

include(cmake/variables.cmake)
include(cmake/dependencies.cmake)
include(cmake/macros.cmake)
include(cmake/coverage.cmake)

add_subdirectory(src)
