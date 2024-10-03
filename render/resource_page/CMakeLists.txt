add_library(resource_page STATIC
    resource_page.h
    resource_page.c)

target_include_directories(resource_page PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}  
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(resource_page
    pico_stdlib
    SH1106
    tool
    hardware_dma
)