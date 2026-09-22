//------------------------//
//  Декоративные эффекты  //
//------------------------//


#include "rgb.h"

static uint8_t current_mode = 0;
const uint8_t RGB_MODE_COUNT = 3;

void cycle_rgb_modes(void) {
    current_mode = (current_mode + 1) % RGB_MODE_COUNT;
    
    switch(current_mode) {
        case 0: // Стандартный режим
            rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            break;
            
        case 1: // Градиент
            rgb_matrix_mode(RGB_MATRIX_GRADIENT_LEFT_RIGHT);
            rgb_matrix_sethsv(18, 199, 255);
            break;
            
        case 2: // Реактивный эффект
            rgb_matrix_mode(RGB_MATRIX_TYPING_HEATMAP);
            break;
    }
}