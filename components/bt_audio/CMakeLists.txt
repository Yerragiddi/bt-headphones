idf_component_register(
    SRCS "bt_audio.c"
    INCLUDE_DIRS "."
    REQUIRES bt nvs_flash fsm i2s_audio config
)
