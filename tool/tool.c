#include "tool.h"
#include "hardware/dma.h"
#include "SH1106.h"

void frame_copy_dma(uint8_t *d_frame,uint8_t *s_frame){
    int dma_chan = dma_claim_unused_channel(1);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c,DMA_SIZE_8);
    channel_config_set_read_increment(&c,true);
    channel_config_set_write_increment(&c,true);

    dma_channel_configure(
        dma_chan,
        &c,
        d_frame,
        s_frame,
        OLED_SIZE_BYTE, 
        true
    );
}