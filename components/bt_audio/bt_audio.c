#include "bt_audio.h"
#include "i2s_audio.h"
#include "fsm.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "nvs_flash.h"
#include "app_config.h"
#include "esp_gap_bt_api.h"

static const char *TAG = "BT_AUDIO";

// ── A2DP data callback ─────────────────────────────────
// called every time audio data arrives from phone
static void bt_data_cb(const uint8_t *data, uint32_t len) {
    i2s_audio_write(data, len);
}
// ── A2DP event callback ────────────────────────────────
static void bt_a2dp_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param) {
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
                ESP_LOGI(TAG, "Phone connected");
                fsm_dispatch(EVENT_BT_CONNECTED);
            } else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                ESP_LOGI(TAG, "Phone disconnected");
                fsm_dispatch(EVENT_BT_DISCONNECTED);
            }
            break;

        case ESP_A2D_AUDIO_STATE_EVT:
            if (param->audio_stat.state == ESP_A2D_AUDIO_STATE_STARTED) {
                ESP_LOGI(TAG, "Audio started");
                fsm_dispatch(EVENT_AUDIO_START);
            } else if (param->audio_stat.state == ESP_A2D_AUDIO_STATE_SUSPEND) {
                ESP_LOGI(TAG, "Audio stopped");
                fsm_dispatch(EVENT_AUDIO_STOP);
                
            }
            break;

        default:
            break;
    }
}

// ── Init ───────────────────────────────────────────────
void bt_audio_init(void) {
    // init NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // release BLE memory — must be done BEFORE controller init

    // init BT controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BTDM));

    // init bluedroid
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    // set device name
    esp_bt_gap_set_device_name(BT_DEVICE_NAME);

    // init A2DP sink
    esp_a2d_sink_init();
    esp_a2d_register_callback(bt_a2dp_cb);
    esp_a2d_sink_register_data_callback(bt_data_cb);

    // make discoverable
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

    ESP_LOGI(TAG, "Bluetooth started: %s", BT_DEVICE_NAME);
}
