#include "power.h"
#include "fsm.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "app_config.h"

static const char *TAG = "POWER";

// ── Idle timeout (ms) ──────────────────────────────────
#define IDLE_TIMEOUT_MS  30000   // 30 seconds in IDLE → sleep

// ── Power monitor task ─────────────────────────────────
static void power_task(void *arg) {
    uint32_t idle_ms = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        headphone_state_t state = fsm_get_state();

        if (state == STATE_IDLE) {
            idle_ms += 1000;
            ESP_LOGI(TAG, "Idle for %lu ms", idle_ms);

            if (idle_ms >= IDLE_TIMEOUT_MS) {
                ESP_LOGI(TAG, "Idle timeout reached");
                fsm_dispatch(EVENT_IDLE_TIMEOUT);
                power_enter_deep_sleep();
            }
        } else if (state == STATE_POWER_OFF) {
            power_enter_deep_sleep();
        } else {
            // reset idle timer when active
            idle_ms = 0;
        }
    }
}

// ── Deep sleep ─────────────────────────────────────────
void power_enter_deep_sleep(void) {
    ESP_LOGI(TAG, "Entering deep sleep, wake on power button");

    // wake up when power button is pressed (LOW signal)
    esp_sleep_enable_ext0_wakeup(BTN_POWER_PIN, 0);

    esp_deep_sleep_start();
}

// ── Init ───────────────────────────────────────────────
void power_init(void) {
    // check if we woke from deep sleep
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_causes();
    if (cause == ESP_SLEEP_WAKEUP_EXT0) {
        ESP_LOGI(TAG, "Woke up from deep sleep via power button");
    }

    // start power monitor task
    xTaskCreate(power_task, "power_task", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "Power management initialized");
}
