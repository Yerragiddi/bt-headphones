#pragma once

#include <stdint.h>

void i2s_audio_init(void);
void i2s_audio_write(const uint8_t *data, uint32_t len);
void i2s_audio_stop(void);
