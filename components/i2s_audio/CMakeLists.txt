idf_component_register(
    SRCS "i2s_audio.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_driver_i2s config
)
