#include "rgb_effects.h"
// #include "rgb_effects_private.h" // Создайте этот файл для приватных структур

static led_mode_t current_led_mode = LAYER_MODE;
static uint16_t last_num_key = KC_NO;
static bool blink_state = false;

// Цвета слоёв
const rgblight_segment_t PROGMEM MAC[] = RGBLIGHT_LAYER_SEGMENTS({0, 16, HSV_RED});
const rgblight_segment_t PROGMEM MAC_FN[] = RGBLIGHT_LAYER_SEGMENTS({0, 16, HSV_GREEN});
const rgblight_segment_t PROGMEM WIN[] = RGBLIGHT_LAYER_SEGMENTS({0, 16, HSV_BLUE});
const rgblight_segment_t PROGMEM WIN_FN[] = RGBLIGHT_LAYER_SEGMENTS({0, 16, HSV_WHITE});

void rgb_effects_init(void) {
    const rgblight_segment_t* const PROGMEM layers[] = {
        MAC, MAC_FN, WIN, WIN_FN, NULL
    };
    rgblight_layers = layers;
}

layer_state_t rgb_effects_layer_state(layer_state_t state) {
    if (current_led_mode == LAYER_MODE) {
        for (uint8_t i = 0; i < 4; i++) {
            rgblight_set_layer_state(i, layer_state_cmp(state, i));
        }
    }
    return state;
}

// Остальные функции из предыдущего примера (убраны для краткости)