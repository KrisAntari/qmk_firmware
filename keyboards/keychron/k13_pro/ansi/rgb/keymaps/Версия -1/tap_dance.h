

// объявляет обработчики и структуры

#pragma once
#include QMK_KEYBOARD_H



// Объявляем функции как extern
extern void td_finished(tap_dance_state_t *state, void *user_data); // Вызывается при нажатии/отпускании
extern void td_reset(tap_dance_state_t *state, void *user_data); // Вызывается после завершения действия

// ==================================================
// Структура контекста для Tap Dance
// ==================================================

typedef struct {
    uint8_t layer; // Номер слоя, который будет активирован
    uint16_t tap_key; // Клавиша, отправляемая при одиночном нажатии
    bool is_holding; // Флаг удержания (true/false)
} td_ctx_t;


// ==================================================
// Типы Tap Dance
// ==================================================

enum td_keycodes {
    TD_L1,   // Первая кастомная клавиша Tap Dance
    TD_L3,   // Вторая кастомная клавиша Tap Dance
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
