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
#include "lvgl.h"

#include <stdio.h>
#include <string.h>

#include "board_config.h"
#include "dvp.h"
#include "fpioa.h"
#include "lcd.h"
#include "nt35310.h"
#include "sleep.h"
#include "sysctl.h"
#include "uarths.h"
#include "unistd.h"

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

void lv_ex_cpicker_1(void) {
  lv_obj_t *cpicker;

  cpicker = lv_cpicker_create(lv_scr_act(), NULL);
  lv_obj_set_size(cpicker, 240, 240);
  lv_obj_align(cpicker, NULL, LV_ALIGN_CENTER, 0, 0);
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

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lcd_set_area(area->x1 + 40, area->y1 + 53, area->x2 + 40, area->y2 + 53);
  tft_write_half(&color_p->full, w * h);
  // tft_write_byte(&color_p->full, w * h / 2);

  // lcd_draw_string(320 / 3, 240 / 2, "liuzewen!", USER_COLOR);

  lv_disp_flush_ready(disp);
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
  printf("LCD init\n");
  lcd_init();
  lcd_set_direction(DIR_YX_LRUD);

  lv_init();
  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 135;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  // lcd_draw_string(320 / 3, 240 / 2, "liuzewen!", USER_COLOR);
  // sleep(3);

  lv_ex_cpicker_1();

  while (1) {
    lv_task_handler(); /* let the GUI do its work */
    msleep(5);
  }

  return 0;
}
