#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

#include <stdlib.h>
#include "ui.h" 
#include <math.h>
#include "resource.h"
#include "frame_template.h"

resource resc[2];

int main()
{
  stdio_init_all();

  i2c_init(i2c0, OLED_BAUD);
  gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
  gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_SCL);
  gpio_pull_up(PIN_SDA);
  sleep_ms(100);
  OLED_Init();
  OLED_Clear();

  OLED_initFrame(frameBuff[0].frame);

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



  while(1)
  {
    sleep_ms(1000 *2);
    printf("cpu usage:%d",resc[0].cpu.core_usage);
    printf("ram usage:%d",resc[0].ram.ram_usage);
  }


    return 0;
}