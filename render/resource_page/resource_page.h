#ifndef __resource_page__
#define __resource_page__

#include <stdint.h>
#include "SH1106.h" 
#include "resource.h"

typedef struct
{
   int_fast16_t ones_place;
   int_fast16_t tens_place;
   int_fast16_t hundre_place; 
   int_fast16_t thousand_place;
}DecimalNum_Components;

typedef struct{
    int_fast16_t change_each;
    int_fast16_t change_margin;
    int_fast16_t change_margin_limiter;
    int_fast16_t after_change;
    int_fast16_t change_total;
}FreqChangeCounter;    //Smooth transitions for displaying various values

//counter--Smooth transitions for displaying various values.
static int_fast16_t cpu_usage_incr_counter;   
static int_fast16_t gpu_usage_incr_counter;
static int_fast16_t ram_usage_incr_counter;
static int_fast16_t vram_usage_incr_counter;
static int_fast16_t cpu_tmp_incr_counter; // CPU temperature
static int_fast16_t gpu_tmp_incr_counter; // GPU temperature

//FOR CPU FREQ
static FreqChangeCounter cpu_freq_FCC;
static DecimalNum_Components cpu_freq_DNC;


//FOR GPU FREQ
static FreqChangeCounter gpu_freq_FCC;
static DecimalNum_Components gpu_freq_DNC;


//FOR VRAM USE
static FreqChangeCounter vram_use_FCC;
static DecimalNum_Components vram_use_DNC;

//FOR RAM USE
static FreqChangeCounter ram_use_FCC;
static DecimalNum_Components ram_use_DNC;

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

static inline void         Cal_frqe_change_each_time(FreqChangeCounter *FCC,DecimalNum_Components *DNC,float freq_change);

void Render_ResPage(uint8_t *frame,resource *resce);


#endif