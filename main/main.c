#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_sntp.h"

#include "xiao.h"
#include "ui.h"
#include "wifi.h"

#if ( CONFIG_SOFTWARE_UNIT_ENV2_SUPPORT \
    || CONFIG_SOFTWARE_UNIT_SK6812_SUPPORT \
    || CONFIG_SOFTWARE_UNIT_4DIGIT_DISPLAY_SUPPORT \
    || CONFIG_SOFTWARE_UNIT_6DIGIT_DISPLAY_SUPPORT \
    || CONFIG_SOFTWARE_I2C_LCD_ST7032_SUPPORT \
    || CONFIG_SOFTWARE_UNIT_LED_SUPPORT )
#include "m5unit.h"
#endif

static const char *TAG = "MY-MAIN";

#ifdef CONFIG_SOFTWARE_BUZZER_SUPPORT
TaskHandle_t xBuzzer;
static void vLoopBuzzerTask(void* pvParameters) {
    ESP_LOGI(TAG, "Buzzer is start!");
    XIAO_Buzzer_Init(PORT_D3_PIN);
    uint32_t DUTY_MAX_VALUE = 8190;
    uint32_t duty[] = {DUTY_MAX_VALUE/4*2};
    while (1) {
        vTaskSuspend(NULL);
        for (uint32_t i = 0; i < sizeof(duty)/sizeof(uint32_t); i++)
        {
            ESP_LOGI(TAG, "Buzzer duty:%d", duty[i]);
            XIAO_Buzzer_Play_Duty_Frequency(duty[i], 261.6);
            vTaskDelay( pdMS_TO_TICKS(500) );
            XIAO_Buzzer_Play_Duty_Frequency(duty[i], 293.665);
            vTaskDelay( pdMS_TO_TICKS(500) );
            XIAO_Buzzer_Play_Duty_Frequency(duty[i], 329.63);
            vTaskDelay( pdMS_TO_TICKS(500) );
            XIAO_Buzzer_Play_Duty_Frequency(duty[i], 349.228);
            vTaskDelay( pdMS_TO_TICKS(500) );
            XIAO_Buzzer_Play_Duty_Frequency(duty[i], 391.995);
            vTaskDelay( pdMS_TO_TICKS(500) );
            XIAO_Buzzer_Play_Duty_Frequency(duty[i], 440);
            vTaskDelay( pdMS_TO_TICKS(500) );
            XIAO_Buzzer_Play_Duty_Frequency(duty[i], 493.883);
            vTaskDelay( pdMS_TO_TICKS(500) );
            XIAO_Buzzer_Play_Duty_Frequency(duty[i], 523.251);
            vTaskDelay( pdMS_TO_TICKS(500) );

            XIAO_Buzzer_Stop();
            vTaskDelay( pdMS_TO_TICKS(5000) );
        }
    }

    vTaskDelete(NULL); // Should never get to here...
}
#endif

#ifdef CONFIG_SOFTWARE_BUTTON_SUPPORT
TaskHandle_t xButton;
Button_t* button_d1_pin;
Button_t* button_d7_pin;
Button_t* button_d6_pin;
Button_t* button_d0_pin;

static void button_task(void* pvParameters) {
    ESP_LOGI(TAG, "start button_task");

    if (Button_Enable(PORT_D1_PIN, ACTIVE_LOW) == ESP_OK) {
        button_d1_pin = Button_Attach(PORT_D1_PIN, ACTIVE_LOW);
    }

    if (Button_Enable(PORT_D7_PIN, ACTIVE_LOW) == ESP_OK) {
        button_d7_pin = Button_Attach(PORT_D7_PIN, ACTIVE_LOW);
    }
    if (Button_Enable(PORT_D6_PIN, ACTIVE_LOW) == ESP_OK) {
        button_d6_pin = Button_Attach(PORT_D6_PIN, ACTIVE_LOW);
    }
    if (Button_Enable(PORT_D0_PIN, ACTIVE_HIGH) == ESP_OK) {
        button_d0_pin = Button_Attach(PORT_D0_PIN, ACTIVE_HIGH);
    }
    while(1){

        if (Button_WasPressed(button_d1_pin)) {
            ESP_LOGI(TAG, "PORT_D1_PIN BUTTON PRESSED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(true);
#endif
        }
        if (Button_WasReleased(button_d1_pin)) {
            ESP_LOGI(TAG, "PORT_D1_PIN BUTTON RELEASED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(false);
#endif
        }
        if (Button_WasLongPress(button_d1_pin, pdMS_TO_TICKS(1000))) { // 1Sec
            ESP_LOGI(TAG, "PORT_D1_PIN BUTTON LONGPRESS!");
#ifdef CONFIG_SOFTWARE_BUZZER_SUPPORT
            vTaskResume(xBuzzer);
#endif
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(false);
#endif
        }

        if (Button_WasPressed(button_d7_pin)) {
            ESP_LOGI(TAG, "PORT_D7_PIN BUTTON PRESSED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(true);
#endif
        }
        if (Button_WasReleased(button_d7_pin)) {
            ESP_LOGI(TAG, "PORT_D7_PIN BUTTON RELEASED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(false);
#endif
        }
        if (Button_WasLongPress(button_d7_pin, pdMS_TO_TICKS(1000))) { // 1Sec
            ESP_LOGI(TAG, "PORT_D7_PIN BUTTON LONGPRESS!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(false);
#endif
        }
        if (Button_WasPressed(button_d6_pin)) {
            ESP_LOGI(TAG, "PORT_D6_PIN BUTTON PRESSED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(true);
#endif
        }
        if (Button_WasReleased(button_d6_pin)) {
            ESP_LOGI(TAG, "PORT_D6_PIN BUTTON RELEASED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(false);
#endif
        }
        if (Button_WasLongPress(button_d6_pin, pdMS_TO_TICKS(1000))) { // 1Sec
            ESP_LOGI(TAG, "PORT_D6_PIN BUTTON LONGPRESS!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(false);
#endif
        }
        if (Button_WasPressed(button_d0_pin)) {
            ESP_LOGI(TAG, "PORT_D0_PIN BUTTON PRESSED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(true);
#endif
        }
        if (Button_WasReleased(button_d0_pin)) {
            ESP_LOGI(TAG, "PORT_D0_PIN BUTTON RELEASED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(false);
#endif
        }
        if (Button_WasLongPress(button_d0_pin, pdMS_TO_TICKS(1000))) { // 1Sec
            ESP_LOGI(TAG, "PORT_D0_PIN BUTTON LONGPRESS!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(false);
#endif
        }

        vTaskDelay(pdMS_TO_TICKS(80));
    }
    vTaskDelete(NULL); // Should never get to here...
}
#endif

#ifdef CONFIG_SOFTWARE_RTC_SUPPORT
TaskHandle_t xRtc;
TaskHandle_t xClock;
static bool g_timeInitialized = false;
const char servername[] = "ntp.jst.mfeed.ad.jp";

static void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
    g_timeInitialized = true;
}

void vLoopRtcTask(void *pvParametes)
{
    //PCF8563
    ESP_LOGI(TAG, "start vLoopRtcTask. TaskDelayTime");

    // Set timezone to Japan Standard Time
    setenv("TZ", "JST-9", 1);
    tzset();

    ESP_LOGI(TAG, "ServerName:%s", servername);
    sntp_setservername(0, servername);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);

    while (1) {
        if (wifi_isConnected() == ESP_OK) {
            sntp_init();
        } else {
            vTaskDelay( pdMS_TO_TICKS(60000) );
            continue;
        }

        ESP_LOGI(TAG, "Waiting for time synchronization with SNTP server");
        while (!g_timeInitialized)
        {
            vTaskDelay( pdMS_TO_TICKS(5000) );
        }

        time_t now = 0;
        struct tm timeinfo = {0};
        time(&now);
        localtime_r(&now, &timeinfo);
        char str1[72] = {0};
        sprintf(str1,"NTP Update : %04d/%02d/%02d %02d:%02d", timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min);
        ESP_LOGI(TAG, "%s", str1);

        rtc_date_t rtcdate;
        rtcdate.year = timeinfo.tm_year+1900;
        rtcdate.month = timeinfo.tm_mon+1;
        rtcdate.day = timeinfo.tm_mday;
        rtcdate.hour = timeinfo.tm_hour;
        rtcdate.minute = timeinfo.tm_min;
        rtcdate.second = timeinfo.tm_sec;
        PCF8563_SetTime(&rtcdate);

        g_timeInitialized = false;
        sntp_stop();

        vTaskDelay( pdMS_TO_TICKS(600000) );
    }
}

void vLoopClockTask(void *pvParametes)
{
    //PCF8563
    ESP_LOGI(TAG, "start vLoopClockTask.");
 
    // Set timezone to Japan Standard Time
    setenv("TZ", "JST-9", 1);
    tzset();

    while (1) {
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
        rtc_date_t rtcdate;
        PCF8563_GetTime(&rtcdate);
        char str1[30] = {0};
        sprintf(str1,"%04d/%02d/%02d %02d:%02d:%02d", rtcdate.year, rtcdate.month, rtcdate.day, rtcdate.hour, rtcdate.minute, rtcdate.second);
        ui_datetime_set(str1);
#endif
        vTaskDelay( pdMS_TO_TICKS(990) );
    }
}
#endif

#if CONFIG_SOFTWARE_UNIT_SK6812_SUPPORT
TaskHandle_t xExternalRGBLedBlink;
void vexternal_LoopRGBLedBlinkTask(void *pvParametes)
{
    ESP_LOGI(TAG, "start vexternal_LoopRGBLedBlinkTask");

    pixel_settings_t px_ext1;
    uint8_t blink_count = 5;
    uint32_t colors[] = {SK6812_COLOR_BLUE, SK6812_COLOR_LIME, SK6812_COLOR_AQUA
                    , SK6812_COLOR_RED, SK6812_COLOR_MAGENTA, SK6812_COLOR_YELLOW
                    , SK6812_COLOR_WHITE};
    Sk6812_Init(&px_ext1, PORT_D10_PIN, RMT_CHANNEL_0, 37*2);
    while (1) {
        for (uint8_t c = 0; c < sizeof(colors)/sizeof(uint32_t); c++) {
            for (uint8_t i = 0; i < blink_count; i++) {
                Sk6812_SetAllColor(&px_ext1, colors[c]);
                Sk6812_Show(&px_ext1, RMT_CHANNEL_0);
    ESP_LOGI(TAG, "Sk6812 show");
                vTaskDelay(pdMS_TO_TICKS(1000));

                Sk6812_SetAllColor(&px_ext1, SK6812_COLOR_OFF);
                Sk6812_Show(&px_ext1, RMT_CHANNEL_0);
    ESP_LOGI(TAG, "Sk6812 off");
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }
}
#endif

#ifdef CONFIG_SOFTWARE_UNIT_ENV2_SUPPORT
TaskHandle_t xUnitEnv2;
void vLoopUnitEnv2Task(void *pvParametes)
{
    ESP_LOGI(TAG, "start I2C Sht3x");
    esp_err_t ret = ESP_OK;
    ret = Sht3x_Init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Sht3x_I2CInit Error");
        return;
    }
    ESP_LOGI(TAG, "Sht3x_Init() is OK!");
    while (1) {
        ret = Sht3x_Read();
        if (ret == ESP_OK) {
            vTaskDelay( pdMS_TO_TICKS(100) );
            ESP_LOGI(TAG, "temperature:%f, humidity:%f", Sht3x_GetTemperature(), Sht3x_GetHumidity());
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_temperature_update( Sht3x_GetIntTemperature() );
            ui_humidity_update( Sht3x_GetIntHumidity() );
#endif
        } else {
            ESP_LOGE(TAG, "Sht3x_Read() is error code:%d", ret);
            vTaskDelay( pdMS_TO_TICKS(10000) );
        }

        vTaskDelay( pdMS_TO_TICKS(5000) );
    }
}
#endif

#if CONFIG_SOFTWARE_UNIT_LED_SUPPORT
// SELECT GPIO_NUM_XX OR PORT_DX_PIN
TaskHandle_t xExternalLED;
Led_t* led_ext1;
Led_t* led_ext2;
static void external_led_task(void* pvParameters) {
    ESP_LOGI(TAG, "start external_led_task");
    Led_Init();
    if (Led_Enable(PORT_D1_PIN, ACTIVE_HIGH) == ESP_OK) {
        led_ext1 = Led_Attach(PORT_D1_PIN, ACTIVE_HIGH);
    }
    if (Led_Enable(PORT_D3_PIN, ACTIVE_LOW) == ESP_OK) {
        led_ext2 = Led_Attach(PORT_D3_PIN, ACTIVE_LOW);
    }
    while(1){
        Led_OnOff(led_ext1, true);
        Led_OnOff(led_ext2, true);
        vTaskDelay(pdMS_TO_TICKS(500));

        Led_OnOff(led_ext1, false);
        Led_OnOff(led_ext2, false);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL); // Should never get to here...
}
#endif

#if ( CONFIG_SOFTWARE_UNIT_4DIGIT_DISPLAY_SUPPORT || CONFIG_SOFTWARE_UNIT_6DIGIT_DISPLAY_SUPPORT )
TaskHandle_t xDigitDisplay;
DigitDisplay_t* digitdisplay_1;
void vLoopUnitDigitDisplayTask(void *pvParametes)
{
    ESP_LOGI(TAG, "start Digit Display");

// Sample Rowdata
//  --0x01--
// |        |
//0x20     0x02
// |        |
//  --0x40- -
// |        |
//0x10     0x04
// |        |
//  --0x08--   0x80


// Sample time
#ifdef CONFIG_SOFTWARE_RTC_SUPPORT
    rtc_date_t rtcdate;
    uint8_t currentHour = 0;
    uint8_t currentMinute = 0;
    uint8_t currentSecond = 0;

    Tm1637_Init();
    if (Tm1637_Enable(PORT_D9_PIN, PORT_D10_PIN) == ESP_OK) {
        digitdisplay_1 = Tm1637_Attach(PORT_D9_PIN, PORT_D10_PIN, BRIGHT_DARKEST);
    } else {
        ESP_LOGE(TAG, "Digit Display Tm1637_Enable Error");
        vTaskDelete(NULL);
    }
    Tm1637_ClearDisplay(digitdisplay_1);
    vTaskDelay(pdMS_TO_TICKS(100));
    while(1) {
        PCF8563_GetTime(&rtcdate);
        if (currentHour != rtcdate.hour) {
            currentHour = rtcdate.hour;
            Tm1637_DisplayBitAddPoint(digitdisplay_1, 0, currentHour / 10, POINT_OFF);
            Tm1637_DisplayBitAddPoint(digitdisplay_1, 1, currentHour % 10, POINT_ON);
        }
        if (currentMinute != rtcdate.minute) {
            currentMinute = rtcdate.minute;
            Tm1637_DisplayBitAddPoint(digitdisplay_1, 2, currentMinute / 10, POINT_OFF);
            Tm1637_DisplayBitAddPoint(digitdisplay_1, 3, currentMinute % 10, POINT_ON);
        }
        if (currentSecond != rtcdate.second) {
            currentSecond = rtcdate.second;
            Tm1637_DisplayBitAddPoint(digitdisplay_1, 4, currentSecond / 10, POINT_OFF);
            Tm1637_DisplayBitAddPoint(digitdisplay_1, 5, currentSecond % 10, POINT_OFF);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL); // Should never get to here...
#endif

/*
// Sample animetion
    uint8_t anime[] = {0x00, 0x30, 0x38, 0x78, 0x79, 0x7f};
    uint8_t animeCurrent = 0;
    uint8_t animeMax = sizeof(anime)/sizeof(uint8_t);
    uint8_t animeDigitPosition = 1;
    uint8_t animeDigitMax = DIGIT_COUNT;
    uint8_t data = 0x00;
    Tm1637_Init();
    if (Tm1637_Enable(PORT_D9_PIN, PORT_D10_PIN) == ESP_OK) {
        digitdisplay_1 = Tm1637_Attach(PORT_D9_PIN, PORT_D10_PIN, BRIGHT_TYPICAL);
    } else {
        ESP_LOGE(TAG, "Digit Display Tm1637_Enable Error");
        vTaskDelete(NULL);
    }
    while(1) {
        Tm1637_ClearDisplay(digitdisplay_1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        for (animeDigitPosition = 1; animeDigitPosition < animeDigitMax+1; animeDigitPosition++) {
            for (animeCurrent = 0; animeCurrent < animeMax; animeCurrent++) {
                for (uint8_t position = DIGIT_COUNT; position > animeDigitPosition-1; position--) {
                    for (uint8_t digit = DIGIT_COUNT; digit > 0; digit--) {
                        data = 0x00;
                        if (digit == position) {
                            data += 0x80;
                        } else {
                            //data = 0x00;
                        }

                        if (digit == animeDigitPosition) {
                            data += anime[animeCurrent];
                        } else if (digit < animeDigitPosition) {
                            data = 0xff;
                        }

                        Tm1637_DisplayBitRowdata(digitdisplay_1, digit-1, data);
                    }
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
            }
        }
    }
    vTaskDelete(NULL); // Should never get to here...
*/
/*
// Sample number
    uint8_t listDisp_1[DIGIT_COUNT];
    uint8_t count = 0;
    Tm1637_Init();
    if (Tm1637_Enable(PORT_D9_PIN, PORT_D10_PIN) == ESP_OK) {
        digitdisplay_1 = Tm1637_Attach(PORT_D9_PIN, PORT_D10_PIN, BRIGHT_TYPICAL);
    } else {
        ESP_LOGE(TAG, "Digit Display Tm1637_Enable Error");
        vTaskDelete(NULL);
    }
    Tm1637_ClearDisplay(digitdisplay_1);
    while(1){
        if (count == UINT8_MAX) {
            count = 0;
        }
        if (count%2) {
            for (uint8_t i = 0; i < DIGIT_COUNT; i++) {
                listDisp_1[i] = i;
            }
        } else {
            for (uint8_t i = 0; i < DIGIT_COUNT; i++) {
                listDisp_1[i] = i+4;
            }
        }
        Tm1637_DisplayAll(digitdisplay_1, listDisp_1);
        count++;
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    vTaskDelete(NULL); // Should never get to here...
*/
/*
// Sample message
    Tm1637_Init();
    if (Tm1637_Enable(PORT_D9_PIN, PORT_D10_PIN) == ESP_OK) {
        digitdisplay_1 = Tm1637_Attach(PORT_D9_PIN, PORT_D10_PIN, BRIGHT_TYPICAL);
    } else {
        ESP_LOGE(TAG, "Digit Display Tm1637_Enable Error");
        vTaskDelete(NULL);
    }
    while (1) {
        Tm1637_ClearDisplay(digitdisplay_1);
        vTaskDelay(pdMS_TO_TICKS(500));
        Tm1637_DisplayStr(digitdisplay_1, "HELL0-1234567890", 500);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL); // Should never get to here...
*/
}
#endif

#ifdef CONFIG_SOFTWARE_I2C_LCD_ST7032_SUPPORT
TaskHandle_t xLcdSt7032;
uint8_t degree[] = {
    0b11000,
    0b11000,
    0b00110,
    0b01001,
    0b01000,
    0b01001,
    0b00110,
    0b00000,
};

void vLoopLcdSt7032Task(void *pvParametes)
{
    ESP_LOGI(TAG, "start LCD ST7032");

    esp_err_t ret = ESP_OK;
#ifdef CONFIG_SOFTWARE_USE_BACKLIGHT
    ret = St7032_Init(I2C_NUM_0, PORT_SDA_PIN, PORT_SCL_PIN, PORT_I2C_STANDARD_BAUD, PORT_D9_PIN);
#else
    ret = St7032_Init(I2C_NUM_0, PORT_SDA_PIN, PORT_SCL_PIN, PORT_I2C_STANDARD_BAUD);
#endif
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "LCD ST7032 INIT Error");
        return;
    }
    ESP_LOGI(TAG, "LCD ST7032 INIT is OK!");

// Sample ALL Character
    St7032_Backlight_OnOff(PORT_LEVEL_HIGH);
    while(1) {
        St7032_ClearDisplay();

        for(uint8_t i = 0; i < 0xFF; i++) {
            St7032_CursorByPotision(1, 1);
            for(uint8_t j = 0; j < 8; j++) {
                St7032_WriteData(i);
            }
            St7032_ShowDisplayByPotision(2, 1, 0x30);
            St7032_ShowDisplayByPotision(2, 2, 0x78);
            St7032_ShowDisplayByPotision(2, 3, St7032_ConvertHexNumber((i & 0xF0) >> 4));
            St7032_ShowDisplayByPotision(2, 4, St7032_ConvertHexNumber((i & 0x0F)));
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

/*
// Sample
    St7032_CreateOrignalCharacter(0x01, degree, sizeof(degree)/sizeof(uint8_t));

    uint8_t moji1[] = "Hello";
    uint8_t moji2[] = "  World!";
    while(1) {
        St7032_ClearDisplay();
        St7032_ReturnHome();
#ifdef CONFIG_SOFTWARE_USE_BACKLIGHT
        St7032_Backlight_OnOff(PORT_LEVEL_HIGH);
#endif
        for(uint8_t i = 0; i < sizeof(moji1)/sizeof(uint8_t); i++) {
            St7032_WriteData(moji1[i]);
        }
       
        vTaskDelay(pdMS_TO_TICKS(1000));
        St7032_CursorByPotision(2, 1);
        for(uint8_t i = 0; i < sizeof(moji2)/sizeof(uint8_t); i++) {
            St7032_WriteData(moji2[i]);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
        St7032_ClearDisplay();
        vTaskDelay(pdMS_TO_TICKS(1000));
#ifdef CONFIG_SOFTWARE_USE_BACKLIGHT
        St7032_Backlight_OnOff(PORT_LEVEL_LOW);
#endif
        St7032_ShowDisplayByPotision(1, 2, 0x32);
        St7032_ShowDisplayByPotision(1, 3, 0x35);
        St7032_ShowDisplayByPotision(1, 4, 0x2E);
        St7032_ShowDisplayByPotision(1, 5, 0x39);
        St7032_ShowDisplayByPotision(1, 7, 0x01);
        St7032_ShowDisplayByPotision(2, 2, 0x34);
        St7032_ShowDisplayByPotision(2, 3, 0x30);
        St7032_ShowDisplayByPotision(2, 4, 0x2E);
        St7032_ShowDisplayByPotision(2, 5, 0x35);
        St7032_ShowDisplayByPotision(2, 7, 0x25);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
*/
    vTaskDelete(NULL); // Should never get to here...
}
#endif


void app_main() {
    ESP_LOGI(TAG, "app_main() start.");
    esp_log_level_set("*", ESP_LOG_ERROR);
//    esp_log_level_set("wifi", ESP_LOG_INFO);
//    esp_log_level_set("gpio", ESP_LOG_INFO);
    esp_log_level_set("MY-MAIN", ESP_LOG_INFO);
//    esp_log_level_set("MY-UI", ESP_LOG_INFO);
//    esp_log_level_set("MY-WIFI", ESP_LOG_INFO);

    XIAO_Init();

#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
    ui_init();
#endif
#ifdef CONFIG_SOFTWARE_MODEL_SEEED_XIAO_ESP32C3
    initialise_wifi();
#endif


#ifdef CONFIG_SOFTWARE_BUTTON_SUPPORT
    // BUTTON
    xTaskCreatePinnedToCore(&button_task, "button_task", 4096 * 1, NULL, 2, &xButton, 1);
#endif

#ifdef CONFIG_SOFTWARE_UNIT_LED_SUPPORT
    // EXTERNAL LED
    xTaskCreatePinnedToCore(&external_led_task, "external_led_task", 4096 * 1, NULL, 2, &xExternalLED, 1);
#endif

#if CONFIG_SOFTWARE_UNIT_SK6812_SUPPORT
    // EXTERNAL RGB LED BLINK
    xTaskCreatePinnedToCore(&vexternal_LoopRGBLedBlinkTask, "external_rgb_led_blink_task", 4096 * 1, NULL, 2, &xExternalRGBLedBlink, 1);
#endif

#ifdef CONFIG_SOFTWARE_RTC_SUPPORT
    // rtc
    xTaskCreatePinnedToCore(&vLoopRtcTask, "rtc_task", 4096 * 1, NULL, 2, &xRtc, 1);
    // clock
    xTaskCreatePinnedToCore(&vLoopClockTask, "clock_task", 4096 * 1, NULL, 2, &xClock, 1);
#endif


#ifdef CONFIG_SOFTWARE_UNIT_ENV2_SUPPORT
    // UI ACTIVE
    xTaskCreatePinnedToCore(&vLoopUnitEnv2Task, "unit_env2_task", 4096 * 1, NULL, 2, &xUnitEnv2, 1);
#endif

#if ( CONFIG_SOFTWARE_UNIT_4DIGIT_DISPLAY_SUPPORT || CONFIG_SOFTWARE_UNIT_6DIGIT_DISPLAY_SUPPORT )
    // DIGIT DISPLAY
    xTaskCreatePinnedToCore(&vLoopUnitDigitDisplayTask, "vLoopUnitDigitDisplayTask", 4096 * 1, NULL, 2, &xDigitDisplay, 1);
#endif

#ifdef CONFIG_SOFTWARE_I2C_LCD_ST7032_SUPPORT
    // LCD_ST7032
    xTaskCreatePinnedToCore(&vLoopLcdSt7032Task, "lcd_st7032_task", 4096 * 1, NULL, 2, &xLcdSt7032, 1);
#endif

#ifdef CONFIG_SOFTWARE_BUZZER_SUPPORT
    // BUZZER
    xTaskCreatePinnedToCore(&vLoopBuzzerTask, "buzzer_task", 4096 * 1, NULL, 2, &xBuzzer, 1);
#endif

}