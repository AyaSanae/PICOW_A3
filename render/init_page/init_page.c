#include "SH1106.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "init_page.h"
#include "pico/util/queue.h"
#include "spp.h"

uint8_t draw_sin(uint16_t x){
    return (uint8_t)(10 * sin(x * M_PI / 32) + 27);
}

void Render_initPage(uint8_t *frame){
    uint8_t range_l = 0;
    uint8_t range_r = OLED_WIDTH;
    while(queue_is_empty(&queue_switch_flag)){
        OLED_initFrame(frame);
        OLED_DrawFun(frame,draw_sin,range_l,range_r);
        if(range_r != 2 * OLED_WIDTH - 1){
            range_l++;
            range_r++;
        }else{
            range_l = 0;
            range_r = OLED_WIDTH;
        }   
        char *hello ="Waiting Connect"; 
        OLED_WriteString(frame,4,48,hello);
        OLED_RenderFrame_DMA(frame);
    }
   OLED_initFrame(frame);
}
