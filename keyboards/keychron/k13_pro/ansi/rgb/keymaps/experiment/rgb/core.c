//-----------------------//
// Базовые настройки RGB //
//-----------------------//

#include "rgb.h"

void rgb_init(void) {
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv(18, 199, 255);
}

__attribute__((weak)) void rgb_update_custom(void) {}