cmake_minimum_required(VERSION 3.22)

project(figment)

add_subdirectory(engine)
#add_subdirectory(editor)
add_subdirectory(portfolio)
add_subdirectory(vk-app)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
#enable_testing()
#find_package(GTest CONFIG REQUIRED)

#if (NOT EMSCRIPTEN)
#    include(FetchContent)
#    FetchContent_Declare(
#            googletest
#            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
#            DOWNLOAD_EXTRACT_TIMESTAMP true
#    )
#    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#    FetchContent_MakeAvailable(googletest)
#
#    enable_testing()
#
#    include(GoogleTest)
##    gtest_discover_tests(engine_tests)
#endif ()
