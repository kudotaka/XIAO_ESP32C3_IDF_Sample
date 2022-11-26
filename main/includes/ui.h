/*
 * AWS IoT EduKit - Core2 for AWS IoT EduKit
 * Cloud Connected Blinky v1.4.1
 * ui.h
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

#pragma once


#ifdef CONFIG_SOFTWARE_RTC_SUPPORT
void ui_datetime_set(char *dateTxt);
#endif

#ifdef CONFIG_SOFTWARE_UNIT_ENV2_SUPPORT
void ui_temperature_update(int32_t value);
void ui_humidity_update(int32_t value);
#endif

#ifdef CONFIG_SOFTWARE_BUTTON_SUPPORT
void ui_button_label_update(bool state);
#endif

#ifdef CONFIG_SOFTWARE_MODEL_SEEED_XIAO_ESP32C3
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
void ui_wifi_label_update(bool state);
#endif
#endif

void ui_init();