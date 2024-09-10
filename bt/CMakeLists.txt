add_library(bt STATIC
    spp.c
    spp.h
    )

target_include_directories(bt PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}  
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(bt
    pico_stdlib
    pico_sync
    pico_util
    pico_btstack_classic
    pico_btstack_cyw43
    pico_cyw43_arch_none
)