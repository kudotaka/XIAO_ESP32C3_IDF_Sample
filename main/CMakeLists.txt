set(COMPONENT_REQUIRES "xiao" "lvgl" "lvgl_esp32_drivers")

idf_component_register(SRCS "main.c" "ui.c" "wifi.c" "SHT3X.c" INCLUDE_DIRS "includes")
