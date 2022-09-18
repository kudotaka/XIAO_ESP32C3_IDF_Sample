/*
 * AWS IoT EduKit - Core2 for AWS IoT EduKit
 * Cloud Connected Blinky v1.4.1
 * .c
 * 
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "xiao.h"
#include "ui.h"

#define MAX_TEXTAREA_LENGTH 128

static lv_obj_t *active_screen;
static lv_obj_t *wifi_label;

#ifdef CONFIG_SOFTWARE_I2C_UNIT_ENV2_SUPPORT
#define MOJI_DEGREESIGN  "°C"
static lv_obj_t *humidity_current;
static lv_obj_t *humidity_label;
static lv_obj_t *temperature_current;
static lv_obj_t *temperature_label;
#endif

#ifdef CONFIG_SOFTWARE_RTC_SUPPORT
static lv_obj_t *datetime_txtlabel;
#endif

#ifdef CONFIG_SOFTWARE_BUTTON_SUPPORT
static lv_obj_t *button_label;
#endif

static char *TAG = "UI";

#ifdef CONFIG_SOFTWARE_MODEL_SEEED_XIAO_ESP32C3
void ui_wifi_label_update(bool state){
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    if (state == false) {
        lv_label_set_text(wifi_label, "");
    } 
    else{
        lv_label_set_text(wifi_label, LV_SYMBOL_WIFI);
    }
    xSemaphoreGive(xGuiSemaphore);
}
#endif

#ifdef CONFIG_SOFTWARE_BUTTON_SUPPORT
void ui_button_label_update(bool state){
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    if (state == false) {
        lv_label_set_text(button_label, "");
    } 
    else{
        lv_label_set_text(button_label, LV_SYMBOL_OK);
    }
    xSemaphoreGive(xGuiSemaphore);
}
#endif

#ifdef CONFIG_SOFTWARE_RTC_SUPPORT
void ui_datetime_set(char *dateTxt) {
    if( dateTxt != NULL ){
        xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

        lv_label_set_text(datetime_txtlabel, dateTxt);

        xSemaphoreGive(xGuiSemaphore);
    } 
    else{
        ESP_LOGE(TAG, "datetime dateTxt is NULL!");
    }
}
#endif

#ifdef CONFIG_SOFTWARE_I2C_UNIT_ENV2_SUPPORT
void ui_humidity_update(int32_t value){
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    lv_label_set_text_fmt(humidity_current, "%d", value);

    xSemaphoreGive(xGuiSemaphore);
}

void ui_temperature_update(int32_t value){
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    lv_label_set_text_fmt(temperature_current, "%d", value);

    xSemaphoreGive(xGuiSemaphore);
}
#endif


void ui_init() {
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    ESP_LOGI(TAG, "ui_init() start.");
    active_screen = lv_scr_act();

#ifdef CONFIG_SOFTWARE_MODEL_SEEED_XIAO_ESP32C3
    wifi_label = lv_label_create(active_screen, NULL);
    lv_obj_align(wifi_label,NULL,LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_label_set_text(wifi_label, "");
#endif

#ifdef CONFIG_SOFTWARE_BUTTON_SUPPORT
    button_label = lv_label_create(active_screen, NULL);
    lv_obj_align(button_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_label_set_text(button_label, "");
#endif


#ifdef CONFIG_SOFTWARE_I2C_UNIT_ENV2_SUPPORT
    humidity_current = lv_label_create(active_screen, NULL);
    lv_label_set_long_mode(humidity_current, LV_LABEL_LONG_DOT);
    lv_label_set_align(humidity_current, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text_fmt(humidity_current, "%d", 0);
    lv_obj_align(humidity_current, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 16);
    humidity_label = lv_label_create(active_screen, NULL);
    lv_label_set_align(humidity_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(humidity_label, "%");
    lv_obj_align(humidity_label, NULL, LV_ALIGN_IN_TOP_LEFT, 27, 16);

    temperature_current = lv_label_create(active_screen, NULL);
    lv_label_set_long_mode(temperature_current, LV_LABEL_LONG_DOT);
    lv_label_set_align(temperature_current, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text_fmt(temperature_current, "%d", 0);
    lv_obj_align(temperature_current, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 32);
    temperature_label = lv_label_create(active_screen, NULL);
    lv_label_set_align(temperature_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(temperature_label, MOJI_DEGREESIGN);
    lv_obj_align(temperature_label, NULL, LV_ALIGN_IN_TOP_LEFT, 25, 32);
#endif

#ifdef CONFIG_SOFTWARE_RTC_SUPPORT
    char str1[30] = {0};
    sprintf(str1,"%04d/%02d/%02d %02d:%02d:%02d", 2022, 1, 1, 12, 0, 0);
    datetime_txtlabel = lv_label_create(active_screen, NULL);
    lv_label_set_align(datetime_txtlabel, LV_LABEL_ALIGN_LEFT);
    lv_label_set_text(datetime_txtlabel, str1);
    lv_obj_align(datetime_txtlabel, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 48);
#endif

    xSemaphoreGive(xGuiSemaphore);
}