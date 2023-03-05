#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <string.h>

#include "lvgl.h"
#include "sleep.h"
#include "stdint.h"
#include "sysctl.h"
#include "uarths.h"
#include "unistd.h"

//按键配置
#define EC11_A_PIN 24
#define EC11_B_PIN 23
#define EC11_K_PIN 34

//屏幕宽长
static const uint32_t screenWidth = 320;   // 240;
static const uint32_t screenHeight = 240;  // 135;

static lv_disp_draw_buf_t draw_buf_dsc_1;
static lv_color_t buf_1[320 * 10]; /*A buffer for 10 rows*/
static lv_color_t buf_2[320 * 10]; /*An other buffer for 10 rows*/

static const long key_long_press_time = 3000;

#endif /* _MAIN_H */
