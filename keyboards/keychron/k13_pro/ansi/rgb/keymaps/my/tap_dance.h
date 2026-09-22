

// объявляет обработчики и структуры

#pragma once
#include QMK_KEYBOARD_H


// ==================================================
// Структура контекста для Tap Dance
// ==================================================

typedef struct {
    uint8_t fn1_layer; // Функция слоя, который будет активирован
    uint16_t tap_key; // Клавиша, отправляемая при одиночном нажатии
    bool is_active; // Универсальный флаг состояния
} td_ctx_t;

extern td_ctx_t win_fn_left_ctx;
extern td_ctx_t win_fn_right_ctx;
extern td_ctx_t mac_fn_left_ctx;
extern td_ctx_t mac_fn_right_ctx;

// Объявляем функции как extern
void td_finished(tap_dance_state_t *state, void *user_data); // Вызывается при нажатии/отпускании
void td_reset(tap_dance_state_t *state, void *user_data); // Вызывается после завершения действия


// ==================================================
// Типы Tap Dance
// ==================================================

enum td_keycodes {
    TD_L1L,   // Переключение на MacFN слева
	TD_L1R,   // Переключение на MacFN справа
    TD_L3L,   // Переключение на WinFN слева
	TD_L3R,   // Переключение на WinFN справа
	TD_T_GRV, // Клавиша Е/Ё 
	TD_VOLM,  // Vol- / Mute 
	TD_PPPN  // Play / Pause / Previos / Next
  //TD_NEW // Новая клавиша НЕ ЗАБУДЬ ПОСТАВИТЬ ЗАПЯТУЮ ПОСЛЕ ПРЕДЫДУЩЕЙ!!!
};

void td_t_grv_finished(tap_dance_state_t *state, void *user_data);
void td_t_grv_reset(tap_dance_state_t *state, void *user_data);

void td_vmute_finished(tap_dance_state_t *state, void *user_data);
void td_vmute_reset(tap_dance_state_t *state, void *user_data);

void td_pppn_finished(tap_dance_state_t *state, void *user_data);
void td_pppn_reset(tap_dance_state_t *state, void *user_data);
