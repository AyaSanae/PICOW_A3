#include <stdint.h>
#include "pico/util/queue.h"

#define RESC_QUEUE_SIZE 24

enum DATA_TYPE{
    RESOURCE_MASG_PACK = 0x00,
};

extern queue_t queue_RescMasgPack_Handler;

//init_page switch to resource_page
extern queue_t queue_switch_flag;

int btstack_main(int argc, const char * argv[]);

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
