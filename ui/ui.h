#ifndef __ui
#define __ui

#include <stdint.h>
#include "SH1106.h" 
#include "resource.h"

void Render_ProgressBar(uint8_t *frame,uint8_t x,uint8_t y,uint8_t x1,uint8_t y1,uint8_t progress);
void Render_ProgressNum(uint8_t *frame,uint8_t x,uint8_t y,uint8_t progress);

void Render_ResPage(uint8_t *frame,resource resce);


#endif