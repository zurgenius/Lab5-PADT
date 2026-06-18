cmake_minimum_required(VERSION 3.20)

project(relational_algebra_lab LANGUAGES CXX)

include(FetchContent)

option(RELALG_ENABLE_WERROR "Treat warnings as errors" OFF)
option(WITH_RELALG_GUI "Build Dear ImGui graphical demo" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(RELALG_WARNING_FLAGS -Wall -Wextra -Wpedantic)
if(RELALG_ENABLE_WERROR)
    list(APPEND RELALG_WARNING_FLAGS -Werror)
endif()

add_library(relalg_warnings INTERFACE)
target_compile_options(relalg_warnings INTERFACE ${RELALG_WARNING_FLAGS})

add_library(relalg_core STATIC
    src/utils.cpp
)
target_include_directories(relalg_core PUBLIC include include/core)
target_link_libraries(relalg_core PUBLIC relalg_warnings)

add_executable(relalg_program src/main.cpp)
set_target_properties(relalg_program PROPERTIES OUTPUT_NAME program)
target_include_directories(relalg_program PRIVATE include include/core)
target_link_libraries(relalg_program PRIVATE relalg_core relalg_warnings)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_executable(relalg_tests
    tests/ast_tests.cpp
    tests/core_tests.cpp
    tests/stream_tests.cpp
)
set_target_properties(relalg_tests PROPERTIES OUTPUT_NAME tests_runner)
target_include_directories(relalg_tests PRIVATE include include/core)
target_link_libraries(relalg_tests PRIVATE relalg_core relalg_warnings GTest::gtest_main)
gtest_discover_tests(relalg_tests)

if(WITH_RELALG_GUI)
    FetchContent_Declare(
        glfw
        URL https://github.com/glfw/glfw/archive/refs/tags/3.3.9.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)

    FetchContent_Declare(
        imgui
        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(imgui)

    target_include_directories(relalg_program PRIVATE
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )

    target_sources(relalg_program PRIVATE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )

    target_link_libraries(relalg_program PRIVATE glfw)

    if(UNIX AND NOT APPLE)
        find_package(OpenGL REQUIRED)
        target_link_libraries(relalg_program PRIVATE OpenGL::GL)
    elseif(WIN32)
        target_link_libraries(relalg_program PRIVATE opengl32)
    elseif(APPLE)
        find_library(OPENGL_LIBRARY OpenGL REQUIRED)
        target_compile_definitions(relalg_program PRIVATE GL_SILENCE_DEPRECATION)
        target_link_libraries(relalg_program PRIVATE ${OPENGL_LIBRARY})
    endif()
else()
    target_compile_definitions(relalg_program PRIVATE RELALG_GUI_DISABLED)
endif()
