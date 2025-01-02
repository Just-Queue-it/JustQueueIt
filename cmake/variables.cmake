
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_program(CCACHE ccache)
if(CCACHE)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif(CCACHE)

option(BUILD_TESTS "Build unit tests tree." OFF)
option(BUILD_BACKEND_SERVER "Build backend server." OFF)

if (DEFINED CONAN_INSTALL_ARGS)
    if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        # https://github.com/conan-io/cmake-conan/issues/577#issuecomment-1781371183
        set(CONAN_INSTALL_ARGS "${CONAN_INSTALL_ARGS};-c tools.build:compiler_executables={\"c\":\"cc\",\"cpp\":\"c++\"}")
    endif()
endif()
if (DEFINED CONAN_ARGS)
    if (BUILD_TESTS)
        set(CONAN_ARGS "${CONAN_ARGS};-o just_queue_it/*:with_tests=True")
    endif()
    if (BUILD_BACKEND_SERVER)
        set(CONAN_ARGS "${CONAN_ARGS};-o just_queue_it/*:with_backend=True")
    endif()
endif()
