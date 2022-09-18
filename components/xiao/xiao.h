#pragma once
#include "freertos/FreeRTOS.h"

#if CONFIG_SOFTWARE_SK6812_SUPPORT
#include "sk6812.h"
#endif

#if CONFIG_SOFTWARE_BUTTON_SUPPORT
#include "button.h"
#endif

#if CONFIG_SOFTWARE_RTC_SUPPORT
#include "pcf8563.h"
#endif

#if CONFIG_SOFTWARE_SSD1306_SUPPORT
#include "freertos/semphr.h"
#include "lvgl.h"
#include "disp_driver.h"
//#include "disp_spi.h"

extern SemaphoreHandle_t xGuiSemaphore;
#endif

#if CONFIG_SOFTWARE_MODEL_SEEED_XIAO_ESP32C3
#include "driver/gpio.h"
#include "driver/uart.h"
#include "i2c_device.h"

// XIAO ESP32C3
#define PORT_D0_PIN GPIO_NUM_2
#define PORT_D1_PIN GPIO_NUM_3
#define PORT_D2_PIN GPIO_NUM_4
#define PORT_D3_PIN GPIO_NUM_5
#define PORT_D4_PIN GPIO_NUM_6
#define PORT_D5_PIN GPIO_NUM_7
#define PORT_D6_PIN GPIO_NUM_21
#define PORT_D7_PIN GPIO_NUM_20
#define PORT_D8_PIN GPIO_NUM_8
#define PORT_D9_PIN GPIO_NUM_9
#define PORT_D10_PIN GPIO_NUM_10
#define PORT_A0_PIN GPIO_NUM_2
#define PORT_A1_PIN GPIO_NUM_3
#define PORT_A2_PIN GPIO_NUM_4
#define PORT_A3_PIN GPIO_NUM_5
#define PORT_SDA_PIN GPIO_NUM_6
#define PORT_SCL_PIN GPIO_NUM_7
#define PORT_TX_PIN GPIO_NUM_21
#define PORT_RX_PIN GPIO_NUM_20
#define PORT_SCK_PIN GPIO_NUM_8
#define PORT_MISO_PIN GPIO_NUM_9
#define PORT_MOSI_PIN GPIO_NUM_10

#define PORT_I2C_STANDARD_BAUD 400000

#define PORT_LEVEL_HIGH 1
#define PORT_LEVEL_LOW  0

typedef enum {
    NONE,   //Reset GPIO to default state.
    OUTPUT, //Set GPIO to output mode.
    INPUT,  //Set GPIO to input mode.
    I2C,    //Enable I2C mode. Only available on Port A—GPIO XX(SDA) and Port A—GPIO XX (SCL).
    ADC,    //Enable ADC mode. Only available on Port B—GPIO XX
    DAC,    //Enable DAC mode. Only available on Port B—GPIO XX
    UART    //Enable UART RX/TX mode. UART TX only available on Port 
                    // C—GPIO 14 and UART RX is only available on Port C—GPIO 
                    // 13. Only supports full-duplex UART so setting one pin 
                    // to UART mode will also set the other pin to UART mode.
} pin_mode_t;

#endif


void XIAO_Init(void);

#if CONFIG_SOFTWARE_BUTTON_SUPPORT
void XIAO_Button_Init(void);
#endif

#if CONFIG_SOFTWARE_SK6812_SUPPORT
#define SK6812_COLOR_OFF     0x000000
#define SK6812_COLOR_BLACK   0x000000
#define SK6812_COLOR_BLUE    0x0000FF
#define SK6812_COLOR_LIME    0x00FF00
#define SK6812_COLOR_AQUA    0x00FFFF
#define SK6812_COLOR_RED     0xFF0000
#define SK6812_COLOR_MAGENTA 0xFF00FF
#define SK6812_COLOR_YELLOW  0xFFFF00
#define SK6812_COLOR_WHITE   0xFFFFFF

void XIAO_Sk6812_Init(gpio_num_t pin, uint16_t count);
void XIAO_Sk6812_SetColor(uint16_t pos, uint32_t color);
void XIAO_Sk6812_SetAllColor(uint32_t color);
void XIAO_Sk6812_SetBrightness(uint8_t brightness);
void XIAO_Sk6812_Show(void);
void XIAO_Sk6812_Clear(void);
#endif

#if CONFIG_SOFTWARE_SSD1306_SUPPORT
void XIAO_Display_Init(void);
#endif

#if CONFIG_SOFTWARE_MODEL_SEEED_XIAO_ESP32C3
esp_err_t XIAO_Port_PinMode(gpio_num_t pin, pin_mode_t mode);
bool XIAO_Port_Read(gpio_num_t pin);
esp_err_t XIAO_Port_Write(gpio_num_t pin, bool level);
I2CDevice_t XIAO_Port_I2C_Begin(uint8_t device_address, uint32_t baud);
esp_err_t XIAO_Port_I2C_Read(I2CDevice_t device, uint32_t register_address, uint8_t *data, uint16_t length);
esp_err_t XIAO_Port_I2C_Write(I2CDevice_t device, uint32_t register_address, uint8_t *data, uint16_t length);
void XIAO_Port_I2C_Close(I2CDevice_t device);
uint32_t XIAO_Port_ADC_ReadRaw(void);
uint32_t XIAO_Port_ADC_ReadMilliVolts(void);
#endif
