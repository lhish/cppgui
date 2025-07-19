cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
cmake_policy(VERSION 3.16)

project(sdl_gui)

set(CMAKE_CXX_STANDARD 23)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_definitions(DEBUG)
    add_compile_options(-g -O0)
endif ()

set(SDLTTF_VENDORED ON)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL3" EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL3_ttf" EXCLUDE_FROM_ALL)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/findskia.cmake)

add_subdirectory(modules/common)
add_subdirectory(modules/gui)

add_executable(sdl_gui main.cpp)
add_executable(test_app test/test_app.cpp)

target_link_libraries(sdl_gui PUBLIC sdl_gui_gui)
target_link_libraries(test_app PUBLIC sdl_gui_gui)
