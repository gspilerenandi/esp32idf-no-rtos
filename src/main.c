#include <stdio.h>
#include <driver/touch_pad.h>
#include <esp_log.h>
#include <driver/gpio.h> /* specifies the GPIO names */

#define TOUCH_PAD_NO_CHANGE   (-1)
#define TOUCH_THRESH_NO_USE   (0)
#define OUTPUT_OFF (0)
#define OUTPUT_ON (1)

bool t4_on = false;
bool t5_on = false;

void delay(){ 
    long i = 0;
    while (i < 999999999) // delay
        i++;   
}

static void read_touch5(void *pt0){
    uint16_t touch_value_5;
    touch_pad_read(TOUCH_PAD_NUM5, &touch_value_5);

    if (t4_on && t5_on){
        printf("Mode 2 --- T%d:[%4d] \n", TOUCH_PAD_NUM5, touch_value_5);
        gpio_set_level(GPIO_NUM_5, OUTPUT_ON);
    } else {
        printf("Mode 1 --- T%d:[%4d] \n", TOUCH_PAD_NUM5, touch_value_5);
        if (touch_value_5 < 600){
            gpio_set_level(GPIO_NUM_5, OUTPUT_ON);
            t5_on = true;
        } else {
            gpio_set_level(GPIO_NUM_5, OUTPUT_OFF);
            t5_on = false;
        }
    }
}

static void read_touch4(void *pt4){
    uint16_t touch_value_4;
    touch_pad_read(TOUCH_PAD_NUM4, &touch_value_4);

    if (t4_on && t5_on){
        printf("Mode 2 --- T%d:[%4d] \n", TOUCH_PAD_NUM4, touch_value_4);
        gpio_set_level(GPIO_NUM_17, OUTPUT_ON);
    } else {
        printf("Mode 1 --- T%d:[%4d] \n", TOUCH_PAD_NUM4, touch_value_4);
        if (touch_value_4 < 600){
            gpio_set_level(GPIO_NUM_17, OUTPUT_ON);
            t4_on = true;
        } else {
            gpio_set_level(GPIO_NUM_17, OUTPUT_OFF);
            t4_on = false;
        }
    } 
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

void initialise_program(){
    uint16_t init = 0;
    /* the LED loops won't start until touch8 is touched */
    while (true) {
        touch_pad_read(TOUCH_PAD_NUM8, &init);
        if (init < 600){
            delay();
            break;
        }
    }
}

void t8_mode_reset(){
    uint16_t t8_reading = 0;
    touch_pad_read(TOUCH_PAD_NUM8, &t8_reading);
    if (t8_reading < 600)
        t4_on = t5_on = false;
}

void touch_reading_loop(){
    while (true) {
        read_touch4(NULL);
        delay(); 
        read_touch5(NULL);
        delay();
        t8_mode_reset();
        delay();
    }
}

void app_main() {
    /* watchdogs have been disabled in the sdkconfig file */
    initialise_gpio();
    initialise_program();
    touch_reading_loop();
}