//------------------------//
// Caps/Num Lock, батарея //
//------------------------//


#include "rgb.h"

void rgb_update_indicators(void) {
    // CapsLock
    if (host_keyboard_led_state().caps_lock) {
        const uint8_t caps_leds[] = {LED_CAPS, LED_W, LED_A, LED_S, LED_D};
        for (uint8_t i = 0; i < ARRAY_SIZE(caps_leds); i++) {
            rgb_matrix_set_color(caps_leds[i], 255, 0, 0);
        }
    }

    // NumLock
    if (host_keyboard_led_state().num_lock) {
        const uint8_t numlock_leds[] = {LED_NUMLOCK, 13, 14, 15, 30, 31, 32};
        for (uint8_t i = 0; i < ARRAY_SIZE(numlock_leds); i++) {
            rgb_matrix_set_color(numlock_leds[i], 0, 255, 0);
        }
    }
}

void rgb_update_battery(uint8_t level) {
    // Реализуйте индикацию батареи
    // Например, градиент от зелёного к красному
    uint8_t hue = (uint8_t)(level * 85 / 100); // 0-85 (0=red, 85=green)
    rgb_matrix_set_color(LED_BATTERY, hue, 255, 255);
}