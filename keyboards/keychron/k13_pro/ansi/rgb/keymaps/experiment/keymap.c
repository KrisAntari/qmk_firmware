

#include QMK_KEYBOARD_H
#include "tap_dance.h"


// Слои
enum layers {
    MAC,	// 0
    MAC_FN,	// 1
    WIN,	// 2
    WIN_FN,	// 3
	ALL_FN	// 4
};


// Пример конфигурации для левой Win-клавиши
const td_config_t win_left_config = {
    .tap_1 = KC_LWIN,		// Одиночный тап
    .tap_2 = TG(WIN_FN),	// Двойной тап
    .tap_3 = TG(ALL_FN),	// Тройной тап
    .hold_1 = MO(WIN_FN),	// Удержание после 1 тапа
    .hold_2 = MO(ALL_FN)	// Удержание после 2 тапов
};


// Таблица Tap Dance действий
tap_dance_action_t tap_dance_actions[] = {
    [TD_L3L] = ACTION_TAP_DANCE_FN_ADVANCED_USER(
        NULL, 
        td_finished, 
        td_reset, 
        &win_left_config
    )
};




const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC] = LAYOUT_ansi_90(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,     KC_F5,    KC_F6,    KC_F7,      KC_F8,      KC_F9,    KC_F10,   KC_F11,     KC_F12,     KC_P7,    KC_P8,    KC_P9,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,      KC_6,     KC_7,     KC_8,       KC_9,       KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,    KC_P4,    KC_P5,    KC_P6,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,      KC_Y,     KC_U,     KC_I,       KC_O,       KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,    KC_P1,    KC_P2,    KC_P3,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,      KC_H,     KC_J,     KC_K,       KC_L,       KC_SCLN,  KC_QUOT,              KC_ENT,     KC_DEL,   KC_P0,    KC_PDOT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,      KC_B,     KC_N,     KC_M,       KC_COMM,    KC_DOT,   KC_SLSH,              KC_RSFT,              KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                                KC_SPC,                                     KC_RCMMD, KC_ROPTN ,MO(MAC_FN),KC_RCTL,     KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_ansi_90(
        _______,            KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,   RGB_VAD,  RGB_VAI,  KC_MPRV,    KC_MPLY,    KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,    KC_PSCR,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,  _______,    _______,    _______,  _______,  _______,
        RGB_TOG,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  KC_INS,     _______,    KC_PAUS,  _______,  _______,    _______,    BT_HST1,  BT_HST2,  BT_HST3,
        _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,              _______,    _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,              _______,              KC_PGUP,
        _______,  _______,  _______,                                 _______,                                    _______,  _______,  _______,    _______,    KC_HOME,  KC_PGDN,  KC_END),

    [WIN] = LAYOUT_ansi_90(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,     KC_F5,    KC_F6,    KC_F7,      KC_F8,      KC_F9,    KC_F10,   KC_F11,     KC_F12,     KC_P7,    KC_P8,    KC_P9,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,      KC_6,     KC_7,     KC_8,       KC_9,       KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,    KC_P4,    KC_P5,    KC_P6,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,      KC_Y,     KC_U,     KC_I,       KC_O,       KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,    KC_P1,    KC_P2,    KC_P3,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,      KC_H,     KC_J,     KC_K,       KC_L,       KC_SCLN,  KC_QUOT,              KC_ENT,     KC_DEL,   KC_P0,    KC_NUM,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,      KC_B,     KC_N,     KC_M,       KC_COMM,    KC_DOT,   KC_SLSH,              KC_RSFT,              KC_UP,
        KC_LCTL, TD(TD_L3L),KC_LALT,                                 KC_SPC,                                     KC_RALT,  TD(TD_L3L), _______,   KC_RCTL,    KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_ansi_90(
        DM_RSTP,            DM_REC1,  DM_PLY1,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,  _______,    _______,    KC_PSCR,  _______,  KC_CALC,
        _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,  _______,    _______,    _______,  _______,  _______,
        RGB_TOG,  KC_HOME,  KC_UP,    KC_END,   _______,  _______,   _______,  _______,  KC_INS,     _______,    KC_PAUS,  _______,  _______,    _______,    BT_HST1,  BT_HST2,  BT_HST3,
        _______,  KC_LEFT,  KC_DOWN,  KC_RGHT,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,              _______,    _______,  _______,  _______,
        _______,            KC_BRID,  KC_BRIU,  RGB_VAD,  RGB_VAI,   BAT_LVL,  KC_TASK, TD(TD_PPPN), TD(TD_VOLM),KC_VOLU,  KC_FILE,              _______,              KC_PGUP,
        _______,  _______,  _______,                                 _______,                                    _______,  _______,  _______,    _______,    KC_HOME,  KC_PGDN,  KC_END),
		
	[ALL_FN] = LAYOUT_ansi_90(
        _______,            _______,  _______,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,  _______,    _______,    _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,  _______,    _______,    _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,  _______,    _______,    _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,              _______,    _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,   _______,  _______,  _______,    _______,    _______,  _______,              _______,              _______,
        _______,  _______,  _______,                                 _______,                                    _______,  _______,  _______,    _______,    _______,  _______,  _______)
};




//====================================================================================НИЖЕ ВСЁ РАБОТАЕТ=============================================================================


void keyboard_post_init_user(void) {

	// Подсветка #ff783d Тёплый белый (RGB (255, 190, 143) HSV (25, 44, 100) HEX (ffbe8f)
	// #ff82a8 Холодный белый
	rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR); //_noeeprom
    rgb_matrix_sethsv(18, 199, 255); //_noeeprom
}



// Индикация
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    /* Индикация слоёв */
    if (get_highest_layer(layer_state) > 0) {
	
        uint8_t layer = get_highest_layer(layer_state);

        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) 
		{
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) 
			{
                uint8_t index = g_led_config.matrix_co[row][col];

                if (index >= led_min && index < led_max && index != NO_LED &&
                keymap_key_to_keycode(layer, (keypos_t){col,row}) > KC_TRNS) 
				{
                    rgb_matrix_set_color(index, 98, 255, 187); //Бирюзовый
					
					RGB_MATRIX_INDICATOR_SET_COLOR(41, 255, 0, 0); //Ins
					
					RGB_MATRIX_INDICATOR_SET_COLOR(82, 0, 0, 0);  // Spacebar Работает на слое. Надо пересунуть на мастер.
					RGB_MATRIX_INDICATOR_SET_COLOR(34, 255, 130, 168); //WASD
					RGB_MATRIX_INDICATOR_SET_COLOR(35, 255, 130, 168);
					RGB_MATRIX_INDICATOR_SET_COLOR(36, 255, 130, 168);
					RGB_MATRIX_INDICATOR_SET_COLOR(51, 255, 130, 168);
					RGB_MATRIX_INDICATOR_SET_COLOR(52, 255, 130, 168);
					RGB_MATRIX_INDICATOR_SET_COLOR(53, 255, 130, 168);
					
					RGB_MATRIX_INDICATOR_SET_COLOR(67, 255, 86, 3); // Bottom
					RGB_MATRIX_INDICATOR_SET_COLOR(68, 255, 86, 3);
					RGB_MATRIX_INDICATOR_SET_COLOR(69, 255, 86, 3);
					RGB_MATRIX_INDICATOR_SET_COLOR(70, 255, 86, 3); 
					RGB_MATRIX_INDICATOR_SET_COLOR(71, 255, 86, 3);
					RGB_MATRIX_INDICATOR_SET_COLOR(72, 255, 86, 3);
					RGB_MATRIX_INDICATOR_SET_COLOR(73, 255, 86, 3);
					RGB_MATRIX_INDICATOR_SET_COLOR(74, 255, 86, 3);
					RGB_MATRIX_INDICATOR_SET_COLOR(75, 255, 86, 3);
					RGB_MATRIX_INDICATOR_SET_COLOR(76, 255, 86, 3);
				}
            }
		
        }
    }


    /*CapsLock Indication Работает */
    if (host_keyboard_led_state().caps_lock) {                  
        RGB_MATRIX_INDICATOR_SET_COLOR(50, 255, 0, 0); // assuming caps lock is at led #50

        RGB_MATRIX_INDICATOR_SET_COLOR(16, 255, 0, 0); // Ё
        RGB_MATRIX_INDICATOR_SET_COLOR(34, 255, 0, 0); // Top
        RGB_MATRIX_INDICATOR_SET_COLOR(35, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(36, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(37, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(38, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(39, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(40, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(41, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(42, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(43, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(44, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(45, 255, 0, 0);  

        RGB_MATRIX_INDICATOR_SET_COLOR(51, 255, 0, 0); //Mid
        RGB_MATRIX_INDICATOR_SET_COLOR(52, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(53, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(54, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(55, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(56, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(57, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(58, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(59, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(60, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(61, 255, 0, 0);

        RGB_MATRIX_INDICATOR_SET_COLOR(67, 255, 0, 0); // Bottom
        RGB_MATRIX_INDICATOR_SET_COLOR(68, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(69, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(70, 255, 0, 0); 
        RGB_MATRIX_INDICATOR_SET_COLOR(71, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(72, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(73, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(74, 255, 0, 0);
        RGB_MATRIX_INDICATOR_SET_COLOR(75, 255, 0, 0);
    }

    /* НумЛок, работает */
    if (host_keyboard_led_state().num_lock){
        RGB_MATRIX_INDICATOR_SET_COLOR(65, 255, 0, 0)
		
        RGB_MATRIX_INDICATOR_SET_COLOR(13, 0, 255, 0)
		RGB_MATRIX_INDICATOR_SET_COLOR(14, 0, 255, 0)
		RGB_MATRIX_INDICATOR_SET_COLOR(15, 0, 255, 0)
		
        RGB_MATRIX_INDICATOR_SET_COLOR(30, 0, 255, 0)
		RGB_MATRIX_INDICATOR_SET_COLOR(31, 0, 255, 0)
		RGB_MATRIX_INDICATOR_SET_COLOR(32, 0, 255, 0)
		
        RGB_MATRIX_INDICATOR_SET_COLOR(47, 0, 255, 0)
		RGB_MATRIX_INDICATOR_SET_COLOR(48, 0, 255, 0)
		RGB_MATRIX_INDICATOR_SET_COLOR(49, 0, 255, 0)
		
        RGB_MATRIX_INDICATOR_SET_COLOR(63, 0, 255, 0)
		RGB_MATRIX_INDICATOR_SET_COLOR(64, 0, 255, 0)
    }
    return false; 
}


