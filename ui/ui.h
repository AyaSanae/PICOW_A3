#ifndef __ui
#define __ui

#include <stdint.h>
#include "SH1106.h" 
#include "resource.h"

//counter--Smooth transitions for displaying various values.
static int_fast16_t c;   
static int_fast16_t g;
static int_fast16_t r;
static int_fast16_t vr;
static int_fast16_t t_c; // CPU temperature
static int_fast16_t t_g; // GPU temperature

static int_fast16_t c_progress;
static int_fast16_t g_progress;
static int_fast16_t r_progress;
static int_fast16_t vr_progress;
static int_fast16_t tc_progress;
static int_fast16_t tg_progress;

static int_fast8_t c_sign;
static int_fast8_t g_sign;
static int_fast8_t r_sign;
static int_fast8_t vr_sign;
static int_fast8_t tc_sign;
static int_fast8_t tg_sign;

static inline void Draw_ProgressBar(uint8_t *frame, uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, int8_t progress);
static inline void Draw_preFrameNum(uint8_t *frame, uint8_t *p_digit, resource *resce);
static inline void Draw_preFrameBar(uint8_t *frame,resource *resce);

static inline void DynamicRendering(uint8_t *frame,uint8_t render_frameNum,resource *resce,
                                        int dma_chan,tmp_change tc,freq_change fc,usage_change uc);
static inline void Cal_resUsage(resource *resce,uint8_t time);

static inline uint8_t*     Cal_digit_mod(resource *resce);
static inline tmp_change   Cal_resTmpChange(resource *resce);
static inline freq_change  Cal_resFreqChange(resource *resce);
static inline usage_change Cal_resUsageChange(resource *resce);



void Render_ResPage(uint8_t *frame,resource *resce);


#endif