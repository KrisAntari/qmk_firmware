/* Copyright 2024 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Стоит запилить:
// 3. Оптимизировать USB-эндпоинты.  5. Навести красивостей с анимацией подсветки.

#include QMK_KEYBOARD_H
#include "keychron_common.h"
#include "action_layer.h"
#include "timer.h"

// ==================================================
// Логика простоя для Pixel Rain
// ==================================================

#define IDLE_BEFORE_RAIN_MS      120000UL  // 2 минуты до включения Pixel Rain
#define BATTERY_RAIN_DURATION_MS 300000UL  // 5 минут работы Pixel Rain от батареи

static uint32_t last_activity_time = 0;
static uint8_t  previous_mode      = 0;
static bool     rain_active        = false;
static bool     rain_from_battery  = false;
static uint32_t rain_started_at    = 0;
static bool     rain_suppressed    = false;  // true после battery-таймаута: не запускать rain до активности

// Вспомогательная функция: работаем от батареи?
#include "usb_util.h"

static bool is_on_battery(void) {
    return !usb_connected_state();
}


#define NUM_LOCK_LED_INDEX 50
#define BLINK_INTERVAL_MS 500

static uint16_t blink_timer = 0;
static bool led_blink_state = false;

enum layers {
    MAC_BASE,
    WIN_BASE,
    MAC_FN1,
    WIN_FN1,
    FN2,
    MIDI
};

extern MidiDevice midi_device;

// ==================================================
// Tap Dance: структура контекста и экземпляры
// ==================================================

typedef struct {
    uint8_t fn1_layer;      // слой, активируемый удержанием/двойным тапом
    uint16_t tap_key;       // клавиша одиночного тапа
    bool is_active;         // общий флаг (для совместимости/отладки)
    bool tap_key_active;    // была ли зарегистрирована tap_key
    bool layer_active;      // был ли включён слой
} td_ctx_t;

td_ctx_t win_fn_left_ctx = {
    .fn1_layer = WIN_FN1,
    .tap_key = KC_LWIN,
    .is_active = false,
    .tap_key_active = false,
    .layer_active = false
};

td_ctx_t win_fn_right_ctx = {
    .fn1_layer = WIN_FN1,
    .tap_key = KC_RGUI,
    .is_active = false,
    .tap_key_active = false,
    .layer_active = false
};

td_ctx_t mac_fn_left_ctx = {
    .fn1_layer = MAC_FN1,
    .tap_key = KC_LOPTN,
    .is_active = false,
    .tap_key_active = false,
    .layer_active = false
};

td_ctx_t mac_fn_right_ctx = {
    .fn1_layer = MAC_FN1,
    .tap_key = KC_ROPTN,
    .is_active = false,
    .tap_key_active = false,
    .layer_active = false
};

// ==================================================
// Tap Dance: обработчики
// ==================================================

// --- Универсальный обработчик (для Win/Option) ---
void td_finished(tap_dance_state_t *state, void *user_data) {
    td_ctx_t *ctx = (td_ctx_t *)user_data;

    switch (state->count) {
        case 1:
            if (state->pressed) {
                // Удержание: регистрируем клавишу без блокировки
                ctx->is_active = true;
                ctx->tap_key_active = true;
                register_code(ctx->tap_key);
            } else {
                // Одиночный тап
                tap_code(ctx->tap_key);
            }
            break;

        case 2:
            if (state->pressed) {
                // Двойное нажатие + удержание → слой на время удержания
                ctx->is_active = true;
                ctx->layer_active = true;
                layer_on(ctx->fn1_layer);
            } else {
                // Двойной тап → переключение слоя
                layer_invert(ctx->fn1_layer);
            }
            break;
    }
}

void td_reset(tap_dance_state_t *state, void *user_data) {
    td_ctx_t *ctx = (td_ctx_t *)user_data;

    if (ctx->tap_key_active) {
        unregister_code(ctx->tap_key);
        ctx->tap_key_active = false;
    }
    if (ctx->layer_active) {
        layer_off(ctx->fn1_layer);
        ctx->layer_active = false;
    }
    ctx->is_active = false;
}

// --- Хардкод для простых сценариев ---

void td_t_grv_finished(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:  // Одиночный тап
            tap_code16(KC_GRV);
            break;
        case 2:  // Двойной тап
            tap_code16(KC_ESC);
            break;
/*
        case 3:  // Тройной тап
            tap_code16(KC_XXX);
            break;
*/
    }
}
void td_t_grv_reset(tap_dance_state_t *state, void *user_data) {
    // Пустая функция, но обязательная
}

void td_vmute_finished(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:  // Одиночный тап
            tap_code16(KC_VOLD);
            break;
        case 2:  // Двойной тап
            tap_code16(KC_MUTE);
            break;
    }
}
void td_vmute_reset(tap_dance_state_t *state, void *user_data) {}

void td_pppn_finished(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:  // Одиночный тап
            tap_code16(KC_MPLY);
            break;
        case 2:  // Двойной тап
            tap_code16(KC_MNXT);
            break;
        case 3:  // Тройной тап
            tap_code16(KC_MPRV);
            break;
    }
}
void td_pppn_reset(tap_dance_state_t *state, void *user_data) {}

// ==================================================
// Tap Dance: типы и таблица действий
// ==================================================

enum td_keycodes {
    TD_L1L,   // Переключение на MacFN слева
    TD_L1R,   // Переключение на MacFN справа
    TD_L3L,   // Переключение на WinFN слева
    TD_L3R,   // Переключение на WinFN справа
    TD_GESC,  // Клавиша Ё/Esc
    TD_VOLM,  // Vol- / Mute
    TD_PPPN   // Play / Pause / Previous / Next
  //TD_NEW    // Новая клавиша — НЕ ЗАБУДЬ ЗАПЯТУЮ ПОСЛЕ ПРЕДЫДУЩЕЙ!!!
};

tap_dance_action_t tap_dance_actions[] = {
    [TD_L1L] = {  // Левый Option, переключение на MacFn
        .fn = { NULL, td_finished, td_reset },
        .user_data = &mac_fn_left_ctx
    },
    [TD_L1R] = {  // Правый Option, переключение на MacFn
        .fn = { NULL, td_finished, td_reset },
        .user_data = &mac_fn_right_ctx
    },
    [TD_L3L] = {  // Левый Win, переключение на WinFn
        .fn = { NULL, td_finished, td_reset },
        .user_data = &win_fn_left_ctx
    },
    [TD_L3R] = {  // Правый Win, переключение на WinFn
        .fn = { NULL, td_finished, td_reset },
        .user_data = &win_fn_right_ctx
    },
    [TD_GESC] = ACTION_TAP_DANCE_FN_ADVANCED(
        NULL,
        td_t_grv_finished,
        td_t_grv_reset
    ),
    [TD_VOLM] = ACTION_TAP_DANCE_FN_ADVANCED(
        NULL,
        td_vmute_finished,
        td_vmute_reset
    ),
    [TD_PPPN] = ACTION_TAP_DANCE_FN_ADVANCED(
        NULL,
        td_pppn_finished,
        td_pppn_reset
    ),
};

// ==================================================
// Карты раскладок
// ==================================================

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* Layer 0
     *┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────────────┬───────┐
     *│ `     │ 1     │ 2     │ 3     │ 4     │ 5     │ 6     │ 7     │ 8     │ 9     │ 0     │ - _   │ = +   │ Backsp        │ PrScr │
     *│00   Ë │01   ! │01   " │03   № │04   ; │05   % │06   : │07   ? │08   * │09   ( │10   ) │11     │12     │13             │14     │
     *├───────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───────────┼───────┤
     *│    Tab    │ Q     │ W     │ E     │ R     │ T     │ Y     │ U     │ I     │ O     │ P     │ [     │ ]     │ \ |       │ Calc  │
     *│15         │16   Й │17   Ц │18   У │19   К │20   Е │21   Н │22   Г │23   Ш │24   Щ │25   З │26   Х │27   Ъ │28     \ / │29     │
     *├───────────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴───────────┼───────┤
     *│    Caps     │ A     │ S     │ D     │ F     │ G     │ H     │ J     │ K     │ L     │; :    │ "     │      Enter      │ . Del │
     *│30           │31   Ф │32   Ы │33   В │34   А │35   П │36   Р │37   О │38   Л │39   Д │40   Ж │41   Э │42               │43     │
     *├─────────────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴─────────┬───────┼───────┤
     *│    Shift        │ Z     │ X     │ C     │ V     │ B     │ N     │ M     │ , <   │ . >   │ / ?   │   Shift     │ ↑     │  Fn2  │
     *│44               │45   Я │46   Ч │47   С │48   М │49   И │50   Т │51   Ь │52   Б │53   Ю │54  ., │55           │56     │57     │
     *├─────────┬───────┴─┬─────┴───┬───┴───────┴───────┴───────┴───────┴───────┴─────┬─┴─────┬─┴─────┬─┴─────┬───────┼───────┼───────┤
     *│ Control │  Option │   Cmd   │                     Space                       │  Cmd  │  Fn1  │Control│ ←     │ ↓     │ →     │
     *│58       │59       │60       │61                                               │62     │63     │64     │65     │66     │67     │
     *└─────────┴─────────┴─────────┴─────────────────────────────────────────────────┴───────┴───────┴───────┴───────┴───────┴───────┘
    */
    [MAC_BASE] = LAYOUT_ansi_68(
        KC_GRV,     KC_1,      KC_2,      KC_3,      KC_4,     KC_5,      KC_6,      KC_7,      KC_8,      KC_9,      KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PSCR,
        KC_TAB,     KC_Q,      KC_W,      KC_E,      KC_R,     KC_T,      KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_CALC,
        KC_CAPS,    KC_A,      KC_S,      KC_D,      KC_F,     KC_G,      KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,  KC_QUOT,            KC_ENT,             KC_DEL,
        KC_LSFT,    KC_Z,      KC_X,      KC_C,      KC_V,     KC_B,      KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,                      KC_RSFT,  KC_UP,    MO(FN2),
        KC_LCTL,    KC_LOPTN,  KC_LCMMD,                                  KC_SPC,                                     KC_RCMMD,  MO(MAC_FN1), KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    /* Layer 1 */
    [WIN_BASE] = LAYOUT_ansi_68(
        KC_GRV,     KC_1,      KC_2,      KC_3,      KC_4,     KC_5,      KC_6,      KC_7,      KC_8,      KC_9,      KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PSCR,
        KC_TAB,     KC_Q,      KC_W,      KC_E,      KC_R,     KC_T,      KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_CALC,
        KC_CAPS,    KC_A,      KC_S,      KC_D,      KC_F,     KC_G,      KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,  KC_QUOT,            KC_ENT,             KC_DEL,
        KC_LSFT,    KC_Z,      KC_X,      KC_C,      KC_V,     KC_B,      KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_SLSH,                      KC_RSFT,  KC_UP,    MO(FN2),
        KC_LCTL,    MO(WIN_FN1), KC_LALT,                                 KC_SPC,                                     KC_RALT,  TD(TD_L3R), KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    /* Layer 2 — MAC_FN1 */
    [MAC_FN1] = LAYOUT_ansi_68(
        KC_ESC,     KC_F1,     KC_F2,     KC_F3,     KC_F4,    KC_F5,     KC_F6,     KC_F7,     KC_F8,     KC_F9,     KC_F10,   KC_F11,   KC_F12,   _______,             _______,
        _______,    KC_MS_BTN1, KC_MS_UP,  KC_MS_BTN2, _______, _______,  _______,   _______,   KC_INS,    KC_SCRL,   KC_PAUS,  _______,  _______,  _______,             _______,
        RGB_TOG,    KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT, _______, _______, _______,   _______,   _______,   _______,   _______,  _______,            _______,             _______,
        _______,    KC_BRID,   KC_BRIU,   KC_MCTRL,  KC_LNPAD, BAT_LVL,  KC_MCTRL,  TD(TD_PPPN), TD(TD_VOLM), KC_VOLU, KC_LNPAD,                     _______,  KC_PGUP,   _______,
        _______,    _______,   _______,                                  _______,                                    _______,   _______,   _______,  KC_HOME,  KC_PGDN,   KC_END),

    /* Layer 3 — WIN_FN1 */
    [WIN_FN1] = LAYOUT_ansi_68(
        KC_ESC,     KC_F1,     KC_F2,     KC_F3,     KC_F4,    KC_F5,     KC_F6,     KC_F7,     KC_F8,     KC_F9,     KC_F10,   KC_F11,   KC_F12,   _______,             _______,
        _______,    KC_MS_BTN1, KC_MS_UP,  KC_MS_BTN2, _______, _______,  _______,   _______,   KC_INS,    KC_SCRL,   KC_PAUS,  _______,  _______,  _______,             _______,
        RGB_TOG,    KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT, _______, _______, _______,   _______,   _______,   _______,   _______,  _______,            _______,             _______,
        _______,    KC_BRID,   KC_BRIU,   RGB_VAD,   RGB_VAI,  BAT_LVL,   KC_TASK,   TD(TD_PPPN), TD(TD_VOLM), KC_VOLU, KC_FILE,                     _______,  KC_PGUP,   _______,
        _______,    _______,   _______,                                  _______,                                    _______,   _______,   _______,  KC_HOME,  KC_PGDN,   KC_END),

    /* Layer 4 — FN2 */
    [FN2] = LAYOUT_ansi_68(
        KC_ESC,     KC_P1,     KC_P2,     KC_P3,     KC_P4,    KC_P5,     KC_P6,     KC_P7,     KC_P8,     KC_P9,     KC_P0,    KC_PMNS,  KC_PPLS,  _______,             _______,
        RGB_TOG,    BT_HST1,   BT_HST2,   BT_HST3,   P2P4G,    _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,             _______,
        _______,    RGB_MOD,   RGB_VAI,   RGB_HUI,   RGB_SAI,  RGB_SPI,   _______,   _______,   _______,   _______,   _______,  _______,            _______,             _______,
        _______,    RGB_RMOD,  RGB_VAD,   RGB_HUD,   RGB_SAD,  RGB_SPD,   KC_NUM,    TG(MIDI),  NK_TOGG,   _______,   _______,                      _______,  _______,   _______,
        _______,    _______,   _______,                                  _______,                                    _______,   _______,   _______,  _______,  _______,   _______),

    /* Layer 5 — MIDI */
    [MIDI] = LAYOUT_ansi_68(
        KC_F13,     KC_F14,    KC_F15,    KC_F16,    KC_F17,   KC_F18,    KC_F19,    KC_F20,    KC_F21,    KC_F22,    KC_F23,   _______,  _______,  _______,             _______,
        _______,    _______,   _______,   _______,   _______,  _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,             _______,
        _______,    _______,   _______,   _______,   _______,  _______,   _______,   _______,   _______,   _______,   _______,  _______,            _______,             _______,
        _______,    _______,   _______,   _______,   _______,  _______,   _______,   TG(MIDI),  _______,   _______,   _______,                      _______,  _______,   _______,
        _______,    _______,   KC_MRWD,                                  KC_F24,                                     _______,   _______,   _______,  _______,  _______,   _______),
};

// clang-format on

// ==================================================
// Обработка нажатий
// ==================================================

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // --- Логика простоя ---
    if (record->event.pressed) {
        last_activity_time = timer_read32(); // Сбрасываем таймер простоя
        rain_suppressed    = false;          // Активность снимает блокировку после battery-таймаута

        // Любая активность (в т.ч. от батареи) возвращает предыдущий эффект
        if (rain_active) {
            rgb_matrix_mode_noeeprom(previous_mode);
            rain_active = false;
        }
    }
    // --- Конец логики простоя ---

    // --- Alt+Q → Alt+F4 (только на Windows-слоях и только чистый левый Alt) ---
    if (keycode == KC_Q && record->event.pressed) {
        uint8_t layer = get_highest_layer(layer_state);
        if (layer == WIN_BASE || layer == WIN_FN1) {
            uint8_t mods = get_mods();
            if ((mods & MOD_BIT(KC_LALT)) && !(mods & ~MOD_BIT(KC_LALT))) {
                tap_code16(KC_F4);
                return false;
            }
        }
    }

    // --- MIDI: обрабатываем только когда активен слой MIDI ---
    if (!layer_state_is(MIDI)) {
        return true;
    }

    // Синтаксис: midi_send_cc(&midi_device, midi_config.channel, номер_контроллера, значение);
    switch (keycode) {
        case KC_F13: // T0
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 0);
            return false;
        case KC_F14: // T1
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 13);
            return false;
        case KC_F15: // T2
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 26);
            return false;
        case KC_F16: // T3
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 39);
            return false;
        case KC_F17: // T4
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 51);
            return false;
        case KC_F18: // T5
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 64);
            return false;
        case KC_F19: // T6
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 77);
            return false;
        case KC_F20: // T7
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 89);
            return false;
        case KC_F21: // T8
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 102);
            return false;
        case KC_F22: // T9
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 115);
            return false;
        case KC_F23: // T10
            if (record->event.pressed) midi_send_cc(&midi_device, midi_config.channel, 16, 127);
            return false;
        case KC_F24: // GO
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 44, 127);
            } else {
                midi_send_cc(&midi_device, midi_config.channel, 44, 0);
            }
            return false;
        case KC_MRWD: // BACK
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 43, 127);
            } else {
                midi_send_cc(&midi_device, midi_config.channel, 43, 0);
            }
            return false;
    }
    return true;
}

// ==================================================
// Инициализация
// ==================================================

void keyboard_post_init_user(void) {
    // Подсветка #ff783d Тёплый белый (RGB 255,190,143 / HSV 25,44,100 / HEX ffbe8f)
    // #ff82a8 Холодный белый
    // Без _noeeprom каждый старт писал в EEPROM — теперь только в RAM.
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(24, 34, 100);

    // Инициализация логики простоя
    last_activity_time = timer_read32();
    previous_mode      = rgb_matrix_get_mode();
}


// ==================================================
// Эффект простоя
// ==================================================

void matrix_scan_user(void) {
    uint32_t now = timer_read32();

    // --- Если Pixel Rain уже активен ---
    if (rain_active) {
        if (rain_from_battery) {
            // От батареи: через 5 минут выключаем подсветку полностью
            if (timer_elapsed32(rain_started_at) > BATTERY_RAIN_DURATION_MS) {
                rgb_matrix_disable_noeeprom();
                rain_active     = false;
                rain_suppressed = true; // не запускать rain снова, пока не будет активности
            }
        }
        // От USB: держим эффект, пока не будет нажата клавиша.
        // Сброс произойдёт в process_record_user.
        return;
    }

    // После battery-таймаута не запускаем rain до следующей активности пользователя
    if (rain_suppressed) {
        return;
    }

    // --- Если Pixel Rain ещё не активен и прошло 2 минуты простоя ---
    if (timer_elapsed32(last_activity_time) > IDLE_BEFORE_RAIN_MS) {
        previous_mode = rgb_matrix_get_mode(); // Запоминаем текущий эффект

        // На случай, если RGB был погашен предыдущим battery-таймаутом
        rgb_matrix_enable_noeeprom();
        rgb_matrix_mode_noeeprom(RGB_MATRIX_PIXEL_RAIN);
        rain_active       = true;
        rain_started_at   = now;
        rain_from_battery = is_on_battery();
    }
}

// ==================================================
// Индикация
// ==================================================

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    const uint8_t layer = get_highest_layer(layer_state);
    const led_t   led_state = host_keyboard_led_state();

    // Сбрасываем мигание везде, кроме слоя FN2
    if (layer != FN2) {
        blink_timer = 0;
        led_blink_state = false;
    }

    /* CapsLock Indication */
    if (led_state.caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(0,  200, 0, 0); // Ё
        RGB_MATRIX_INDICATOR_SET_COLOR(16, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(17, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(18, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(19, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(20, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(21, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(22, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(23, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(24, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(25, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(26, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(27, 200, 0, 0);

        RGB_MATRIX_INDICATOR_SET_COLOR(30, 250, 0, 0); // CapsLock
        RGB_MATRIX_INDICATOR_SET_COLOR(31, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(32, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(33, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(34, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(35, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(36, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(37, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(38, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(39, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(40, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(41, 200, 0, 0);

        RGB_MATRIX_INDICATOR_SET_COLOR(45, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(46, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(47, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(48, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(49, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(50, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(51, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(52, 200, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(53, 200, 0, 0);
    }

    /* Индикация слоёв */
    if (layer == MAC_FN1) {
        RGB_MATRIX_INDICATOR_SET_COLOR(0,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(1,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(2,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(3,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(4,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(5,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(6,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(7,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(8,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(9,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(10, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(11, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(12, 98, 255, 187);

        RGB_MATRIX_INDICATOR_SET_COLOR(23, 200, 20, 20); // Ins
        RGB_MATRIX_INDICATOR_SET_COLOR(24, 200, 20, 20); // Scroll Lock
        RGB_MATRIX_INDICATOR_SET_COLOR(25, 200, 20, 20); // Pause

        RGB_MATRIX_INDICATOR_SET_COLOR(16, 255, 130, 168); // Mouse Btn1
        RGB_MATRIX_INDICATOR_SET_COLOR(17, 255, 130, 168); // Mouse Up
        RGB_MATRIX_INDICATOR_SET_COLOR(18, 255, 130, 168); // Mouse Btn2
        RGB_MATRIX_INDICATOR_SET_COLOR(31, 255, 130, 168); // Mouse Left
        RGB_MATRIX_INDICATOR_SET_COLOR(32, 255, 130, 168); // Mouse Down
        RGB_MATRIX_INDICATOR_SET_COLOR(33, 255, 130, 168); // Mouse Right

        RGB_MATRIX_INDICATOR_SET_COLOR(45, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(46, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(47, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(48, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(49, 50, 255, 50);
        RGB_MATRIX_INDICATOR_SET_COLOR(50, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(51, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(52, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(53, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(54, 255, 86, 3);
    } else if (layer == WIN_FN1) {
        RGB_MATRIX_INDICATOR_SET_COLOR(0,  255, 86, 3);  // Esc
        RGB_MATRIX_INDICATOR_SET_COLOR(1,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(2,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(3,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(4,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(5,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(6,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(7,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(8,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(9,  98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(10, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(11, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(12, 98, 255, 187);

        RGB_MATRIX_INDICATOR_SET_COLOR(23, 200, 0, 0); // Ins
        RGB_MATRIX_INDICATOR_SET_COLOR(24, 200, 0, 0); // Scroll Lock
        RGB_MATRIX_INDICATOR_SET_COLOR(25, 200, 0, 0); // Pause

        RGB_MATRIX_INDICATOR_SET_COLOR(16, 255, 130, 168);
        RGB_MATRIX_INDICATOR_SET_COLOR(17, 255, 130, 168);
        RGB_MATRIX_INDICATOR_SET_COLOR(18, 255, 130, 168);
        RGB_MATRIX_INDICATOR_SET_COLOR(31, 255, 130, 168);
        RGB_MATRIX_INDICATOR_SET_COLOR(32, 255, 130, 168);
        RGB_MATRIX_INDICATOR_SET_COLOR(33, 255, 130, 168);

        RGB_MATRIX_INDICATOR_SET_COLOR(45, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(46, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(47, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(48, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(49, 50, 255, 50);
        RGB_MATRIX_INDICATOR_SET_COLOR(50, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(51, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(52, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(53, 255, 86, 3);
        RGB_MATRIX_INDICATOR_SET_COLOR(54, 255, 86, 3);

        RGB_MATRIX_INDICATOR_SET_COLOR(56, 255, 130, 168); // ↑
        RGB_MATRIX_INDICATOR_SET_COLOR(65, 255, 130, 168); // ←
        RGB_MATRIX_INDICATOR_SET_COLOR(66, 255, 130, 168); // ↓
        RGB_MATRIX_INDICATOR_SET_COLOR(67, 255, 130, 168); // →
    } else if (layer == FN2) {
        RGB_MATRIX_INDICATOR_SET_COLOR(15, 98, 255, 187); // RGB TOG
        RGB_MATRIX_INDICATOR_SET_COLOR(16, 0, 0, 168);    // BT-1
        RGB_MATRIX_INDICATOR_SET_COLOR(17, 0, 0, 168);    // BT-2
        RGB_MATRIX_INDICATOR_SET_COLOR(18, 0, 0, 168);    // BT-3
        RGB_MATRIX_INDICATOR_SET_COLOR(19, 0, 0, 168);    // Донгл

        RGB_MATRIX_INDICATOR_SET_COLOR(31, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(32, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(33, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(34, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(35, 98, 255, 187);

        RGB_MATRIX_INDICATOR_SET_COLOR(45, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(46, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(47, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(48, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(49, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(50, 98, 255, 187);
        RGB_MATRIX_INDICATOR_SET_COLOR(51, 0, 255, 0);

        // === NumLock: зелёный при вкл., мигающий красный при выкл. ===
        if (led_state.num_lock) {
            RGB_MATRIX_INDICATOR_SET_COLOR(NUM_LOCK_LED_INDEX, 0x00, 0xFF, 0x00);
            blink_timer = 0;
            led_blink_state = false;
        } else {
            if (timer_elapsed(blink_timer) > BLINK_INTERVAL_MS) {
                led_blink_state = !led_blink_state;
                blink_timer = timer_read();
            }
            if (led_blink_state) {
                RGB_MATRIX_INDICATOR_SET_COLOR(NUM_LOCK_LED_INDEX, 0xFF, 0x00, 0x00);
            } else {
                RGB_MATRIX_INDICATOR_SET_COLOR(NUM_LOCK_LED_INDEX, 0x00, 0x00, 0x00);
            }
        }
    } else if (layer == MIDI) {
        RGB_MATRIX_INDICATOR_SET_COLOR(0,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(1,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(2,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(3,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(4,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(5,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(6,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(7,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(8,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(9,  0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(10, 0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(51, 200, 0, 0);

        RGB_MATRIX_INDICATOR_SET_COLOR(60, 0, 255, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(61, 0, 255, 0);
    }

    // Отключаем стандартные индикаторы QMK — мы всё рисуем сами.
    return true;
}