#include "buttons.h"
#include "fsm.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "app_config.h"

static const char *TAG = "BUTTONS";

// ── Debounce time ──────────────────────────────────────
#define DEBOUNCE_MS     50
#define LONG_PRESS_MS   1000   // hold duration for power off

// ── Button task ────────────────────────────────────────
static void button_task(void *arg) {
    uint32_t pin = (uint32_t)arg;

    // wait for debounce
    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));

    // measure hold duration
    uint32_t hold_ms = 0;
    while (gpio_get_level(pin) == 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
        hold_ms += 10;
    }

    // dispatch event based on which pin and hold duration
    if (pin == BTN_POWER_PIN && hold_ms >= LONG_PRESS_MS) {
        ESP_LOGI(TAG, "Power button long press");
        fsm_dispatch(EVENT_BTN_POWER);

    } else if (pin == BTN_PAUSE_PIN) {
        ESP_LOGI(TAG, "Pause button press");
        fsm_dispatch(EVENT_BTN_PAUSE);

    } else if (pin == BTN_VOL_UP_PIN) {
        ESP_LOGI(TAG, "Volume up");
        fsm_dispatch(EVENT_BTN_VOL_UP);

    } else if (pin == BTN_VOL_DOWN_PIN) {
        ESP_LOGI(TAG, "Volume down");
        fsm_dispatch(EVENT_BTN_VOL_DOWN);
    }

    vTaskDelete(NULL);
}

// ── ISR handler ────────────────────────────────────────
static void IRAM_ATTR gpio_isr_handler(void *arg) {
    uint32_t pin = (uint32_t)arg;
    // spawn task to handle debounce outside ISR
    xTaskCreate(button_task, "btn_task", 2048, (void *)pin, 10, NULL);
}

// ── Init ───────────────────────────────────────────────
void buttons_init(void) {
    // configure all button pins as input with pull-up
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BTN_POWER_PIN)   |
                        (1ULL << BTN_PAUSE_PIN)    |
                        (1ULL << BTN_VOL_UP_PIN)   |
                        (1ULL << BTN_VOL_DOWN_PIN),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,   // trigger on press (falling edge)
    };
    gpio_config(&io_conf);

    // install ISR service and attach handlers
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_POWER_PIN,    gpio_isr_handler, (void *)BTN_POWER_PIN);
    gpio_isr_handler_add(BTN_PAUSE_PIN,    gpio_isr_handler, (void *)BTN_PAUSE_PIN);
    gpio_isr_handler_add(BTN_VOL_UP_PIN,   gpio_isr_handler, (void *)BTN_VOL_UP_PIN);
    gpio_isr_handler_add(BTN_VOL_DOWN_PIN, gpio_isr_handler, (void *)BTN_VOL_DOWN_PIN);

    ESP_LOGI(TAG, "Buttons initialized");
}
