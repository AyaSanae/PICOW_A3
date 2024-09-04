#ifndef __ui
#define __ui

#include <stdint.h>
#include "SH1106.h" 
#include "resource.h"

//counter--Smooth transitions for displaying various values.
static int_fast16_t cpu_usage_incr_counter;   
static int_fast16_t gpu_usage_incr_counter;
static int_fast16_t ram_usage_incr_counter;
static int_fast16_t vram_usage_incr_counter;
static int_fast16_t cpu_tmp_incr_counter; // CPU temperature
static int_fast16_t gpu_tmp_incr_counter; // GPU temperature

//FOR CPU FREQ
static int_fast16_t cpu_freq_incr_each;
static int_fast16_t cpu_freq_incr_margin; 
static int_fast16_t cpu_freq_after_incr;
static int_fast16_t cpu_freq_after_incr_margin;
static int_fast16_t cpu_freq_incr_total;
static int_fast16_t cpu_freq_incr_margin_counter;  //each increse 1 to deal with margin
static int_fast16_t cpu_freq_ones_place;
static int_fast16_t cpu_freq_tens_place;   
static int_fast16_t cpu_freq_hundre_place; 
static int_fast16_t cpu_freq_thousand_place;

//FOR GPU FREQ
static int_fast16_t gpu_freq_incr_each;
static int_fast16_t gpu_freq_incr_margin; 
static int_fast16_t gpu_freq_after_incr;
static int_fast16_t gpu_freq_after_incr_margin;
static int_fast16_t gpu_freq_incr_total;
static int_fast16_t gpu_freq_incr_margin_counter;  
static int_fast16_t gpu_freq_ones_place;
static int_fast16_t gpu_freq_tens_place;   
static int_fast16_t gpu_freq_hundre_place; 
static int_fast16_t gpu_freq_thousand_place;

//FOR VRAM USE
static int_fast16_t vram_use_incr_each;
static int_fast16_t vram_use_incr_margin; 
static int_fast16_t vram_use_after_incr;
static int_fast16_t vram_use_after_incr_margin;
static int_fast16_t vram_use_incr_total;
static int_fast16_t vram_use_incr_margin_counter;
static int_fast16_t vram_use_ones_place;
static int_fast16_t vram_use_tens_place;   
static int_fast16_t vram_use_hundre_place; 

//FOR RAM USE
static int_fast16_t ram_use_incr_each;
static int_fast16_t ram_use_incr_margin; 
static int_fast16_t ram_use_after_incr;
static int_fast16_t ram_use_after_incr_margin;
static int_fast16_t ram_use_incr_total;
static int_fast16_t ram_use_incr_margin_counter;
static int_fast16_t ram_use_ones_place;
static int_fast16_t ram_use_tens_place;   
static int_fast16_t ram_use_hundre_place; 

static int_fast16_t cpu_usage_progress;
static int_fast16_t gpu_usage_progress;
static int_fast16_t ram_usage_progress;

static int_fast16_t vram_usage_progress;
static int_fast16_t cpu_tmp_progress;
static int_fast16_t gpu_tmp_progress;

static inline void Draw_ProgressBar(uint8_t *frame, uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, int8_t progress);
static inline void Draw_preFrameNum(uint8_t *frame, uint8_t *p_digit, resource *resce);
static inline void Draw_preFrameBar(uint8_t *frame,resource *resce);
static inline void Draw_decimal_point(uint8_t * frame);


static inline void DynamicRendering_INIT(usage_change uc,freq_change fc,tmp_change tc,resource *resce);
static inline void DynamicRendering(uint8_t *frame,uint8_t render_frameNum,resource *resce,
                                        int dma_chan,tmp_change tc,freq_change fc,usage_change uc);

static inline void Cal_resUsage(resource *resce,uint8_t time);

static inline uint8_t*     Cal_digit_mod(resource *resce);
static inline tmp_change   Cal_resTmpChange(resource *resce);
static inline freq_change  Cal_resFreqChange(resource *resce);
static inline usage_change Cal_resUsageChange(resource *resce);


void Render_ResPage(uint8_t *frame,resource *resce);


#endif