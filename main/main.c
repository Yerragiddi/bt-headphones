#include "fsm.h"
#include "bt_audio.h"
#include "i2s_audio.h"
#include "buttons.h"
#include "led.h"
#include "power.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Booting ESP32 Headphones...");

    fsm_init();
    i2s_audio_init();
    bt_audio_init();
    buttons_init();
    led_init();
    power_init();

    ESP_LOGI(TAG, "All systems go!");
}
