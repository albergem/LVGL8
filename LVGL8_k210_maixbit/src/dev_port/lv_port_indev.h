//输入驱动
#ifndef _IN_DEV_H_
#define _IN_DEV_H_

#include "lvgl.h"
#include "main.h"

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t *indev_touchpad;
lv_indev_t *indev_mouse;
lv_indev_t *indev_keypad;
lv_indev_t *indev_encoder;

lv_indev_drv_t indev_drv;

lv_group_t *group;

#define BUTTON_INDEV

#ifdef BUTTON_INDEV

void lv_port_indev_init(void);

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static void encoder_handler(void);

#endif

#endif
