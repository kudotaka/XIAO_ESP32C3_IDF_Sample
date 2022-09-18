// https://github.com/m5stack/M5Unit-ENV/blob/master/src/SHT3X.cpp
#include "freertos/FreeRTOS.h"
#include "SHT3X.h"
#include "xiao.h"

#define SHT3X_ADDR (0x44)

#define Sht3x_COMMAND_MEASURE {0x2C, 0x06}
static I2CDevice_t sht3x_device;
static float temperature = 0.0;
static float humidity = 0.0;

float Sht3x_GetTemperature() {
    return temperature;
}

int32_t Sht3x_GetIntTemperature() {
    return (int32_t)temperature;
}

float Sht3x_GetHumidity() {
    return humidity;
}

int32_t Sht3x_GetIntHumidity() {
    return (int32_t)humidity;
}

esp_err_t Sht3x_Read() {
    esp_err_t ret = ESP_OK;
//    uint8_t cmd[2] = {0};
    uint8_t cmd[2] = Sht3x_COMMAND_MEASURE;
    uint8_t tempdata[6] = {0};

//    cmd[0] = 0x2C;
//    cmd[1] = 0x06;

    ret = XIAO_Port_I2C_Write(sht3x_device, (uint32_t)I2C_NO_REG, cmd, (uint16_t)2);
    vTaskDelay(100/portTICK_RATE_MS);

    if (ret == ESP_OK) {
        ret = XIAO_Port_I2C_Read(sht3x_device, (uint32_t)I2C_NO_REG, tempdata, (uint16_t)6);
        vTaskDelay(10);

        humidity = ((((tempdata[3] * 256.0) + tempdata[4]) * 100) / 65535.0);
        temperature = ((((tempdata[0] * 256.0) + tempdata[1]) * 175) / 65535.0) - 45;
    }

    return ret;
}

esp_err_t Sht3x_Init(void) {
    sht3x_device = XIAO_Port_I2C_Begin((uint8_t)SHT3X_ADDR, (uint32_t)PORT_I2C_STANDARD_BAUD);
    if (sht3x_device == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
}