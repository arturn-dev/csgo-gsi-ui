cmake_minimum_required(VERSION 3.17)
project(csgo_gsi_ui)

set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

include(thirdparty/thirdparty.cmake)

add_subdirectory(src)