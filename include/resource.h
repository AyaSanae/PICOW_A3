#ifndef __resource
#define __resource

#include<stdint.h>

#define RESC_CUR  1
#define RESC_PRE  0

typedef struct 
{
    uint16_t freq;
    uint16_t freq_cur;    
    uint8_t  core_usage;
    uint8_t  tmp;
}CPU;

typedef struct 
{
    uint16_t freq;
    uint16_t freq_cur;
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

typedef struct 
{
   int_fast8_t cpu_usage_change; 
   int_fast8_t gpu_usage_change;    
   int_fast8_t vram_usage_change;    
   int_fast8_t ram_usage_change;  
}usage_change;

typedef struct{
    int_fast16_t cpu_freq_change;
    int_fast16_t gpu_freq_change;
    float vram_change;
    float ram_change;
}freq_change;
 
typedef struct 
{
    int_fast8_t cpu_tmp_change; 
    int_fast8_t gpu_tmp_change; 
}tmp_change;

typedef struct 
{
  //cpu
  uint16_t cpu_freq;
  uint16_t cpu_freq_cur;    
  uint8_t  cpu_tmp;

  //gpu
  uint16_t gpu_freq;
  uint16_t gpu_freq_cur;
  uint8_t  gpu_tmp;
  uint8_t  gpu_vram;
  float    gpu_vram_cur; 

  //ram
  uint8_t ram;
  float ram_cur;
}resc_serialize;



#endif