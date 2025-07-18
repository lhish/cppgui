set(LIB sdl_gui_common)

aux_source_directory(src SRC)
add_library(${LIB} SHARED ${SRC})

find_library(CORE_FOUNDATION_LIB NAMES CoreFoundation PATHS /System/Library/Frameworks)
find_library(CORE_GRAPHICS_LIB NAMES CoreGraphics PATHS /System/Library/Frameworks)
find_library(CORE_TEXT_LIB NAMES CoreText PATHS /System/Library/Frameworks)
find_library(OPENGL_LIB NAMES OpenGL PATHS /System/Library/Frameworks)
find_library(FOUNDATION_LIB NAMES Foundation PATHS /System/Library/Frameworks)

target_include_directories(
  ${LIB}
  PUBLIC include
)

target_link_libraries(
  ${LIB}
        PUBLIC
        skia
        skshaper
        freetype
        png
        z
        webp
        harfbuzz
        ${CORE_FOUNDATION_LIB}
        ${CORE_GRAPHICS_LIB}
        ${CORE_TEXT_LIB}
        ${OPENGL_LIB}
        ${FOUNDATION_LIB}
        SDL3_ttf::SDL3_ttf SDL3::SDL3
)
