/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <sleep.h>
#include <stdio.h>
#include <string.h>

#include "encoding.h"
#include "fpioa.h"
#include "lcd.h"
#include "lv_demo.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl.h"
#include "nt35310.h"
#include "sysctl.h"
#include "uarths.h"
#include "unistd.h"

static void io_mux_init(void) {
  /* Init SPI IO map and function settings */
  fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
  fpioa_set_function(36, FUNC_SPI0_SS3);
  fpioa_set_function(39, FUNC_SPI0_SCLK);
  fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);
  sysctl_set_spi0_dvp_data(1);
}

static void io_set_power(void) {
  /* Set dvp and spi pin to 1.8V */
  sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
  sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
}

uint32_t buff[240 * 320];

int main(void) {
  /* Set CPU and dvp clk */
  sysctl_pll_set_freq(SYSCTL_PLL0, 800000000UL);
  sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
  sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);

  uarths_init();

  io_mux_init();
  io_set_power();
  plic_init();

  lv_init();

  //屏幕驱动
  lv_port_disp_init();
  //输入驱动
  lv_port_indev_init();

  // lv_demo_widgets();  // OK
  // lv_demo_benchmark();  // OK
  lv_demo_keypad_encoder();  // works, but I haven't an encoder
  // lv_demo_music();  // NOK
  // lv_demo_stress();  // seems to be OK

  // uint64_t last_time = read_cycle();
  lcd_clear(BLACK);

  while (1) {
    // if (read_cycle() - last_time >
    //     5 * 1000 * sysctl_clock_get_freq(SYSCTL_CLOCK_CPU) / 1000000UL) {
    //   last_time = read_cycle();
    //   lv_task_handler(); /* let the GUI do its work */
    //   lv_tick_inc(5);
    // }

    lv_task_handler(); /* let the GUI do its work */
    lv_tick_inc(5);
    msleep(5);
  }

  return 0;
}
