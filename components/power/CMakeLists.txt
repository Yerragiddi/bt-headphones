idf_component_register(
    SRCS "power.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_driver_gpio fsm config
)
