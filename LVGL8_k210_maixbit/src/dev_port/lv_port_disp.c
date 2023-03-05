//屏幕驱动

#include "lv_port_disp.h"

#include "lcd.h"
#include "lvgl.h"
#include "main.h"
#include "nt35310.h"
#include "sleep.h"
#include "sysctl.h"
#include "uarths.h"
#include "unistd.h"

// LCD屏幕初始化

static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                       lv_color_t *color_p);

/* Initialize your display and the required peripherals. */
static void disp_init(void) {
  /* LCD init */
  printf("LCD init\r\n");
  lcd_init();
  lcd_set_direction(DIR_YX_LRUD);
}

void lv_port_disp_init(void) {
  disp_init();

  lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, buf_2,
                        screenWidth * 10); /*Initialize the display buffer*/

  // static lv_disp_draw_buf_t draw_buf;
  // static lv_color_t buf[screenWidth * 10];
  // lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*-----------------------------------
   * Register the display in LVGL
   *----------------------------------*/

  static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
  lv_disp_drv_init(&disp_drv);   /*Basic initialization*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = disp_flush;
  disp_drv.draw_buf = &draw_buf_dsc_1;
  // disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
}

/* Display flushing */ /*显示驱动*/
void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lcd_set_area(area->x1 /*+ 40*/, area->y1 /*+ 53*/, area->x2 /*+ 40*/,
               area->y2 /*+ 53*/);
  tft_write_half((uint16_t *)&color_p->full, w * h);

  lv_disp_flush_ready(disp);
}

// void lcd_draw_picture_by_half(uint16_t x1, uint16_t y1, uint16_t width,
//                               uint16_t height, uint16_t* ptr) {
//   lcd_set_area(x1, y1, x1 + width - 1, y1 + height - 1);
//   tft_write_half(ptr, width * height);
// }

// static void disp_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area,
//                        lv_color_t* color_p) {
//   /*The most simple case (but also the slowest) to put all pixels to the
//   screen
//    * one-by-one*/
//   uint16_t x1, x2, y1, y2;
//   y1 = area->y1;
//   y2 = area->y2;
//   x1 = area->x1;
//   x2 = area->x2;

//   lcd_draw_picture_by_half((uint16_t)x1, (uint16_t)y1, (uint16_t)(x2 - x1 +
//   1),
//                            (uint16_t)(y2 - y1 + 1), (uint16_t*)color_p);
//   /* IMPORTANT!!!
//    * Inform the graphics library that you are ready with the flushing*/
//   lv_disp_flush_ready(disp_drv);
// }
