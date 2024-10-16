add_library(init_page STATIC
    init_page.h
    init_page.c)

target_include_directories(init_page PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}  
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(init_page
    pico_sync
    pico_util
    pico_stdlib
    SH1106
    tool
    bt
    hardware_dma
)
