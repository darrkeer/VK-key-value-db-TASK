cmake_minimum_required(VERSION 3.14)

project(KVStorage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
add_link_options(-fsanitize=address)

add_library(treap STATIC
    ./src/Treap.h
    ./src/Treap.cpp
)

add_library(kv-storage STATIC 
    ./src/KVStorage.h 
    ./src/KVStorage.cpp
)

target_link_libraries(kv-storage treap)

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
    unittests/TreapTest.cpp
    unittests/KVStorageTest.cpp
    unittests/MultiThreadTest.cpp
)

target_link_libraries(tests PRIVATE 
    GTest::gtest_main
    kv-storage
)

enable_testing()
add_test(NAME tests COMMAND tests)