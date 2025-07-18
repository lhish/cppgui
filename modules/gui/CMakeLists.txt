set(LIB sdl_gui_gui)

aux_source_directory(src SRC)
add_library(${LIB} SHARED ${SRC})

target_include_directories(
        ${LIB}
        PUBLIC include
)

target_link_libraries(
        ${LIB}
        sdl_gui_common
)
