#include "i2s_audio.h"
#include "driver/i2s_std.h"
#include "esp_log.h"
#include "app_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"

static const char *TAG = "I2S";
static i2s_chan_handle_t tx_handle = NULL;
static RingbufHandle_t ringbuf = NULL;

// ── I2S writer task ───────────────────────────────────
static void i2s_task(void *arg) {
    size_t bytes_written = 0;
    size_t item_size = 0;

    while (1) {
        uint8_t *data = (uint8_t *)xRingbufferReceive(ringbuf, &item_size, portMAX_DELAY);
        if (data != NULL) {
            i2s_channel_write(tx_handle, data, item_size, &bytes_written, portMAX_DELAY);
            vRingbufferReturnItem(ringbuf, data);
        }
    }
}

void i2s_audio_init(void) {
    // create ring buffer — 16KB
    ringbuf = xRingbufferCreate(1024 * 16, RINGBUF_TYPE_BYTEBUF);

    // channel config
    i2s_chan_config_t chan_cfg = {
        .id = I2S_NUM_0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 16,
        .dma_frame_num = 1024,
        .auto_clear = true,
    };

    i2s_new_channel(&chan_cfg, &tx_handle, NULL);

    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(
                        I2S_DATA_BIT_WIDTH_16BIT,
                        I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_BCLK_PIN,
            .ws   = I2S_LRC_PIN,
            .dout = I2S_DOUT_PIN,
            .din  = I2S_GPIO_UNUSED,
        },
    };

    i2s_channel_init_std_mode(tx_handle, &std_cfg);
    i2s_channel_enable(tx_handle);

    // start I2S writer task
    xTaskCreate(i2s_task, "i2s_task", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG, "I2S initialized");
}

void i2s_audio_write(const uint8_t *data, uint32_t len) {
    if (ringbuf != NULL) {
        xRingbufferSend(ringbuf, data, len, pdMS_TO_TICKS(10));
    }
}

void i2s_audio_stop(void) {
    ESP_LOGI(TAG, "I2S stopped");
}
