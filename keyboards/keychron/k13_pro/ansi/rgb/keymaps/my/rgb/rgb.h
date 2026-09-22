//----------------------------//
// Основной заголовочный файл //
//----------------------------//

#pragma once
#include QMK_KEYBOARD_H
#include "rgb/config.h"

void rgb_init(void);
void rgb_update_layer(layer_state_t state);
void rgb_update_indicators(void);
void rgb_update_battery(uint8_t level);
void cycle_rgb_modes(void);