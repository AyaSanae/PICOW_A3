#ifndef __tool
#define __tool

#include <stdint.h>

#define DMA_CHAN_UNSET  100

int frame_copy_dma(uint8_t *d_frame,uint8_t *s_frame,int dma_chan);
int max(int x, int y);

#endif