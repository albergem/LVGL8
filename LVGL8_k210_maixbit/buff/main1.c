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
#include <sysctl.h>

#include "board_config.h"
#include "dvp.h"
#include "fpioa.h"
#include "lcd.h"
#include "lv_examples.h"
#include "lvgl.h"
#include "nt35310.h"
#include "ov2640.h"
#include "ov5640.h"
#include "plic.h"
#include "sysctl.h"
#include "uarths.h"
#include "unistd.h"

void lv_port_disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                       lv_color_t *color_p);

static void io_mux_init(void) {
  /* Init DVP IO map and function settings */
  fpioa_set_function(42, FUNC_CMOS_RST);
  fpioa_set_function(13, FUNC_CMOS_PWDN);
  fpioa_set_function(46, FUNC_CMOS_XCLK);
  fpioa_set_function(43, FUNC_CMOS_VSYNC);
  fpioa_set_function(45, FUNC_CMOS_HREF);
  fpioa_set_function(47, FUNC_CMOS_PCLK);
  fpioa_set_function(41, FUNC_SCCB_SCLK);
  fpioa_set_function(40, FUNC_SCCB_SDA);

  /* Init SPI IO map and function settings */
  fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
  fpioa_set_function(37, FUNC_SPI0_SS3);
  fpioa_set_function(39, FUNC_SPI0_SCLK);
  // fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);

  sysctl_set_spi0_dvp_data(1);
}

static void io_set_power(void) {
#if 1
  /* Set dvp and spi pin to 1.8V */
  sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
  sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);

#else
  /* Set dvp and spi pin to 1.8V */
  sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
  sysctl_set_power_mode(SYSCTL_POWER_BANK2, SYSCTL_POWER_V18);
#endif
}

int main(void) {
  /* Set CPU and dvp clk */
  sysctl_pll_set_freq(SYSCTL_PLL0, 800000000UL);
  sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
  sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);

  uarths_init();

  io_mux_init();
  io_set_power();

  /* LCD init */
  printf("LCD init\n");
  lcd_init();
  lcd_set_direction(DIR_YX_LRUD);

  /*Initialize LittlevGL*/
  lv_init();
  lv_port_disp_init();

  lv_demo_widgets();

  dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
  dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

  lcd_draw_string(320 / 2, 240 / 2, "liuzewen!", USER_COLOR);
  sleep(2);

  while (1) {
    lv_task_handler();
    msleep(3); /*Just to let the system breathe */
  }

  return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void) {
  static lv_disp_buf_t draw_buf_dsc_1;
  static lv_color_t draw_buf_1[LV_HOR_RES_MAX * 10]; /*A buffer for 10 rows*/
  lv_disp_buf_init(&draw_buf_dsc_1, draw_buf_1, NULL,
                   LV_HOR_RES_MAX * 10); /*Initialize the display buffer*/

  /*-----------------------------------
   * Register the display in LVGL
   *----------------------------------*/

  lv_disp_drv_t disp_drv;      /*Descriptor of a display driver*/
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/

  /*Set up the functions to access to your display*/

  /*Set the resolution of the display*/
  disp_drv.hor_res = 320;
  disp_drv.ver_res = 240;

  /*Used to copy the buffer's content to the display*/
  disp_drv.flush_cb = disp_flush;

  /*Set a display buffer*/
  disp_drv.buffer = &draw_buf_dsc_1;

  /*Finally register the driver*/
  lv_disp_drv_register(&disp_drv);
}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the
 * background but 'lv_disp_flush_ready()' has to be called when finished. */
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                       lv_color_t *color_p) {
  uint16_t c;
  int32_t x;
  int32_t y;

  for (y = area->y1; y <= area->y2; y++) {
    for (x = area->x1; x <= area->x2; x++) {
      /* Put a pixel to the display. For example: */
      /* put_px(x, y, *color_p)*/
      c = color_p->full;
      // tft.writeColor(c, 1);
      lcd_draw_point(x, y, c);
      color_p++;
    }
  }

  printf("刷新中！\n");
  lv_disp_flush_ready(disp_drv);
}
