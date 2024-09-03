#ifndef __tool
#define __tool

#include <stdint.h>
#include "resource.h"

#define DMA_CHAN_UNSET  100

int frame_copy_dma(uint8_t *d_frame,uint8_t *s_frame,int dma_chan);
static inline int max_abs(int x, int y);

uint8_t getResourceChangeMax_abs(usage_change uc,tmp_change tc);

#endif