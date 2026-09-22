//----------------------------//
//      Индикация слоёв       //
//----------------------------//

#include "rgb.h"

void rgb_update_layer(layer_state_t state) {
    if (get_highest_layer(state) == 0) return;
    
    // Базовый цвет для всех клавиш слоя
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            uint8_t index = g_led_config.matrix_co[row][col];
            if (index != NO_LED && keymap_key_to_keycode(layer, (keypos_t){col,row}) > KC_TRNS) {
                rgb_matrix_set_color(index, 98, 255, 187);
            }
        }
    }

    // Кастомные индикаторы
    rgb_matrix_set_color(LED_INS, 255, 0, 0);
    rgb_matrix_set_color(LED_SPACE, 0, 0, 0);
    const uint8_t wasd_leds[] = {LED_W, LED_A, LED_S, LED_D};
    for (uint8_t i = 0; i < ARRAY_SIZE(wasd_leds); i++) {
        rgb_matrix_set_color(wasd_leds[i], 255, 130, 168);
    }
}