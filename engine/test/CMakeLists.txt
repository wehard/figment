cmake_minimum_required(VERSION 3.12)

project(engine_tests)

add_executable(engine_tests)

set_property(TARGET engine_tests PROPERTY CXX_STANDARD 20)

add_dependencies(engine_tests engine)

find_package(GTest CONFIG REQUIRED)

target_sources(engine_tests PRIVATE
        ArenaTests.cpp
        AStarTests.cpp
        DeletionQueueTests.cpp
        HandleTests.cpp
        PoolTests.cpp
        ResourceManagerTests.cpp
)

target_link_libraries(engine_tests PRIVATE
        engine
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
)

enable_testing()
