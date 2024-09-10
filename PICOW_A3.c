#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include <stdio.h>
#include <stdlib.h>
#include "ui.h" 
#include <math.h>
#include "resource.h"
#include "frame_template.h"

#include "pico/cyw43_arch.h"
#include "btstack_run_loop.h"

#include "pico/util/queue.h"
#include "spp.h"

resource resc[2];

void btstack_run_core1(){
  btstack_main(0, NULL);
  btstack_run_loop_execute();
}

void resc_serialize_decode(uint8_t *data,resc_serialize *resc_s){
  memcpy(resc_s,data,RESC_QUEUE_SIZE);
}

//TEST
//void show_byte(resc_serialize *resc){
//  uint8_t *p = resc;
//  for(int i = 0; i < sizeof(resc_serialize); i++){
//    printf("0x%.2x ",*(p + i));
//  }
//}
//resc_serialize resc_serialize_decode(uint8_t *data){
//    resc_serialize resc_s;
//    memcpy(&resc_s,data,RESC_QUEUE_SIZE);
//
//    return resc_s;
//}

int main()
{
  stdio_init_all();
  cyw43_arch_init();
  multicore_launch_core1(btstack_run_core1);

  i2c_init(i2c0, OLED_BAUD);
  gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
  gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_SCL);
  gpio_pull_up(PIN_SDA);
  sleep_ms(100);
  OLED_Init();
  OLED_Clear();
  OLED_initFrame(frameBuff[0].frame);

  uint8_t resc_data[RESC_QUEUE_SIZE];
  resc_serialize resc_cur;
  uint8_t first_flag = 0;
  while (1)
  {
    if (queue_is_full(&queue_RescMasgPack_Handler))
    {
      for (int i = 0; i < RESC_QUEUE_SIZE; i++)
      {
        queue_remove_blocking(&queue_RescMasgPack_Handler, &resc_data[i]);
      }
      resc_serialize_decode(resc_data,&resc_cur);
      resc[RESC_PRE] = resc[RESC_CUR];
      resc[RESC_CUR].cpu.freq = resc_cur.cpu_freq;
      resc[RESC_CUR].cpu.tmp  = resc_cur.cpu_tmp;
      resc[RESC_CUR].cpu.freq_cur = resc_cur.cpu_freq_cur;

      resc[RESC_CUR].gpu.freq = resc_cur.gpu_freq;
      resc[RESC_CUR].gpu.tmp  = resc_cur.gpu_tmp;
      resc[RESC_CUR].gpu.vram = resc_cur.gpu_vram;
      resc[RESC_CUR].gpu.vram_cur = resc_cur.gpu_vram_cur;
      resc[RESC_CUR].gpu.freq_cur = resc_cur.gpu_freq_cur;

      resc[RESC_CUR].ram.ram  = resc_cur.ram;
      resc[RESC_CUR].ram.ram_cur  = resc_cur.ram_cur;

      if(!first_flag){
        resc[RESC_PRE] = resc[RESC_CUR];
        first_flag = 1;
      }

      Render_ResPage(frameBuff[0].frame,resc);
    }
  }

/***************TEST************************

  resc[0].cpu.freq     = 2000;
  resc[0].cpu.freq_cur = 400;
  resc[0].cpu.tmp      =50;

  resc[0].gpu.freq        = 1000;  
  resc[0].gpu.freq_cur    = 400;  
  resc[0].gpu.tmp         = 65;
  resc[0].gpu.vram        = 5;
  resc[0].gpu.vram_cur    = 4.3;

  resc[0].ram.ram         = 16;
  resc[0].ram.ram_cur     = 7.5;

  resc[1] = resc[0];

  Render_ResPage(frameBuff[0].frame,resc);
  sleep_ms(9 * 1000);
  resc[1].cpu.freq_cur = 1500;
  resc[1].gpu.freq_cur = 200;
  resc[1].ram.ram_cur  = 9;
  Render_ResPage(frameBuff[0].frame,resc);
  resc[0] = resc[1];
  sleep_ms(2 * 1000);
  resc[1].cpu.freq_cur = 200;
  resc[1].gpu.freq_cur = 900;
  resc[1].ram.ram_cur  = 12;
  Render_ResPage(frameBuff[0].frame,resc);
  resc[0] = resc[1];
  sleep_ms(2 * 1000);
  resc[1].cpu.freq_cur = 500;
  resc[1].gpu.freq_cur = 600;
  resc[1].ram.ram_cur  = 7;
  resc[1].cpu.tmp      = 52;
  resc[1].gpu.tmp      = 92;
  Render_ResPage(frameBuff[0].frame,resc);

  uint8_t data[RESC_QUEUE_SIZE];
  while (1)
  {
    if(queue_is_full(&queue_RescMasgPack_Handler)){

      printf("remove:\n");
      for (int i = 0; i < RESC_QUEUE_SIZE; i++)
      {
        queue_remove_blocking(&queue_RescMasgPack_Handler,&data[i]);
        printf("%d ",i);
      }
      break;
    }
  }

  resc_serialize resc_cur = resc_serialize_decode(data);
  printf("cpu_Freq:%d\n",resc_cur.cpu_freq);
  printf("cpu_Freq_cur:%d\n",resc_cur.cpu_freq_cur);
  printf("cpu_tmp:%d\n",resc_cur.cpu_tmp);
  printf("gpu_Freq:%d\n",resc_cur.gpu_freq);
  printf("gpu_Freq_cur:%d\n",resc_cur.gpu_freq_cur);
  printf("gpu_tmp:%d\n",resc_cur.gpu_tmp);
  printf("gpu_vram:%d\n",resc_cur.gpu_vram);
  printf("gpu_vram_cur:%.2f\n",resc_cur.gpu_vram_cur);
  printf("ram:%d\n",resc_cur.ram);
  printf("ram_cur:%.2f\n",resc_cur.ram_cur);

  printf("data array:\n");
  for(int i = 0; i < RESC_QUEUE_SIZE; i++){
    printf("0x%.2x ",data[i]);
    printf("%d ",i);
  } 

  printf("\nresc_serial data:\n");
  show_byte(&resc_cur);

  printf("\nqueue is empty:%d",queue_is_empty(&queue_RescMasgPack_Handler));
  while (1)
  {
  }
**********************************************************/
  return 0;
}