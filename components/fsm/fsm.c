#include "fsm.h"
#include "esp_log.h"

static const char *TAG = "FSM";
static headphone_state_t current_state = STATE_IDLE;

void fsm_init(void) {
    current_state = STATE_IDLE;
    ESP_LOGI(TAG, "FSM initialized, state: IDLE");
}

headphone_state_t fsm_get_state(void) {
    return current_state;
}

void fsm_dispatch(headphone_event_t event) {
    switch (current_state) {

        case STATE_IDLE:
            if (event == EVENT_BT_CONNECTED) {
                current_state = STATE_CONNECTED;
                ESP_LOGI(TAG, "IDLE → CONNECTED");
            } else if (event == EVENT_BTN_POWER) {
                current_state = STATE_POWER_OFF;
                ESP_LOGI(TAG, "IDLE → POWER_OFF");
            } else if (event == EVENT_IDLE_TIMEOUT) {
                current_state = STATE_POWER_OFF;
                ESP_LOGI(TAG, "IDLE → POWER_OFF (timeout)");
            }
            break;

        case STATE_CONNECTED:
            if (event == EVENT_AUDIO_START) {
                current_state = STATE_PLAYING;
                ESP_LOGI(TAG, "CONNECTED → PLAYING");
            } else if (event == EVENT_BT_DISCONNECTED) {
                current_state = STATE_IDLE;
                ESP_LOGI(TAG, "CONNECTED → IDLE");
            } else if (event == EVENT_BTN_POWER) {
                current_state = STATE_POWER_OFF;
                ESP_LOGI(TAG, "CONNECTED → POWER_OFF");
            }
            break;

        case STATE_PLAYING:
            if (event == EVENT_BTN_PAUSE) {
                current_state = STATE_PAUSED;
                ESP_LOGI(TAG, "PLAYING → PAUSED");
            } else if (event == EVENT_AUDIO_STOP) {
                current_state = STATE_CONNECTED;
                ESP_LOGI(TAG, "PLAYING → CONNECTED");
            } else if (event == EVENT_BT_DISCONNECTED) {
                current_state = STATE_IDLE;
                ESP_LOGI(TAG, "PLAYING → IDLE");
            } else if (event == EVENT_BTN_POWER) {
                current_state = STATE_POWER_OFF;
                ESP_LOGI(TAG, "PLAYING → POWER_OFF");
            }
            break;

        case STATE_PAUSED:
            if (event == EVENT_BTN_PAUSE) {
                current_state = STATE_PLAYING;
                ESP_LOGI(TAG, "PAUSED → PLAYING");
            } else if (event == EVENT_BT_DISCONNECTED) {
                current_state = STATE_IDLE;
                ESP_LOGI(TAG, "PAUSED → IDLE");
            } else if (event == EVENT_BTN_POWER) {
                current_state = STATE_POWER_OFF;
                ESP_LOGI(TAG, "PAUSED → POWER_OFF");
            }
            break;

        case STATE_POWER_OFF:
            // handled by power component, deep sleep here
            ESP_LOGI(TAG, "Entering deep sleep...");
            break;
    }
}
