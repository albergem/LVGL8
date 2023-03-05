//输入驱动

#include "lv_port_indev.h"

#include <fpioa.h>
#include <gpio.h>
#include <sleep.h>

#include "encoding.h"
#include "lvgl.h"
#include "sysctl.h"

#ifdef BUTTON_INDEV

static int32_t encoder_diff;
static lv_indev_state_t encoder_state;

void lv_port_indev_init(void) {
  /* Here you will find example implementation of input devices supported by
   * LittelvGL:
   *  - Touchpad
   *  - Mouse (with cursor support)
   *  - Keypad (supports GUI usage only with key)
   *  - Encoder (supports GUI usage only with: left, right, push)
   *  - Button (external buttons to press points on the screen)
   *
   *  The `..._read()` function are only examples.
   *  You should shape them according to your hardware
   */

  /*------------------
   * Encoder
   * -----------------*/

  /*Initialize your encoder if you have*/
  printf("lvgl indev init\r\n");

  encoder_init();

  /*Register a encoder input device*/
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_ENCODER;

  indev_drv.read_cb = encoder_read;
  indev_encoder = lv_indev_drv_register(&indev_drv);

  /* 创建group */
  group = lv_group_create();
  lv_indev_set_group(indev_encoder, group);

  printf("lvgl indev init end\r\n");
}

/*------------------
 * Encoder
 * -----------------*/

/* Initialize your keypad */
static void encoder_init(void) {
  /*Your code comes here*/

  gpio_init();

  printf("encoder_init\r\n");

  fpioa_set_function(EC11_A_PIN, FUNC_GPIO0);
  fpioa_set_function(EC11_B_PIN, FUNC_GPIO1);
  fpioa_set_function(EC11_K_PIN, FUNC_GPIO2);

  gpio_set_drive_mode(0, GPIO_DM_INPUT_PULL_UP);
  gpio_set_drive_mode(1, GPIO_DM_INPUT_PULL_UP);
  gpio_set_drive_mode(2, GPIO_DM_INPUT_PULL_UP);

  printf("encoder_init end\r\n");
}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  encoder_handler();
  data->enc_diff = encoder_diff;
  data->state = encoder_state;

  // printf("x: %d,y: %d\n", data->enc_diff, data->state);
}

/*Call this function in an interrupt to process encoder events (turn, press)*/
static void encoder_handler(void) {
  encoder_state = LV_INDEV_STATE_REL;
  encoder_diff = 0;

  if (gpio_get_pin(0) == GPIO_PV_LOW) {
    msleep(5);
    if (gpio_get_pin(0) == GPIO_PV_LOW) {
      encoder_diff = -1;
      encoder_state = LV_INDEV_STATE_REL;
      uint64_t last_time = read_cycle();
      while (gpio_get_pin(0) == GPIO_PV_LOW) {
        if (read_cycle() - last_time >
            key_long_press_time * 1000 *
                sysctl_clock_get_freq(SYSCTL_CLOCK_CPU) / 1000000UL)
          break;
      }
      printf("0 x: %d,y: %d\r\n", encoder_diff, encoder_state);
    }
  }

  if (gpio_get_pin(1) == GPIO_PV_LOW) {
    msleep(5);
    if (gpio_get_pin(1) == GPIO_PV_LOW) {
      encoder_diff = 1;
      encoder_state = LV_INDEV_STATE_REL;
      uint64_t last_time = read_cycle();
      while (gpio_get_pin(1) == GPIO_PV_LOW) {
        if (read_cycle() - last_time >
            key_long_press_time * 1000 *
                sysctl_clock_get_freq(SYSCTL_CLOCK_CPU) / 1000000UL)
          break;
      }
      printf("1 x: %d,y: %d\r\n", encoder_diff, encoder_state);
    }
  }

  if (gpio_get_pin(2) == GPIO_PV_LOW) {
    encoder_state = LV_INDEV_STATE_PR;
    printf("2 x: %d,y: %d\r\n", encoder_diff, encoder_state);
  } else {
    encoder_state = LV_INDEV_STATE_REL;
  }
}

#endif
