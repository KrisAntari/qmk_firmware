
#include "tap_dance.h"
#include "action_layer.h" // Для работы со слоями

// ==================================================
// Функция обработки событий Tap Dance
// ==================================================


void td_finished(tap_dance_state_t *state, void *user_data) {
    td_ctx_t *ctx = (td_ctx_t *)user_data;

    switch(state->count) {
        case 1:
            if (state->pressed) {
                // Удержание: регистрируем клавишу без блокировки
                ctx->is_active = true;
                register_code(ctx->tap_key);
            } else {
                // Одиночный тап
                tap_code(ctx->tap_key);
            }
            break;

        case 2:
            if (state->pressed) {
                // Двойное нажатие + удержание
                layer_on(ctx->fn1_layer);
                ctx->is_active = true;
            } else {
                // Двойной тап
                layer_invert(ctx->fn1_layer);
            }
            break;

    }
}

void td_reset(tap_dance_state_t *state, void *user_data) {
    td_ctx_t *ctx = (td_ctx_t *)user_data;

    if (ctx->is_active) {
        // Гарантированный сброс состояний
        unregister_code(ctx->tap_key);
        layer_off(ctx->fn1_layer);
        ctx->is_active = false;
    }
}


// ==================================================
// Хардкод для простых сценариев. Каждой клавише свой набор функций.
// ==================================================

void td_t_grv_finished(tap_dance_state_t *state, void *user_data) {
    switch(state->count) {
        case 1:  // Одиночный тап
            tap_code16(KC_GRV);
            break;
        case 2:  // Двойной тап
            tap_code16(KC_ESC);
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

void td_lralt_finished(tap_dance_state_t *state, void *user_data) {
	    switch(state->count) {
        case 1:  // Одиночный тап
            tap_code16(KC_LALT);
            break;
        case 2:  // Двойной тап
            tap_code16(KC_RALT);
            break;

    }
}

void td_t_grv_reset(tap_dance_state_t *state, void *user_data) {
	//Пустая функция, но обязательная
}
void td_vmute_reset(tap_dance_state_t *state, void *user_data) {}
void td_pppn_reset(tap_dance_state_t *state, void *user_data) {}
