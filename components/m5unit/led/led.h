#pragma once

#include "stdio.h"
#include "driver/gpio.h"
#include "esp_log.h"


typedef struct _Led_t  {
    gpio_num_t pin;             //GPIO
    uint8_t state;              //The led state
} Led_t;

void Led_Init();
esp_err_t Led_Enable(gpio_num_t pin);
Led_t* Led_Attach(gpio_num_t pin);
esp_err_t Led_OnOff(Led_t* led, uint8_t nextState);
