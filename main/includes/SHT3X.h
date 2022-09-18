// https://github.com/m5stack/M5Unit-ENV/blob/master/src/SHT3X.h
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "esp_err.h"

esp_err_t Sht3x_Init();
esp_err_t Sht3x_Read();
float Sht3x_GetTemperature();
int32_t Sht3x_GetIntTemperature();
float Sht3x_GetHumidity();
int32_t Sht3x_GetIntHumidity();

#ifdef __cplusplus
}
#endif
