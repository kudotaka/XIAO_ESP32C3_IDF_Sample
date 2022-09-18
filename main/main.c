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

#ifdef CONFIG_SOFTWARE_I2C_UNIT_ENV2_SUPPORT
#include "SHT3X.h"
#endif


static const char *TAG = "MAIN";


#ifdef CONFIG_SOFTWARE_BUTTON_SUPPORT
TaskHandle_t xButton;
Button_t* button_d2_pin;
Button_t* button_d0_pin;

static void button_task(void* pvParameters) {
    ESP_LOGI(TAG, "start button_task");

    if (Button_Enable(PORT_D2_PIN) == ESP_OK) {
        button_d2_pin = Button_Attach(PORT_D2_PIN);
    }
    if (Button_Enable(PORT_D0_PIN) == ESP_OK) {
        button_d0_pin = Button_Attach(PORT_D0_PIN);
    }

    while(1){
        if (Button_WasPressed(button_d2_pin)) {
            ESP_LOGI(TAG, "PORT_D2_PIN BUTTON PRESSED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(true);
#endif
        }
        if (Button_WasReleased(button_d2_pin)) {
            ESP_LOGI(TAG, "PORT_D2_PIN BUTTON RELEASED!");
#ifdef CONFIG_SOFTWARE_SSD1306_SUPPORT
            ui_button_label_update(false);
#endif
        }
        if (Button_WasLongPress(button_d2_pin, pdMS_TO_TICKS(1000))) { // 1Sec
            ESP_LOGI(TAG, "PORT_D2_PIN BUTTON LONGPRESS!");
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

#ifdef CONFIG_SOFTWARE_EXTERNAL_LED_SUPPORT
TaskHandle_t xExtLED;
static void external_led_task(void* pvParameters) {
    ESP_LOGI(TAG, "start external_led_task");

    if (XIAO_Port_PinMode(PORT_D3_PIN, OUTPUT) != ESP_OK) {
        ESP_LOGI(TAG, "XIAO_Port_PinMode() is error. port:%d", PORT_D3_PIN);
    } else {
        vTaskDelay(pdMS_TO_TICKS(100));
        while (1) {
            if (XIAO_Port_Write(PORT_D3_PIN, PORT_LEVEL_LOW) != ESP_OK) {
                ESP_LOGE(TAG, "XIAO_Port_Write(LOW) is error. port:%d", PORT_D3_PIN);
                break;
            }
//            ESP_LOGI(TAG, "XIAO_Port_Write(LOW) is OFF");
            vTaskDelay(pdMS_TO_TICKS(800));

            if (XIAO_Port_Write(PORT_D3_PIN, PORT_LEVEL_HIGH) != ESP_OK) {
                ESP_LOGE(TAG, "XIAO_Port_Write(HIGH) is error. port:%d", PORT_D3_PIN);
                break;
            }
//            ESP_LOGI(TAG, "XIAO_Port_Write(HIGH) is ON");
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
    vTaskDelete(NULL); // Should never get to here...
}
#endif

#ifdef CONFIG_SOFTWARE_SK6812_SUPPORT
TaskHandle_t xRGBLedBlink;
void vLoopRGBLedBlinkTask(void *pvParametes)
{
    ESP_LOGI(TAG, "start vLoopRGBLedBlinkTask");
    uint8_t blink_count = 5;
    uint32_t colors[] = {SK6812_COLOR_BLUE, SK6812_COLOR_LIME, SK6812_COLOR_AQUA
                    , SK6812_COLOR_RED, SK6812_COLOR_MAGENTA, SK6812_COLOR_YELLOW
                    , SK6812_COLOR_WHITE};

    while (1) {
        for (uint8_t c = 0; c < sizeof(colors)/sizeof(uint32_t); c++) {
            for (uint8_t i = 0; i < blink_count; i++) {
                XIAO_Sk6812_SetAllColor(colors[c]);
                XIAO_Sk6812_Show();
                vTaskDelay(pdMS_TO_TICKS(1000));

                XIAO_Sk6812_SetAllColor(SK6812_COLOR_OFF);
                XIAO_Sk6812_Show();
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }
}
#endif

#ifdef CONFIG_SOFTWARE_I2C_UNIT_ENV2_SUPPORT
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


void app_main() {
    ESP_LOGI(TAG, "app_main() start.");

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


#ifdef CONFIG_SOFTWARE_EXTERNAL_LED_SUPPORT
    // External LED
    xTaskCreatePinnedToCore(&external_led_task, "external_led_task", 4096 * 1, NULL, 2, &xExtLED, 1);
#endif


#ifdef CONFIG_SOFTWARE_SK6812_SUPPORT
    // RGB LED BLINK
    uint16_t led_count = 1;
#if CONFIG_SOFTWARE_MODEL_NEOHEX_37
    led_count = 37;
#elif CONFIG_SOFTWARE_MODEL_RGBUNIT_3
    led_count = 3;
#else
    led_count = 1;
#endif
    XIAO_Sk6812_Init(PORT_D1_PIN, led_count);
    xTaskCreatePinnedToCore(&vLoopRGBLedBlinkTask, "rgb_led_blink_task", 4096 * 1, NULL, 2, &xRGBLedBlink, 1);
#endif


#ifdef CONFIG_SOFTWARE_RTC_SUPPORT
    // rtc
    xTaskCreatePinnedToCore(&vLoopRtcTask, "rtc_task", 4096 * 1, NULL, 2, &xRtc, 1);
    // clock
    xTaskCreatePinnedToCore(&vLoopClockTask, "clock_task", 4096 * 1, NULL, 2, &xClock, 1);
#endif


#ifdef CONFIG_SOFTWARE_I2C_UNIT_ENV2_SUPPORT
    // UI ACTIVE
    xTaskCreatePinnedToCore(&vLoopUnitEnv2Task, "unit_env2_task", 4096 * 1, NULL, 2, &xUnitEnv2, 1);
#endif

}