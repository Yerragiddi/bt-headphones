idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES fsm bt_audio i2s_audio buttons led power config
)
