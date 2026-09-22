
#include "tap_dance.h"
#include "action_layer.h" // Для работы со слоями



// ==================================================
// Функция обработки событий Tap Dance
// ==================================================

void td_finished(tap_dance_state_t *state, void *user_data) {
    td_ctx_t *ctx = (td_ctx_t *)user_data;
    
	 // Если клавиша зажата (удержание)
	
    if (state->pressed) {
        ctx->is_holding = true; // Устанавливаем флаг удержания
        layer_on(ctx->layer);	// Включаем слой
    } else {    			    // Если клавиша отпущена (тап)
        switch(state->count) {
            case 1:				// Одиночный тап
                if (!ctx->is_holding) // Если не было удержания
					tap_code16(ctx->tap_key); // Отправляем клавишу
                break;
            case 2:				// Двойной тап
                layer_invert(ctx->layer);	// Переключаем слой (вкл/выкл)
                break;
//			case 3:				// Действие для тройного тапа
//				break;
    
        }
        ctx->is_holding = false;	// Сбрасываем флаг
    }
}

// ==================================================
// Хардкод для простых сценариев. Каждой клавише свой набор функций.
// ==================================================

void td_t_grv_finished(tap_dance_state_t *state, void *user_data) {
    switch(state->count) {
        case 1:  // Одиночный тап
            tap_code16(KC_T);
            break;
        case 2:  // Двойной тап
            tap_code16(KC_GRV);
            break;
/*
		case 3:	// Тройной тап
		tap_code16(KC_XXX);
	    break; 
*/
    }
}
void td_vmute_finished(tap_dance_state_t *state, void *user_data) {
    switch(state->count) {
        case 1:  // Одиночный тап
            tap_code16(KC_VOLD);
            break;
        case 2:  // Двойной тап
            tap_code16(KC_MUTE);
            break;
    }
}

void td_pppn_finished(tap_dance_state_t *state, void *user_data) {
    switch(state->count) {
        case 1:  // Одиночный тап
            tap_code16(KC_MPLY);
            break;
        case 2:  // Двойной тап
            tap_code16(KC_MNXT);
            break;
		case 3:	// Тройной тап
			tap_code16(KC_MPRV);
			break; 

    }
}

void td_t_grv_reset(tap_dance_state_t *state, void *user_data) {
	//Пустая функция, но обязательная
}
void td_vmute_reset(tap_dance_state_t *state, void *user_data) {}
void td_pppn_reset(tap_dance_state_t *state, void *user_data) {}

// ==================================================
// Функция сброса состояния
// ==================================================

void td_reset(tap_dance_state_t *state, void *user_data) {
    td_ctx_t *ctx = (td_ctx_t *)user_data;
     // Если было удержание
    if (ctx->is_holding) {
        layer_off(ctx->layer);	 // Выключаем слой
        ctx->is_holding = false; // Сбрасываем флаг
    }
}