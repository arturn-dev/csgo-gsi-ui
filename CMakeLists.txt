cmake_minimum_required(VERSION 3.17)
project(csgo_gsi_ui)

set(CMAKE_CXX_STANDARD 17)
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(thirdparty/thirdparty.cmake)

add_subdirectory(src)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/662fe38e44900c007eccb65a5d2ea19df7bd520e.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(hello_test test/GSIServerTest.cpp src/GSIServer.cpp test/DataProviderTest.cpp src/DataProvider.cpp)
target_link_libraries(hello_test gtest_main)
target_include_directories(hello_test PUBLIC "${HTTPLIB_INCLUDE_DIR}")
target_include_directories(hello_test PUBLIC "${NLOHMANN_INCLUDE_DIR}")
target_include_directories(hello_test PUBLIC "${PLOG_INCLUDE_DIR}")
include(GoogleTest)
gtest_discover_tests(hello_test)