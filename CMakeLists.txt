cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
cmake_policy(VERSION 3.16)

project(sdl_gui)

set(CMAKE_CXX_STANDARD 23)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_definitions(DEBUG)
    add_compile_options(-g -O0)
else ()
    add_compile_options(-O3)
endif ()

set(SDLTTF_VENDORED ON)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL3" EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL3_ttf" EXCLUDE_FROM_ALL)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/findskia.cmake)

add_subdirectory(modules/common)
add_subdirectory(modules/gui)

add_executable(sdl_gui main.cpp)
target_link_libraries(sdl_gui PUBLIC sdl_gui_gui)
aux_source_directory(test ALL_TEST)
foreach (TEST ${ALL_TEST})
    get_filename_component(EXE_NAME ${TEST} NAME_WE)
    string(REGEX REPLACE "^.*/" "" module_name "${DIRECTORY_NAME}")
    add_executable("${EXE_NAME}" ${TEST})
    target_link_libraries("${EXE_NAME}" PUBLIC sdl_gui_gui)
endforeach ()