idf_component_register(
    SRCS "buttons.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_driver_gpio fsm config
)
