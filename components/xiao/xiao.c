#include "stdbool.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/spi_common.h"
#include "esp_idf_version.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"

#include "lvgl.h"
#include "lvgl_helpers.h"
#include "xiao.h"


static const char *TAG = "XIAO";

void XIAO_Init(void) {
    ESP_LOGI(TAG, "XIAO_Init() is start.");

#if CONFIG_SOFTWARE_BUTTON_SUPPORT
    XIAO_Button_Init();
#endif

#if CONFIG_SOFTWARE_SSD1306_SUPPORT
    XIAO_Display_Init();
#endif

#if CONFIG_SOFTWARE_RTC_SUPPORT
    PCF8563_Init();
#endif

}

/* ===================================================================================================*/
/* --------------------------------------------- BUTTON ----------------------------------------------*/
#if CONFIG_SOFTWARE_BUTTON_SUPPORT
void XIAO_Button_Init(void) {
    Button_Init();
}
#endif
/* ----------------------------------------------- End -----------------------------------------------*/
/* ===================================================================================================*/

/* ===================================================================================================*/
/* --------------------------------------------- SK6812 ----------------------------------------------*/
#if CONFIG_SOFTWARE_SK6812_SUPPORT
pixel_settings_t px;

void XIAO_Sk6812_Init(gpio_num_t pin, uint16_t count) {
    px.pixel_count = count;
    px.brightness = 10;
    sprintf(px.color_order, "GRBW");
    px.nbits = 24;
    px.timings.t0h = (350);
    px.timings.t0l = (800);
    px.timings.t1h = (600);
    px.timings.t1l = (700);
    px.timings.reset = 80000;
    px.pixels = (uint8_t *)malloc((px.nbits / 8) * px.pixel_count);
    neopixel_init(pin, RMT_CHANNEL_0);
    np_clear(&px);
}

void XIAO_Sk6812_SetColor(uint16_t pos, uint32_t color) {
    np_set_pixel_color(&px, pos, color << 8);
}

void XIAO_Sk6812_SetAllColor(uint32_t color) {
    for (uint8_t i = 0; i < px.pixel_count; i++) {
        np_set_pixel_color(&px, i, color << 8);
    }
}

void XIAO_Sk6812_SetBrightness(uint8_t brightness) {
    px.brightness = brightness;
}

void XIAO_Sk6812_Show(void) {
    np_show(&px, RMT_CHANNEL_0);
}

void XIAO_Sk6812_Clear(void) {
    np_clear(&px);
}
#endif
/* ----------------------------------------------- End -----------------------------------------------*/
/* ===================================================================================================*/

/* ===================================================================================================*/
/* ---------------------------------------------- Ports ----------------------------------------------*/
#if CONFIG_SOFTWARE_MODEL_SEEED_XIAO_ESP32C3

static esp_err_t check_pins(gpio_num_t pin, pin_mode_t mode){
    esp_err_t err = ESP_ERR_NOT_SUPPORTED;
/*    if (pin != PORT_SDA_PIN && pin != PORT_SCL_PIN)
    {
        ESP_LOGE(TAG, "Only Port (GPIO6(D4)) and GPIO7(D5)) are supported. Pin selected: %d", pin);
    }
    else
*/
    if (mode == I2C && (pin != PORT_SDA_PIN && pin != PORT_SCL_PIN))
    {
        ESP_LOGE(TAG, "I2C is only supported on GPIO6(D4)(SDA) and GPIO7(D5)(SCL).");
    }
/*
    else if (mode == ADC && pin != PORT_ADC_PIN)
    {
        ESP_LOGE(TAG, "ADC is only supported on GPIO 0.");
    }
*/
    else 
    {
        err = ESP_OK;
    }
    return err;
}

esp_err_t XIAO_Port_PinMode(gpio_num_t pin, pin_mode_t mode){
    esp_err_t err = check_pins(pin, mode);
    if (err != ESP_OK){
        ESP_LOGE(TAG, "Invalid mode selected for GPIO %d. Error code: 0x%x.", pin, err);
        return err;
    }

    if (mode == OUTPUT || mode == INPUT){
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
        io_conf.pin_bit_mask = (1ULL << pin);

        if (mode == OUTPUT){
            io_conf.mode = GPIO_MODE_OUTPUT; 
            io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
            io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
            err = gpio_config(&io_conf);
            if (err != ESP_OK){
                ESP_LOGE(TAG, "Error configuring GPIO %d. Error code: 0x%x.", pin, err);
            }
        } else{
            io_conf.mode = GPIO_MODE_INPUT;
            io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
            io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
            err = gpio_config(&io_conf);
            if (err != ESP_OK){
                ESP_LOGE(TAG, "Error configuring GPIO %d. Error code: 0x%x.", pin, err);
            }
        }  
    }
/*
    else if (mode == ADC)
    {
        err = adc1_config_width(ADC_WIDTH);
        if(err != ESP_OK){
            ESP_LOGE(TAG, "Error configuring ADC width on pin %d. Error code: 0x%x.", pin, err);
            return err;
        }
        
        err = adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTENUATION);
        if(err != ESP_OK){
            ESP_LOGE(TAG, "Error configuring ADC channel attenuation on pin %d. Error code: 0x%x.", pin, err);
        }
        
        adc_characterization = calloc(1, sizeof(esp_adc_cal_characteristics_t));
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTENUATION, ADC_WIDTH, DEFAULT_VREF, adc_characterization);
    }
    else if (mode == DAC){
        dac_output_enable(DAC_CHANNEL);
    }
    else if (mode == UART){
        err = uart_driver_install(PORT_C_UART_NUM, UART_RX_BUF_SIZE, 0, 0, NULL, 0);
        if(err != ESP_OK){
            ESP_LOGE(TAG, "UART driver installation failed for UART num %d. Error code: 0x%x.", PORT_C_UART_NUM, err);
            return err;
        }

        err = uart_set_pin(PORT_C_UART_NUM, PORT_C_UART_TX_PIN, PORT_C_UART_RX_PIN, 
        UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        if(err != ESP_OK){
            ESP_LOGE(TAG, "Failed to set pins %d, %d, to  UART%d. Error code: 0x%x.", PORT_C_UART_RX_PIN, PORT_C_UART_TX_PIN, PORT_C_UART_NUM, err);
        }
    }
*/
    else if (mode == NONE){
//        dac_output_disable(DAC_CHANNEL);
        gpio_reset_pin(pin);
        uart_driver_delete(UART_NUM_0);
        i2c_free_port(I2C_NUM_0);
    }

    return err;
}

bool XIAO_Port_Read(gpio_num_t pin){
    ESP_ERROR_CHECK_WITHOUT_ABORT(check_pins(pin, INPUT));

    return gpio_get_level(pin);
}

esp_err_t XIAO_Port_Write(gpio_num_t pin, bool level){
    esp_err_t err = check_pins(pin, OUTPUT);
    
    err = gpio_set_level(pin, level);
    if (err != ESP_OK){
        ESP_LOGE(TAG, "Error setting GPIO %d state. Error code: 0x%x.", pin, err);
    }
    return err;
}

I2CDevice_t XIAO_Port_I2C_Begin(uint8_t device_address, uint32_t baud){
    if (baud < 100000) {
        baud = 100000;
    } else if (400000 < baud) {
        baud = 400000;
    }
    return i2c_malloc_device(I2C_NUM_0, PORT_SDA_PIN, PORT_SCL_PIN, baud, device_address);
}

esp_err_t XIAO_Port_I2C_Read(I2CDevice_t device, uint32_t register_address, uint8_t *data, uint16_t length){
    return i2c_read_bytes(device, register_address, data, length);
}

esp_err_t XIAO_Port_I2C_Write(I2CDevice_t device, uint32_t register_address, uint8_t *data, uint16_t length){
    return i2c_write_bytes(device, register_address, data, length);
}

void XIAO_Port_I2C_Close(I2CDevice_t device){
    i2c_free_device(device);
}

#endif
/* ----------------------------------------------- End -----------------------------------------------*/
/* ===================================================================================================*/

/* ===================================================================================================*/
/* --------------------------------------------- DISPLAY ---------------------------------------------*/
#if CONFIG_SOFTWARE_SSD1306_SUPPORT

#define LV_TICK_PERIOD_MS 1

SemaphoreHandle_t xGuiSemaphore;

static void guiTask(void *pvParameter);
static void lv_tick_task(void *arg);

void XIAO_Display_Init(void) {
    xGuiSemaphore = xSemaphoreCreateMutex();

    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t* buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t* buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820         \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A    \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D     \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    // Initialize the working buffer depending on the selected display
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    xSemaphoreGive(xGuiSemaphore);

    xTaskCreatePinnedToCore(guiTask, "gui", 4096*2, NULL, 2, NULL, 1);
}

static void lv_tick_task(void *arg) {
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void guiTask(void *pvParameter) {
    
    (void) pvParameter;

    while (1) {
        // Delay 1 tick (assumes FreeRTOS tick is 10ms
        vTaskDelay(pdMS_TO_TICKS(10));

        // Try to take the semaphore, call lvgl related function on success
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
       }
    }

    // A task should NEVER return
    vTaskDelete(NULL);
}
#endif
/* ----------------------------------------------- End -----------------------------------------------*/
/* ===================================================================================================*/
