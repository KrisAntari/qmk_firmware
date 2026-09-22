
#include QMK_KEYBOARD_H

// Первый блок тапданса. 

	typedef enum {
		TD_NONE,
		TD_UNKNOWN,
		TD_SINGLE_TAP,
		TD_SINGLE_HOLD,
		TD_DOUBLE_TAP
	} td_state_t;

	typedef struct {
		bool is_press_action;
		td_state_t state;
	} td_tap_t;
	
	
	enum {
	QUOT_LALL,	//работает.
	SS_RUB,		// не работает.
	SS_HASH = SAFE_RANGE,	// работает.
};


	// Укажите функции, которые будут использоваться с вашими клавишами tap dance

	// Функция, связанная со всеми tap dances
	td_state_t cur_dance(tap_dance_state_t *state);

	// Функции, связанные с индивидуальным tap dances
	void ql_finished(tap_dance_state_t *state, void *user_data);
	void ql_reset(tap_dance_state_t *state, void *user_data);


// clang-format off /* в файле \qmk_firmware\keyboards\keychron\k13_pro\k13_pro.c в 66 и 68 строке устанавливаются базовые слои. */
enum layers{
  MAC_BASE,
  MAC_FN,
  WIN_BASE,
  WIN_FN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_ansi_90(
        KC_ESC,             KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_P7,    KC_P8,    KC_P9,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_P4,    KC_P5,    KC_P6,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_P1,    KC_P2,    KC_P3,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,   KC_DEL,   KC_P0,    KC_PDOT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD, KC_ROPTN, MO(MAC_FN),KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_ansi_90(
        _______,            KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,  _______,  _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  KC_INS,   KC_PGUP,  KC_HOME,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  KC_SNAP,  KC_PGDN,  KC_END,             _______,  _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______),

    [WIN_BASE] = LAYOUT_ansi_90(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_P7,    KC_P8,    KC_P9,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_P4,    KC_P5,    KC_P6,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_P1,    KC_P2,    KC_P3,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,   KC_DEL,   KC_P0,    KC_PDOT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RGUI, MO(WIN_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_ansi_90(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  _______,  _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  KC_APP,   KC_SCRL,  KC_INS,   KC_PGUP,  KC_HOME,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  KC_PSCR,  KC_PGDN,  KC_END,             _______,  _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______)
};

// Определите текущее состояние Tap Dance
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) return TD_DOUBLE_TAP;
    else return TD_UNKNOWN;
}

// Инициализируйте структуру касания, связанную с примером клавиши tap dance
static td_tap_t ql_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

// Функции, которые управляют действиями клавиши Tap Dance, переключающей слой.
void ql_finished(tap_dance_state_t *state, void *user_data) {
    ql_tap_state.state = cur_dance(state);
    switch (ql_tap_state.state) {
        case TD_SINGLE_TAP:					// Одинарное нажатие
            tap_code(KC_APP);				// Вызвать контекстное меню
            break;
			
        case TD_SINGLE_HOLD:				// Одинарное нажатие и удержание
				layer_on(WIN_FN);			// Включить слой FN
            break;
			
        case TD_DOUBLE_TAP:					// Двойное нажатие
			if (layer_state_is(WIN_FN)) {	// Проверьте, установлен ли уже этот слой
                layer_off(WIN_FN);			// Если он уже установлен, то выключите его
            }else {
                layer_on(WIN_FN);			// Если этот параметр еще не установлен, то включите слой
            }
            break;
        default:
            break;
    }
}

    // Если клавиша была нажата, а теперь отпущена, то выключите слой
void ql_reset(tap_dance_state_t *state, void *user_data) {
    if (ql_tap_state.state == TD_SINGLE_HOLD) {
        layer_off(WIN_FN);
    }
    ql_tap_state.state = TD_NONE;
}

// Ассоциируйте нашу клавишу Tap Dance с ее функциональностью
tap_dance_action_t tap_dance_actions[] = {
    [QUOT_LALL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ql_finished, ql_reset),
};

// Установите длительность нажатия на клавиши для Tap Dance
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            return 210;
        default:
            return TAPPING_TERM;
    }
}
