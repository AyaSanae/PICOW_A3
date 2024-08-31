add_library(ui STATIC
    ui.h
    ui.c)

target_include_directories(ui PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}  
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(ui
    pico_stdlib
    SH1106
)