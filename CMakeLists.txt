cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
cmake_policy(VERSION 3.16)
project(sdl_gui)
set(CMAKE_CXX_STANDARD 23)
set(SDLTTF_VENDORED ON)
add_compile_options(-g -O0)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL3" EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL3_ttf" EXCLUDE_FROM_ALL)
set(SKIA_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/skia_install/skia/out/Static_arm")
if (NOT EXISTS "${SKIA_BUILD_DIR}/libskia.a") # 假设存在 libskia.a
    message(FATAL_ERROR "Skia static libraries not found in ${SKIA_BUILD_DIR}. Please ensure Skia is built correctly for arm64.")
endif ()
set(SKIA_INCLUDE_DIR "${SKIA_BUILD_DIR}/../../")
link_directories(${SKIA_BUILD_DIR})
include_directories(${SKIA_INCLUDE_DIR})
add_executable(sdl_gui test.cpp)
target_link_libraries(sdl_gui PRIVATE SDL3_ttf::SDL3_ttf SDL3::SDL3)
target_link_libraries(sdl_gui PRIVATE
        # OpenGL 和 Vulkan 相关的库 (如果 Skia 使用了 GL 或 Vulkan)
        # -framework OpenGL (macOS)
        # -framework CoreFoundation
        # -framework CoreGraphics
        # -framework CoreText (用于文字渲染)
        # -framework Foundation

        # Skia 核心库
        skia # 尝试链接 libskia.a
        skshaper # 尝试链接 libskshaper.a (如果使用文本)

        freetype # libfreetype.a
        png       # libpng.a
        z         # libz.a
        webp      # libwebp.a
        harfbuzz  # libharfbuzz.a (如果 skia_use_harfbuzz=true)
)

# 添加 macOS 的系统框架
# 这些是 Skia 在 macOS 上运行时可能需要的核心框架
find_library(CORE_FOUNDATION_LIB NAMES CoreFoundation PATHS /System/Library/Frameworks)
find_library(CORE_GRAPHICS_LIB NAMES CoreGraphics PATHS /System/Library/Frameworks)
find_library(CORE_TEXT_LIB NAMES CoreText PATHS /System/Library/Frameworks)
find_library(OPENGL_LIB NAMES OpenGL PATHS /System/Library/Frameworks)
find_library(FOUNDATION_LIB NAMES Foundation PATHS /System/Library/Frameworks)

target_link_libraries(sdl_gui PRIVATE
        ${CORE_FOUNDATION_LIB}
        ${CORE_GRAPHICS_LIB}
        ${CORE_TEXT_LIB}
        ${OPENGL_LIB}
        ${FOUNDATION_LIB}
)
