#include "resource_page.h"
#include <string.h>
#include "pico/stdlib.h"
#include "SH1106.h" 
#include "resource.h"
#include "tool.h"
#include <stdlib.h>
#include "frame_template.h"
#include "hardware/dma.h"

#include <stdio.h>

static inline void Draw_ProgressBar(uint8_t *frame,uint8_t x,uint8_t y,uint8_t x1,uint8_t y1,int8_t progress){
    assert(progress <= 100 && progress >= 0);

        uint16_t start = (y / 8) * OLED_WIDTH + x;
        uint16_t end = (y1 / 8) * OLED_WIDTH + x1;
        uint16_t bar = end - start;
        uint8_t p = (progress * bar) / 100;
        memset(frame + start, 0xff, p);
}

static inline void Draw_decimal_point(uint8_t * frame){
    OLED_setPixel(frame,64,55,1);
    OLED_setPixel(frame,64,63,1);
}

static inline void Cal_resUsage(resource *resce,uint8_t time){
    resce[time].cpu.core_usage =  ((float)resce[time].cpu.freq_cur / resce[time].cpu.freq) * 100; 
    resce[time].gpu.core_usage =  ((float)resce[time].gpu.freq_cur / resce[time].gpu.freq) * 100; 
    resce[time].gpu.vram_usage =  (resce[time].gpu.vram_cur / resce[time].gpu.vram) * 100; 
    resce[time].ram.ram_usage  =  (resce[time].ram.ram_cur  / resce[time].ram.ram)  * 100;
}
static inline usage_change Cal_resUsageChange(resource *resce){
    usage_change uc;

    uc.cpu_usage_change   = (int16_t)resce[RESC_CUR].cpu.core_usage - resce[RESC_PRE].cpu.core_usage;
    uc.gpu_usage_change   = (int16_t)resce[RESC_CUR].gpu.core_usage - resce[RESC_PRE].gpu.core_usage;
    uc.vram_usage_change  = (int16_t)resce[RESC_CUR].gpu.vram_usage - resce[RESC_PRE].gpu.vram_usage;
    uc.ram_usage_change   = (int16_t)resce[RESC_CUR].ram.ram_usage  - resce[RESC_PRE].ram.ram_usage;

    return uc;
}

static inline freq_change Cal_resFreqChange(resource *resce){
    freq_change fc;
    fc.cpu_freq_change = (int16_t)resce[RESC_CUR].cpu.freq_cur - resce[RESC_PRE].cpu.freq_cur; 
    fc.gpu_freq_change = (int16_t)resce[RESC_CUR].gpu.freq_cur - resce[RESC_PRE].gpu.freq_cur; 
    fc.ram_change      = resce[RESC_CUR].ram.ram_cur  - resce[RESC_PRE].ram.ram_cur; 
    fc.vram_change     = resce[RESC_CUR].gpu.vram_cur  - resce[RESC_PRE].gpu.vram_cur; 

    return fc;
}

static inline tmp_change Cal_resTmpChange(resource *resce){
    tmp_change tp;
    tp.cpu_tmp_change = (int16_t)resce[RESC_CUR].cpu.tmp - resce[RESC_PRE].cpu.tmp;
    tp.gpu_tmp_change = (int16_t)resce[RESC_CUR].gpu.tmp - resce[RESC_PRE].gpu.tmp;

    return tp;
}

static inline uint8_t* Cal_digit_mod(resource *resce){
    uint8_t *p = malloc(26 * sizeof(uint8_t));
    assert(*p != NULL);
    struct
    {
        uint16_t value;
        size_t start_index;
        size_t num_digits;
    } values[] = {
        {resce[RESC_PRE].cpu.core_usage, 0, 2},                 // CPU usage
        {resce[RESC_PRE].gpu.core_usage, 2, 2},                 // GPU usage
        {resce[RESC_PRE].gpu.vram_usage, 4, 2},                 // VRAM usage
        {resce[RESC_PRE].ram.ram_usage, 6, 2},                  // RAM usage
        {resce[RESC_PRE].cpu.tmp, 8, 2},                        // CPU TMP
        {resce[RESC_PRE].gpu.tmp, 10, 2},                       // GPU TMP
        {resce[RESC_PRE].cpu.freq_cur, 12, 4},                  // CPU FREQ
        {resce[RESC_PRE].gpu.freq_cur, 16, 4},                  // GPU FREQ
        {(uint16_t)(resce[RESC_PRE].gpu.vram_cur * 10), 20, 3}, // VRAM use
        {(uint16_t)(resce[RESC_PRE].ram.ram_cur * 10), 23, 3}   // RAM use
    };

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    {
        for (size_t j = 0; j < values[i].num_digits; ++j)
        {
            p[values[i].start_index + j] = (values[i].value / (1 << (j * 4))) % 10 + '0'; // 每位的计算
        }
    }

    return p;
}

static inline void Draw_preFrameNum(uint8_t *frame,uint8_t *p_digit,resource *resce){
    uint8_t positions[][2] = {
        {88, 32}, {96, 32}, // CPU Usage
        {88, 41}, {96, 41}, // GPU Usage
        {88, 48}, {96, 48}, // VRAM Usage
        {88, 56}, {96, 56}, // RAM Usage
        {112, 32}, {120, 32}, // CPU_TMP
        {112, 41}, {120, 41}, // GPU_TMP
        {47, 32}, {55, 32}, {62, 32}, {70, 32}, // CPU_FREQ
        {46, 40}, {54, 40}, {61, 40}, {69, 40}, // GPU_FREQ
        {49, 48}, {57, 48}, {66, 48}, // VRAM USE
        {49, 56}, {57, 56}, {66, 56}  // RAM USE
    };

    for (size_t i = 0; i < sizeof(positions) / sizeof(positions[0]); ++i) {
        OLED_WriteChar_fix(frame, positions[i][0], positions[i][1], p_digit[i]);
    }

    Draw_decimal_point(frame);

    free(p_digit);
}

static inline void Draw_preFrameBar(uint8_t *frame,resource *resce){
    //CPU Usage Progress Bar
    Draw_ProgressBar(frame,27,0,OLED_WIDTH,0,resce[RESC_PRE].cpu.core_usage); 
    //GPU Usage Progress Bar
    Draw_ProgressBar(frame,27,8,OLED_WIDTH,8,resce[RESC_PRE].gpu.core_usage);
    //RAM Usage Progress Bar
    Draw_ProgressBar(frame,27,16, OLED_WIDTH, 16, resce[RESC_PRE].ram.ram_usage);
}

static inline void checkAndIncrement(int *counter, int change){
    if (*counter < abs(change) && change) {
        (*counter)++;
    }
}

static inline void DynamicRendering_INIT(usage_change uc,freq_change fc,tmp_change tc,resource *resce){
          //COUNTER INIT
          cpu_usage_incr_counter = 0;
          gpu_usage_incr_counter = 0;
          ram_usage_incr_counter = 0;
          vram_usage_incr_counter = 0;
          cpu_tmp_incr_counter    = 0;
          gpu_tmp_incr_counter    = 0;

          //FreqChangeCounter INIT
          ram_use_FCC.change_margin_limiter = 0;
          cpu_freq_FCC.change_margin_limiter = 0;
          gpu_freq_FCC.change_margin_limiter = 0;
          vram_use_FCC.change_margin_limiter = 0;

          //CPU FREQ INCREASE INIT
          cpu_freq_FCC.change_total  = 0;
          cpu_freq_FCC.after_change  = resce[RESC_PRE].cpu.freq_cur;
          cpu_freq_FCC.change_each   = fc.cpu_freq_change / 100;
          cpu_freq_FCC.change_margin = fc.cpu_freq_change % 100;


          //GPU FREQ INCREASE INIT
          gpu_freq_FCC.change_total  = 0;
          gpu_freq_FCC.after_change  = resce[RESC_PRE].gpu.freq_cur;
          gpu_freq_FCC.change_each   = fc.gpu_freq_change / 100;
          gpu_freq_FCC.change_margin = fc.gpu_freq_change % 100;

          //VRAM USE INCREASE INIT
          vram_use_FCC.change_total  = 0;
          vram_use_FCC.after_change  = (int_fast16_t)(resce[RESC_PRE].gpu.vram_cur * 10);
          vram_use_FCC.change_each   = ((int_fast16_t)(fc.vram_change * 10)) / 100;
          vram_use_FCC.change_margin = ((int_fast16_t)(fc.vram_change * 10)) % 100;

          //RAM USE INCREASE INIT
          ram_use_FCC.change_total  = 0;
          ram_use_FCC.after_change  = (int_fast16_t)(resce[RESC_PRE].ram.ram_cur * 10);
          ram_use_FCC.change_each   = ((int_fast16_t)(fc.ram_change * 10)) / 100;
          ram_use_FCC.change_margin = ((int_fast16_t)(fc.ram_change * 10)) % 100;

}

static inline void Cal_frqe_change_each_time(FreqChangeCounter *FCC,DecimalNum_Components *DNC,float freq_change){
    int_fast8_t sign = freq_change >= 0 ? 1 : -1;

    FCC->after_change += FCC->change_each;
    FCC->change_total = FCC->after_change + FCC->change_margin_limiter * sign;

    DNC->ones_place     = FCC->change_total % 10; 
    DNC->tens_place     = FCC->change_total / 10 % 10; 
    DNC->hundre_place   = FCC->change_total / 100 % 10; 
    DNC->thousand_place = FCC->change_total / 1000;
}

static void DynamicRendering(uint8_t *frame,uint8_t render_frameNum,resource *resce,
                                        int dma_chan,tmp_change tc,freq_change fc,usage_change uc){

    for(uint16_t change = 0;change < render_frameNum;change++)
    {
       if(change == 0)
            DynamicRendering_INIT(uc,fc,tc,resce);

        //like ((uc.cpu_usage_change >= 0) ? 1 : -1) 
        //it use to get the change sign
        cpu_usage_progress  = resce[RESC_PRE].cpu.core_usage + cpu_usage_incr_counter * ((uc.cpu_usage_change >= 0) ? 1 : -1);
        gpu_usage_progress  = resce[RESC_PRE].gpu.core_usage + gpu_usage_incr_counter  * ((uc.gpu_usage_change >= 0) ? 1 : -1);
        ram_usage_progress  = resce[RESC_PRE].ram.ram_usage + ram_usage_incr_counter * ((uc.ram_usage_change >= 0) ? 1 : -1);
        vram_usage_progress = resce[RESC_PRE].gpu.vram_usage + vram_usage_incr_counter * ((uc.vram_usage_change >= 0) ? 1 : -1);
        cpu_tmp_progress    = resce[RESC_PRE].cpu.tmp + cpu_tmp_incr_counter * ((tc.cpu_tmp_change >= 0) ? 1 : -1);
        gpu_tmp_progress    = resce[RESC_PRE].gpu.tmp + gpu_tmp_incr_counter * ((tc.gpu_tmp_change >= 0) ? 1 : -1);

        Cal_frqe_change_each_time(&cpu_freq_FCC,&cpu_freq_DNC,fc.cpu_freq_change);
        Cal_frqe_change_each_time(&gpu_freq_FCC,&gpu_freq_DNC,fc.gpu_freq_change);
        Cal_frqe_change_each_time(&ram_use_FCC,&ram_use_DNC,fc.ram_change);
        Cal_frqe_change_each_time(&vram_use_FCC,&vram_use_DNC,fc.vram_change);
        

        checkAndIncrement(&cpu_usage_incr_counter, uc.cpu_usage_change);
        checkAndIncrement(&gpu_usage_incr_counter, uc.gpu_usage_change);
        checkAndIncrement(&vram_usage_incr_counter, uc.vram_usage_change);
        checkAndIncrement(&ram_usage_incr_counter, uc.ram_usage_change);
        checkAndIncrement(&cpu_tmp_incr_counter, tc.cpu_tmp_change);
        checkAndIncrement(&gpu_tmp_incr_counter , tc.gpu_tmp_change);

        checkAndIncrement(&ram_use_FCC.change_margin_limiter, ram_use_FCC.change_margin);
        checkAndIncrement(&cpu_freq_FCC.change_margin_limiter, cpu_freq_FCC.change_margin);
        checkAndIncrement(&gpu_freq_FCC.change_margin_limiter, gpu_freq_FCC.change_margin);
        checkAndIncrement(&vram_use_FCC.change_margin_limiter, vram_use_FCC.change_margin);

        dma_channel_wait_for_finish_blocking(dma_chan);

        //CPU FREQ
        OLED_WriteChar_fix(frame,72,32,cpu_freq_DNC.ones_place     + '0');
        OLED_WriteChar_fix(frame,64,32,cpu_freq_DNC.tens_place     + '0');
        OLED_WriteChar_fix(frame,56,32,cpu_freq_DNC.hundre_place   + '0');
        OLED_WriteChar_fix(frame,48,32,cpu_freq_DNC.thousand_place + '0');

        //GPU_FREQ
        OLED_WriteChar_fix(frame,72,40,gpu_freq_DNC.ones_place     + '0');
        OLED_WriteChar_fix(frame,64,40,gpu_freq_DNC.tens_place     + '0');
        OLED_WriteChar_fix(frame,56,40,gpu_freq_DNC.hundre_place   + '0');
        OLED_WriteChar_fix(frame,48,40,gpu_freq_DNC.thousand_place + '0');

        //RAM USE
        OLED_WriteChar_fix(frame,66,56,ram_use_DNC.ones_place + '0');
        OLED_WriteChar_fix(frame,57,56,ram_use_DNC.tens_place   + '0');
        OLED_WriteChar_fix(frame,49,56,ram_use_DNC.hundre_place   + '0');

        //VRAM USE
        OLED_WriteChar_fix(frame,66,48,vram_use_DNC.ones_place + '0');
        OLED_WriteChar_fix(frame,57,48,vram_use_DNC.tens_place   + '0');
        OLED_WriteChar_fix(frame,49,48,vram_use_DNC.hundre_place   + '0');

        // Draw RAM,VRAM USE DECIMAL POINT
        Draw_decimal_point(frame);

        // CPU
        Draw_ProgressBar(frame, 27, 0, OLED_WIDTH, 0, cpu_usage_progress);
        OLED_WriteChar_fix(frame, 88, 32, ((cpu_usage_progress / 10) % 10) + '0');
        OLED_WriteChar_fix(frame, 96, 32, (cpu_usage_progress  % 10) + '0');

        // GPU
        Draw_ProgressBar(frame, 27, 8, OLED_WIDTH, 8, gpu_usage_progress);
        OLED_WriteChar_fix(frame, 88, 41, ((gpu_usage_progress / 10) % 10) + '0');
        OLED_WriteChar_fix(frame, 96, 41, (gpu_usage_progress  % 10) + '0');

        // VRAM
        OLED_WriteChar_fix(frame, 88, 48, ((vram_usage_progress / 10) % 10) + '0');
        OLED_WriteChar_fix(frame, 96, 48, (vram_usage_progress  % 10) + '0');

        // RAM
        Draw_ProgressBar(frame, 27, 16, OLED_WIDTH, 16, ram_usage_progress );
        OLED_WriteChar_fix(frame, 88, 56, ((ram_usage_progress  / 10) % 10) + '0');
        OLED_WriteChar_fix(frame, 96, 56, (ram_usage_progress   % 10) + '0');

        // CPU_TMP
        OLED_WriteChar_fix(frame, 112, 32, ((cpu_tmp_progress / 10) % 10) + '0');
        OLED_WriteChar_fix(frame, 120, 32, (cpu_tmp_progress  % 10) + '0');

        // GPU_TMP
        OLED_WriteChar_fix(frame, 112, 41, ((gpu_tmp_progress / 10) % 10) + '0');
        OLED_WriteChar_fix(frame, 120, 41, (gpu_tmp_progress  % 10) + '0');

        OLED_RenderFrame_DMA(frame);
        frame_copy_dma(frame, frame_template, dma_chan);
    }
}

void Render_ResPage(uint8_t *frame,resource *resce){

    Cal_resUsage(resce,RESC_CUR);
    Cal_resUsage(resce,RESC_PRE);

    tmp_change   tc = Cal_resTmpChange(resce); 
    freq_change  fc = Cal_resFreqChange(resce); 
    usage_change uc = Cal_resUsageChange(resce);

    int dma_chan = frame_copy_dma(frame,frame_template,DMA_CHAN_UNSET);

    uint8_t *p_digit = Cal_digit_mod(resce);

    dma_channel_wait_for_finish_blocking(dma_chan);

    Draw_preFrameBar(frame,resce);
    Draw_preFrameNum(frame,p_digit,resce);

    OLED_RenderFrame_DMA(frame);

    uint8_t render_frameNum = 100;
    DynamicRendering(frame,render_frameNum,resce,dma_chan,tc,fc,uc);

    dma_channel_wait_for_finish_blocking(dma_chan);
    dma_channel_cleanup(dma_chan);
    dma_channel_unclaim(dma_chan);
}
