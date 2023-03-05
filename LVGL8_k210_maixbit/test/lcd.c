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
#include "lcd.h"

#include <stdio.h>
#include <string.h>

#include "board_config.h"
#include "dvp.h"
#include "fpioa.h"
#include "nt35310.h"
#include "sleep.h"
#include "sysctl.h"
#include "uarths.h"
#include "unistd.h"

uint32_t g_lcd_gram0[38400] __attribute__((aligned(64)));
uint32_t g_lcd_gram1[38400] __attribute__((aligned(64)));

volatile uint8_t g_dvp_finish_flag;
volatile uint8_t g_ram_mux;

static int on_irq_dvp(void *ctx) {
  if (dvp_get_interrupt(DVP_STS_FRAME_FINISH)) {
    /* switch gram */
    dvp_set_display_addr(g_ram_mux ? (uint32_t)g_lcd_gram0
                                   : (uint32_t)g_lcd_gram1);

    dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
    g_dvp_finish_flag = 1;
  } else {
    if (g_dvp_finish_flag == 0) dvp_start_convert();
    dvp_clear_interrupt(DVP_STS_FRAME_START);
  }

  return 0;
}

static void io_mux_init(void) {
  /* Init SPI IO map and function settings */
  fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
  fpioa_set_function(37, FUNC_SPI0_SS3);
  fpioa_set_function(39, FUNC_SPI0_SCLK);
  // fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);

  sysctl_set_spi0_dvp_data(1);
}

static void io_set_power(void) {
  /* Set dvp and spi pin to 1.8V */
  sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
  sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
}

int main(void) {
  /* Set CPU and dvp clk */
  sysctl_pll_set_freq(SYSCTL_PLL0, 800000000UL);
  sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
  sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
  uarths_init();

  io_mux_init();
  io_set_power();
  plic_init();

  /* LCD init */
  printf("LCD init\r\n");
  lcd_init();
  lcd_set_direction(DIR_YX_LRUD);

  lcd_draw_string(320 / 3, 240 / 2, "liuzewen!", USER_COLOR);
  sleep(2);

  while (1) {
    lcd_draw_string(320 / 3, 240 / 2, "liuzewen!", USER_COLOR);
    sleep(2);
  }

  return 0;
}
