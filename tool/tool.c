#include "tool.h"
#include "hardware/dma.h"
#include "SH1106.h"
#include "resource.h"
#include <stdlib.h>

int frame_copy_dma(uint8_t *d_frame,uint8_t *s_frame,int dma_chan){
    int chan = dma_chan;

    if(chan == 100){
       chan = dma_claim_unused_channel(1);
    }
    dma_channel_config c = dma_channel_get_default_config(chan);
    channel_config_set_transfer_data_size(&c,DMA_SIZE_8);
    channel_config_set_read_increment(&c,true);
    channel_config_set_write_increment(&c,true);

    dma_channel_configure(
        chan,
        &c,
        d_frame,
        s_frame,
        OLED_SIZE_BYTE, 
        true
    );

    return chan;
}


static inline int max_abs(int x, int y){
    int x_abs = abs(x); 
    int y_abs = abs(y); 
    return (x > y) ? x : y;
}

uint16_t getResourceChangeMax_abs(usage_change uc,freq_change fc,tmp_change tc){
    uint16_t uc_max = max_abs(max_abs(max_abs(uc.cpu_usage_change,uc.gpu_usage_change),uc.ram_usage_change),uc.vram_usage_change);
    uint16_t fc_max = max_abs(max_abs(max_abs(fc.cpu_freq_change,fc.gpu_freq_change),fc.ram_change),fc.vram_change);
    uint16_t tc_max = max_abs(tc.cpu_tmp_change,tc.gpu_tmp_change);
    
    return max_abs(uc_max,max_abs(fc_max,tc_max));
}