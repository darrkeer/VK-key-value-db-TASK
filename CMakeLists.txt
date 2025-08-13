cmake_minimum_required(VERSION 3.14)

project(KVStorage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(kv-storage STATIC 
    ./src/KVStorage.h 
    ./src/KVStorage.cpp
)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

if(NOT googletest_POPULATED)
    message(FATAL_ERROR "Failed to fetch GoogleTest")
endif()

add_executable(tests 
    unittests/tests.cpp
)

target_link_libraries(tests PRIVATE 
    GTest::gtest_main
    kv-storage
)

enable_testing()
add_test(NAME tests COMMAND tests)