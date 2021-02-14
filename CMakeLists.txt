cmake_minimum_required(VERSION 3.17)
project(csgo_gsi_ui)

set(CMAKE_CXX_STANDARD 14)

add_executable(csgo_gsi_ui  src/main.cpp
                            src/GSIServer.cpp)

target_include_directories(csgo_gsi_ui PUBLIC cpp-httplib)