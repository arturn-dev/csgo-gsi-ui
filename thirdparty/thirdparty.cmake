# OpenGL
find_package(OpenGL REQUIRED)
set(OPENGL_LIBRARY ${OPENGL_LIBRARIES})

# glfw
find_library(GLFW_LIBRARY "glfw" "/usr/lib" "/usr/local/lib")
find_path(GLFW_INCLUDE_DIR "glfw/glfw.h" "/usr/include" "/usr/local/include")

if ((NOT GLFW_LIBRARY) OR (NOT GLFW_INCLUDE_DIR))
    set(GLFW_DIR "${THIRDPARTY_DIR}/glfw")

    message("Unable to find glfw, cloning...")
    execute_process(COMMAND git submodule update --init ${GLFW_DIR}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory("${GLFW_DIR}")

    set(GLFW_LIBRARY "glfw" "${GLFW_LIBRARIES}")
    set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")
endif ()

# glad
set(GLAD_DIR "${THIRDPARTY_DIR}/glad")
add_library("glad" STATIC "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")

set(GLAD_LIBRARY "glad")
set(GLAD_INCLUDE_DIR "${GLAD_DIR}/include")

# imgui
set(IMGUI_DIR "${THIRDPARTY_DIR}/imgui")
execute_process(COMMAND git submodule update --init ${IMGUI_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_library("imgui" STATIC "${IMGUI_DIR}/imgui.cpp"
        "${IMGUI_DIR}/imgui_demo.cpp"
        "${IMGUI_DIR}/imgui_draw.cpp"
        "${IMGUI_DIR}/imgui_widgets.cpp")
target_include_directories("imgui" PRIVATE "${IMGUI_DIR}")

set(IMGUI_LIBRARY "imgui")
set(IMGUI_INCLUDE_DIR "${IMGUI_DIR}")

# httplib, nlohmann::json
set(HTTPLIB_INCLUDE_DIR "${THIRDPARTY_DIR}/httplib")
set(NLOHMANN_INCLUDE_DIR "${THIRDPARTY_DIR}/nlohmann")
set(PLOG_INCLUDE_DIR "${THIRDPARTY_DIR}/plog/include")