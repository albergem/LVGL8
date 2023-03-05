//屏幕驱动

#ifndef _DSP_DEV_H_
#define _DSP_DEV_H_

#include "lvgl.h"
#include "main.h"

// static lv_disp_draw_buf_t draw_buf_dsc_1;
// static lv_color_t buf_1[screenWidth * 10]; /*A buffer for 10 rows*/
// static lv_color_t buf_2[screenWidth * 10]; /*An other buffer for 10 rows*/

void lv_port_disp_init(void);
static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                       lv_color_t *color_p);

#endif
