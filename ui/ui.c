#include "ui.h"
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

    //CPU usage
    p[0] = (resce[RESC_PRE].cpu.core_usage / 10) % 10 + '0';
    p[1] = (resce[RESC_PRE].cpu.core_usage  % 10) + '0';

    //GPU usage
    p[2] = ((resce[RESC_PRE].gpu.core_usage / 10) % 10 ) + '0';
    p[3] = (resce[RESC_PRE].gpu.core_usage  % 10) + '0';

    //VRAM usage
    p[4] = ((resce[RESC_PRE].gpu.vram_usage / 10) % 10 ) + '0';
    p[5] = (resce[RESC_PRE].gpu.vram_usage  % 10) + '0';

    //RAM usage
    p[6] = ((resce[RESC_PRE].ram.ram_usage / 10) % 10 ) + '0';
    p[7] = (resce[RESC_PRE].ram.ram_usage  % 10) + '0';

    //CPU TMP
    p[8] = ((resce[RESC_PRE].cpu.tmp / 10) % 10) + '0';
    p[9] = ((resce[RESC_PRE].cpu.tmp ) % 10) + '0';

    //GPU TMP
    p[10] = ((resce[RESC_PRE].gpu.tmp / 10) % 10) + '0';
    p[11] = ((resce[RESC_PRE].gpu.tmp ) % 10) + '0';

    //CPU FREQ
    p[12] = (resce[RESC_PRE].cpu.freq_cur / 1000) % 10 + '0';
    p[13] = (resce[RESC_PRE].cpu.freq_cur / 100) % 10 + '0';
    p[14] = (resce[RESC_PRE].cpu.freq_cur / 10) % 10 + '0';
    p[15] = resce[RESC_PRE].cpu.freq_cur % 10 + '0';

    //GPU FREQ
    p[16] = (resce[RESC_PRE].gpu.freq_cur / 1000) % 10 + '0';
    p[17] = (resce[RESC_PRE].gpu.freq_cur / 100) % 10 + '0'; 
    p[18] = (resce[RESC_PRE].gpu.freq_cur / 10) % 10 + '0';
    p[19] = resce[RESC_PRE].gpu.freq_cur % 10 + '0';

    //VRAM use
    //Convert the floating-point number to an integer for easier rendering on the frame
    uint16_t vram_float2int = resce[RESC_PRE].gpu.vram_cur * 10;
    p[20] = (vram_float2int / 100) % 10 + '0';
    p[21] = (vram_float2int / 10) % 10 + '0';
    p[22] = vram_float2int % 10 + '0';

    uint16_t ram_float2int = resce[RESC_PRE].ram.ram_cur * 10;
    p[23] = (ram_float2int / 100) % 10 + '0';
    p[24] = (ram_float2int / 10) % 10 + '0';
    p[25] = ram_float2int % 10 + '0';

    return p;
}

static inline void Draw_preFrameNum(uint8_t *frame,uint8_t *p_digit,resource *resce){

    //CPU Usage
    OLED_WriteChar(frame,88,32,p_digit[0]);
    OLED_WriteChar(frame,96,32,p_digit[1]);

    //GPU Usage
    OLED_WriteChar(frame,88,41,p_digit[2]);
    OLED_WriteChar(frame,96,41,p_digit[3]);

    //VRAM Usage
    OLED_WriteChar(frame,88,48,p_digit[4]);
    OLED_WriteChar(frame,96,48,p_digit[5]);

    //RAM Usage
    OLED_WriteChar(frame,88,56,p_digit[6]);
    OLED_WriteChar(frame,96,56,p_digit[7]);

    //CPU_TMP
    OLED_WriteChar(frame,112,32,p_digit[8]);
    OLED_WriteChar(frame,120,32,p_digit[9]);

    //GPU_TMP
    OLED_WriteChar(frame,112,41,p_digit[10]);
    OLED_WriteChar(frame,120,41,p_digit[11]);

    //CPU_FREQ
    OLED_WriteChar(frame,47,32,p_digit[12]);
    OLED_WriteChar(frame,55,32,p_digit[13]);
    OLED_WriteChar(frame,62,32,p_digit[14]);
    OLED_WriteChar(frame,70,32,p_digit[15]);

    //GPU_FREQ
    OLED_WriteChar(frame,46,40,p_digit[16]);
    OLED_WriteChar(frame,54,40,p_digit[17]);
    OLED_WriteChar(frame,61,40,p_digit[18]);
    OLED_WriteChar(frame,69,40,p_digit[19]);

    //VRAM USE
    OLED_WriteChar_fix(frame,49,48,p_digit[20]);
    OLED_WriteChar_fix(frame,57,48,p_digit[21]);
    OLED_WriteChar_fix(frame,66,48,p_digit[22]);

    //RAM USE
    OLED_WriteChar_fix(frame,49,56,p_digit[23]);
    OLED_WriteChar_fix(frame,57,56,p_digit[24]);
    OLED_WriteChar_fix(frame,66,56,p_digit[25]);

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
          c = 0;
          g = 0;
          r = 0;
          vr = 0;
          t_c  = 0;
          t_g  = 0;
          ram_use_incr_margin_counter = 0;
          cpu_freq_incr_margin_counter = 0;
          gpu_freq_incr_margin_counter = 0;
          vram_use_incr_margin_counter = 0;

          //CHANGE SIGN INIT
          c_sign = (uc.cpu_usage_change >= 0) ? 1 : -1; 
          g_sign = (uc.gpu_usage_change >= 0) ? 1 : -1; 
          r_sign = (uc.ram_usage_change >= 0) ? 1 : -1; 
          vr_sign = (uc.vram_usage_change >= 0) ? 1 : -1;
          tc_sign  = (tc.cpu_tmp_change >= 0) ? 1 : -1;
          tg_sign  = (tc.gpu_tmp_change >= 0) ? 1 : -1;
          ram_use_incr_sign = (fc.ram_change >= 0)? 1 : -1; 
          vram_use_incr_sign = (fc.vram_change >= 0)? 1 : -1; 
          cpu_freq_incr_sign = (fc.cpu_freq_change >= 0) ? 1 : -1;
          gpu_freq_incr_sign = (fc.gpu_freq_change >= 0) ? 1 : -1;

          //CPU FREQ INCREASE INIT
          cpu_freq_incr_total = 0;
          cpu_freq_after_incr = resce[RESC_PRE].cpu.freq_cur;
          cpu_freq_incr_each = fc.cpu_freq_change / 100;
          cpu_freq_incr_margin = fc.cpu_freq_change % 100;

          //GPU FREQ INCREASE INIT
          gpu_freq_incr_total = 0;
          gpu_freq_after_incr = resce[RESC_PRE].gpu.freq_cur;
          gpu_freq_incr_each = fc.gpu_freq_change / 100;
          gpu_freq_incr_margin = fc.gpu_freq_change % 100;

          //VRAM USE INCREASE INIT
          vram_use_incr_total = 0;
          vram_use_after_incr = (int_fast16_t)(resce[RESC_PRE].gpu.vram_cur * 10);
          vram_use_incr_each = ((int_fast16_t)(fc.vram_change * 10)) / 100;
          vram_use_incr_margin = ((int_fast16_t)(fc.vram_change * 10)) % 100;

          //RAM USE INCREASE INIT
          ram_use_incr_total = 0;
          ram_use_after_incr = (int_fast16_t)(resce[RESC_PRE].ram.ram_cur * 10);
          ram_use_incr_each = ((int_fast16_t)(fc.ram_change * 10)) / 100;
          ram_use_incr_margin = ((int_fast16_t)(fc.ram_change * 10)) % 100;

}


static void DynamicRendering(uint8_t *frame,uint8_t render_frameNum,resource *resce,
                                        int dma_chan,tmp_change tc,freq_change fc,usage_change uc){

    for(uint16_t change = 0;change < render_frameNum;change++)
    {
       if(change == 0)
            DynamicRendering_INIT(uc,fc,tc,resce);

        c_progress  = resce[RESC_PRE].cpu.core_usage + c * c_sign;
        g_progress  = resce[RESC_PRE].gpu.core_usage + g * g_sign;
        r_progress  = resce[RESC_PRE].ram.ram_usage + r * r_sign;
        vr_progress = resce[RESC_PRE].gpu.vram_usage + vr * vr_sign;
        tc_progress = resce[RESC_PRE].cpu.tmp + t_c * tc_sign;
        tg_progress = resce[RESC_PRE].gpu.tmp + t_g * tg_sign;

        //CPU FREQ INCREASE CAL
        cpu_freq_after_incr += cpu_freq_incr_each;
        cpu_freq_incr_total =  cpu_freq_after_incr + cpu_freq_incr_margin_counter * cpu_freq_incr_sign;
        //GET CPU FREQ EACH NUM;
        cpu_freq_ones_place   = cpu_freq_incr_total % 10; 
        cpu_freq_tens_place   = cpu_freq_incr_total / 10 % 10; 
        cpu_freq_hundre_place = cpu_freq_incr_total / 100 % 10; 
        cpu_freq_thousand_place = cpu_freq_incr_total / 1000; 

        //GPU FREQ INCREASE CAL
        gpu_freq_after_incr += gpu_freq_incr_each;
        gpu_freq_incr_total =  gpu_freq_after_incr + gpu_freq_incr_margin_counter * gpu_freq_incr_sign;
        //GET GlU FREQ EACH NUM;
        gpu_freq_ones_place   = gpu_freq_incr_total % 10; 
        gpu_freq_tens_place   = gpu_freq_incr_total / 10 % 10; 
        gpu_freq_hundre_place = gpu_freq_incr_total / 100 % 10; 
        gpu_freq_thousand_place = gpu_freq_incr_total / 1000; 

        //RAM USE INCREASE CAL
        ram_use_after_incr  += ram_use_incr_each;
        ram_use_incr_total   = ram_use_after_incr + ram_use_incr_margin_counter * ram_use_incr_sign;
        ram_use_ones_place   = ram_use_incr_total % 10; 
        ram_use_tens_place   = ram_use_incr_total / 10 % 10; 
        ram_use_hundre_place = ram_use_incr_total / 100 % 10;

        //VRAM USE INCREASE CAL
        vram_use_after_incr  += vram_use_incr_each;
        vram_use_incr_total   = vram_use_after_incr + vram_use_incr_margin_counter * vram_use_incr_sign;
        vram_use_ones_place   = vram_use_incr_total % 10; 
        vram_use_tens_place   = vram_use_incr_total / 10 % 10; 
        vram_use_hundre_place = vram_use_incr_total / 100 % 10; 


        checkAndIncrement(&c, uc.cpu_usage_change);
        checkAndIncrement(&g, uc.gpu_usage_change);
        checkAndIncrement(&vr, uc.vram_usage_change);
        checkAndIncrement(&r, uc.ram_usage_change);
        checkAndIncrement(&t_c, tc.cpu_tmp_change);
        checkAndIncrement(&t_g, tc.gpu_tmp_change);
        checkAndIncrement(&cpu_freq_incr_margin_counter, cpu_freq_incr_margin);
        checkAndIncrement(&gpu_freq_incr_margin_counter, gpu_freq_incr_margin);
        checkAndIncrement(&vram_use_incr_margin_counter, vram_use_incr_margin);
        checkAndIncrement(&ram_use_incr_margin_counter,  ram_use_incr_margin);

        dma_channel_wait_for_finish_blocking(dma_chan);

        //CPU FREQ
        OLED_WriteChar(frame,70,32,cpu_freq_ones_place + '0');
        OLED_WriteChar(frame,62,32,cpu_freq_tens_place + '0');
        OLED_WriteChar(frame,54,32,cpu_freq_hundre_place + '0' );
        OLED_WriteChar(frame,46,32,cpu_freq_thousand_place + '0');

        //GPU_FREQ
        OLED_WriteChar(frame,70,40,gpu_freq_ones_place + '0');
        OLED_WriteChar(frame,62,40,gpu_freq_tens_place + '0');
        OLED_WriteChar(frame,54,40,gpu_freq_hundre_place + '0' );
        OLED_WriteChar(frame,46,40,gpu_freq_thousand_place + '0');

        //RAM USE
        OLED_WriteChar_fix(frame,49,56,ram_use_hundre_place + '0');
        OLED_WriteChar_fix(frame,57,56,ram_use_tens_place + '0');
        OLED_WriteChar_fix(frame,66,56,ram_use_ones_place + '0');

        //VRAM USE
        OLED_WriteChar_fix(frame,49,48,vram_use_hundre_place + '0');
        OLED_WriteChar_fix(frame,57,48,vram_use_tens_place + '0');
        OLED_WriteChar_fix(frame,66,48,vram_use_ones_place + '0');

        // Draw RAM,VRAM USE DECIMAL POINT
        Draw_decimal_point(frame);

        // CPU
        Draw_ProgressBar(frame, 27, 0, OLED_WIDTH, 0, c_progress);
        OLED_WriteChar(frame, 88, 32, ((c_progress / 10) % 10) + '0');
        OLED_WriteChar(frame, 96, 32, (c_progress % 10) + '0');

        // GPU
        Draw_ProgressBar(frame, 27, 8, OLED_WIDTH, 8, g_progress);
        OLED_WriteChar(frame, 88, 41, ((g_progress / 10) % 10) + '0');
        OLED_WriteChar(frame, 96, 41, (g_progress % 10) + '0');

        // VRAM
        OLED_WriteChar(frame, 88, 48, ((vr_progress / 10) % 10) + '0');
        OLED_WriteChar(frame, 96, 48, (vr_progress % 10) + '0');

        // RAM
        Draw_ProgressBar(frame, 27, 16, OLED_WIDTH, 16, r_progress);
        OLED_WriteChar(frame, 88, 56, ((r_progress / 10) % 10) + '0');
        OLED_WriteChar(frame, 96, 56, (r_progress % 10) + '0');

        // CPU_TMP
        OLED_WriteChar(frame, 112, 32, ((tc_progress / 10) % 10) + '0');
        OLED_WriteChar(frame, 120, 32, (tc_progress % 10) + '0');

        // GPU_TMP
        OLED_WriteChar(frame, 112, 41, ((tg_progress / 10) % 10) + '0');
        OLED_WriteChar(frame, 120, 41, (tg_progress % 10) + '0');

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