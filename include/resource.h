#ifndef __resource
#define __resource

#include<stdint.h>

typedef struct 
{
    uint16_t freq;
    uint16_t freq_cur;    
    uint8_t  core_usage;
    uint8_t  tmp;
}CPU;

typedef struct 
{
    uint8_t freq;
    uint8_t freq_cur;
    uint8_t core_usage;
    uint8_t tmp;
    uint8_t vram;
    float vram_cur;
    uint8_t  vram_usage;
}GPU;

typedef struct 
{
    uint8_t ram;
    float ram_cur;
    uint8_t ram_usage;
}RAM;

typedef struct 
{
    CPU cpu;
    GPU gpu;
    RAM ram;
}resource;

#endif