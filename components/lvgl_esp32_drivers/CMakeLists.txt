if(ESP_PLATFORM)

file(GLOB SOURCES *.c)
set(LVGL_INCLUDE_DIRS . lvgl_tft)
list(APPEND SOURCES "lvgl_tft/disp_driver.c" "lvgl_tft/disp_spi.c")

#@todo add SimleInclude macro here

# Include only the source file of the selected
# display controller.
if(CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306)
    list(APPEND SOURCES "lvgl_tft/ssd1306.c")
else()
    message(WARNING "LVGL ESP32 drivers: Display controller not defined.")
endif()

if(CONFIG_LV_TFT_DISPLAY_CONTROLLER_ILI9341)
    list(APPEND SOURCES "lvgl_tft/ili9341.c")
else()
    message(WARNING "LVGL ESP32 drivers: Display controller not defined.")
endif()

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ${LVGL_INCLUDE_DIRS}
                       REQUIRES lvgl)
                       
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_LVGL_H_INCLUDE_SIMPLE")

else()
    message(FATAL_ERROR "LVGL ESP32 drivers: ESP_PLATFORM is not defined. Try reinstalling ESP-IDF.")
endif()
