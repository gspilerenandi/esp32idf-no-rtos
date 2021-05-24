#include <stdio.h>
#include "driver/touch_pad.h"
#include "esp_log.h"
#include "driver/gpio.h" /* specifies the GPIO names */

#define TOUCH_PAD_NO_CHANGE   (-1)
#define TOUCH_THRESH_NO_USE   (0)
#define OUTPUT_OFF (0)
#define OUTPUT_ON (1)

static void read_touch5(void *pt0){
    uint16_t touch_value_5;
    long i = 0;
    touch_pad_read(TOUCH_PAD_NUM5, &touch_value_5);
    printf("T%d:[%4d] \n", TOUCH_PAD_NUM5, touch_value_5);
    if (touch_value_5 < 600)
        gpio_set_level(GPIO_NUM_5, OUTPUT_ON);
    else
        gpio_set_level(GPIO_NUM_5, OUTPUT_OFF);
    while (i < 10000000) // delay
        i++;
}

static void read_touch4(void *pt4){
    uint16_t touch_value_4;
    long i = 0;
    touch_pad_read(TOUCH_PAD_NUM4, &touch_value_4);
    printf("T%d:[%4d] \n", TOUCH_PAD_NUM4, touch_value_4);
    if (touch_value_4 < 600)
        gpio_set_level(GPIO_NUM_17, OUTPUT_ON);
    else
        gpio_set_level(GPIO_NUM_17, OUTPUT_OFF);
    while (i < 10000000) // delay
        i++;   
}

static void initialise_gpio(){
    /* capacitive sensors */
    touch_pad_init();
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(TOUCH_PAD_NUM5, TOUCH_THRESH_NO_USE);
    touch_pad_config(TOUCH_PAD_NUM4, TOUCH_THRESH_NO_USE);
    touch_pad_config(TOUCH_PAD_NUM8, TOUCH_THRESH_NO_USE);
    /* output pins */
    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = (1 << 17) | (1 << 5); // (1 << PIN) what is the purpose of << ?
    gpioConfig.mode = GPIO_MODE_OUTPUT;
    gpio_config(&gpioConfig);
}

void init_program(){
    uint16_t init;
    while (true) {
        touch_pad_read(TOUCH_PAD_NUM8, &init);
        if (init < 600)
            break;
    }
}

void touch_reading_loop(){
    while (true) {
        read_touch4(NULL);
        read_touch5(NULL);
    }
}

void app_main() {
    /* watchdogs have been disabled in the sdkconfig file*/
    initialise_gpio();
    init_program();
    touch_reading_loop();
}