#include "led.h"
#include "fsm.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "app_config.h"

static const char *TAG = "LED";

// ── LED blink task ─────────────────────────────────────
static void led_task(void *arg) {
    while (1) {
        headphone_state_t state = fsm_get_state();

        switch (state) {
            case STATE_IDLE:
                // slow blink — searching for device
                gpio_set_level(LED_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(200));
                gpio_set_level(LED_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(1800));
                break;

            case STATE_CONNECTED:
                // fast blink — connected, no audio
                gpio_set_level(LED_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(200));
                gpio_set_level(LED_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(200));
                break;

            case STATE_PLAYING:
                // solid on — playing
                gpio_set_level(LED_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(500));
                break;

            case STATE_PAUSED:
                // double blink — paused
                gpio_set_level(LED_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(LED_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(LED_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_set_level(LED_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(700));
                break;

            case STATE_POWER_OFF:
                // off
                gpio_set_level(LED_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
                break;
        }
    }
}

// ── Init ───────────────────────────────────────────────
void led_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // start LED task
    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "LED initialized");
}

void led_update(void) {
    // nothing needed — led_task reads FSM state automatically
}
