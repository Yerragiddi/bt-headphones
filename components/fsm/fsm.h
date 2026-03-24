#pragma once

// ── States ───────────────────────────────────────────
typedef enum {
    STATE_IDLE,         // BT on, searching for device
    STATE_CONNECTED,    // phone connected, no audio yet
    STATE_PLAYING,      // audio streaming
    STATE_PAUSED,       // connected but paused
    STATE_POWER_OFF,    // deep sleep
} headphone_state_t;

// ── Events ───────────────────────────────────────────
typedef enum {
    EVENT_BT_CONNECTED,       // phone connected
    EVENT_BT_DISCONNECTED,    // phone disconnected
    EVENT_AUDIO_START,        // music started playing
    EVENT_AUDIO_STOP,         // music stopped
    EVENT_BTN_PAUSE,          // pause/play button pressed
    EVENT_BTN_POWER,          // power button held
    EVENT_BTN_VOL_UP,         // volume up
    EVENT_BTN_VOL_DOWN,       // volume down
    EVENT_IDLE_TIMEOUT,       // idle too long → sleep
} headphone_event_t;

// ── Public API ────────────────────────────────────────
void fsm_init(void);
void fsm_dispatch(headphone_event_t event);
headphone_state_t fsm_get_state(void);
