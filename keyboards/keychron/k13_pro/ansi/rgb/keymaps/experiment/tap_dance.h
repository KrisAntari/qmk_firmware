#pragma once
#include QMK_KEYBOARD_H


enum td_keycodes {
    TD_L3L,   // Левый Win
    TD_PPPN,  // Play/Pause
    TD_VOLM,  // Volume/Mute
    // Добавьте другие TD-клавиши здесь
    TD_NEW_KEY
};

/*
 * Универсальная система Tap Dance с поддержкой:
 * - До 5 тапов
 * - Отдельные действия для каждого тапа и удержания
 * - Совместимость со стандартными QMK-макросами (MO, TG и др.)
 */

// ==================================================
// Структура конфигурации для одной TD-клавиши
// ==================================================
typedef struct {
    // Действия для тапов (1-5)
    uint16_t tap_1;     // Одиночный тап
    uint16_t tap_2;     // Двойной тап
    uint16_t tap_3;     // Тройной тап
    uint16_t tap_4;     // Четверной тап
    uint16_t tap_5;     // Пятикратный тап
    
    // Действия для удержания после тапов
    uint16_t hold_1;    // Удержание после 1 тапа
    uint16_t hold_2;    // Удержание после 2 тапов
    uint16_t hold_3;    // Удержание после 3 тапов
    uint16_t hold_4;    // Удержание после 4 тапов
    uint16_t hold_5;    // Удержание после 5 тапов
} td_config_t;

// ==================================================
// Прототипы функций обработки
// ==================================================
void universal_td_finished(tap_dance_state_t *state, void *user_data);
void universal_td_reset(tap_dance_state_t *state, void *user_data);

// ==================================================
// Макросы для удобного создания конфигов
// ==================================================
/*
#define TD_CONF(...) (td_config_t){__VA_ARGS__}
#define TD_TAP_1(x) .tap_1 = x
#define TD_TAP_2(x) .tap_2 = x
#define TD_TAP_3(x) .tap_3 = x
#define TD_TAP_4(x) .tap_4 = x
#define TD_TAP_5(x) .tap_5 = x
#define TD_HOLD_1(x) .hold_1 = x
#define TD_HOLD_2(x) .hold_2 = x 
#define TD_HOLD_3(x) .hold_3 = x
#define TD_HOLD_4(x) .hold_4 = x
#define TD_HOLD_5(x) .hold_5 = x
*/