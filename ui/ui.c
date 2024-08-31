#include "ui.h"
#include <string.h>
#include "pico/stdlib.h"
#include "SH1106.h" 
#include "resource.h"

void Render_ProgressBar(uint8_t *frame,uint8_t x,uint8_t y,uint8_t x1,uint8_t y1,uint8_t progress){
    assert(progress <= 100);

        uint16_t start = (y / 8) * OLED_WIDTH + x;
        uint16_t end = (y1 / 8) * OLED_WIDTH + x1;
        uint16_t bar = end - start;
        uint8_t p = (progress * bar) / 100;
        memset(frame + start, 0xff, p);
}

void Render_ProgressNum(uint8_t *frame,uint8_t x,uint8_t y,uint8_t progress){
    assert(progress <= 100);
    OLED_WriteChar(frame,x,y,progress); 
}

char *CPU_text[] = {"CPU "};
char *GPU_text[] = {"GPU "};
char *RAM_text[] = {"RAM "};

void Render_ResPage(uint8_t *frame,resource resce){ 
    OLED_WriteString(frame,0,0,CPU_text[0]);
    OLED_WriteString(frame,0,8,GPU_text[0]);
    OLED_WriteString(frame,0,16,RAM_text[0]);

    OLED_RenderFrame_DMA_Clear(frame);
}














