#pragma once

#include "quantum.h"


typedef enum {
    LAYER_MODE,
    FINGER_ZONE_MODE,
    LAST_KEY_BLINK_MODE
} led_mode_t;

// Инициализация
void rgb_effects_init(void);

// Обработка слоёв
layer_state_t rgb_effects_layer_state(layer_state_t state);

// Обработка режимов
void rgb_effects_task(void);
bool rgb_effects_process_record(uint16_t keycode, keyrecord_t* record);