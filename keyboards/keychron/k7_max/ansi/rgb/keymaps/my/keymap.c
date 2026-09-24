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

//Стоит запилить: 1. Проверку слоя для миди и клавиатурных сокращений. 2. ОптимизироватьtapDance. 3. Оптимизировать USB-эндпоинты. 4. Прокачать энергосбережение. 5. Навести красивостей с анимацией подсветки.



#include QMK_KEYBOARD_H
#include "keychron_common.h"
#include "tap_dance.h"
#include "timer.h" //поддержка мигания диодом.

#define NUM_LOCK_LED_INDEX 50
#define BLINK_INTERVAL_MS 500

static uint16_t blink_timer = 0;
static bool led_blink_state = false;

enum layers{
  MAC_BASE,
  WIN_BASE,
  MAC_FN1,
  WIN_FN1,
  FN2,
  MIDI
};


extern MidiDevice midi_device;

// Контексты TapDance должны быть глобальными

td_ctx_t win_fn_left_ctx = {
    .fn1_layer = WIN_FN1,	// Привязка функции к слою
    .tap_key = KC_LWIN,	// Левая клавиша Win
    .is_active = false	
};

td_ctx_t win_fn_right_ctx = {
    .fn1_layer = WIN_FN1,	// Можно указать другой слой если нужно
    .tap_key = KC_RGUI,	// Правая клавиша Win
    .is_active = false
};

td_ctx_t mac_fn_left_ctx = {
    .fn1_layer = MAC_FN1,
    .tap_key = KC_LOPTN, // Левая клавиша Option
    .is_active = false
};

td_ctx_t mac_fn_right_ctx = {
    .fn1_layer = MAC_FN1,    
    .tap_key = KC_ROPTN, // Правая клавиша  Option
    .is_active = false
};



// ==================================================
// Массив действий Tap Dance
// ==================================================

tap_dance_action_t tap_dance_actions[] = {

	[TD_L1L] = { // Левый Option, переключение на MacFn
        .fn = { NULL, td_finished, td_reset },
        .user_data = &mac_fn_left_ctx
    },
	[TD_L1R] = {  // Правый Option, переключение на MacFn
		.fn = { NULL, td_finished, td_reset },
		.user_data = &mac_fn_right_ctx
    },

    [TD_L3L] = { // Левый Win, переключение на WinFn
        .fn = { NULL, td_finished, td_reset },
        .user_data = &win_fn_right_ctx
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
     KC_GRV,     KC_1,      KC_2,      KC_3,      KC_4,     KC_5,      KC_6,      KC_7,      KC_8,      KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PSCR,
     KC_TAB,     KC_Q,      KC_W,      KC_E,      KC_R,     KC_T,      KC_Y,      KC_U,      KC_I,      KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_CALC,
     KC_CAPS,    KC_A,      KC_S,      KC_D,      KC_F,     KC_G,      KC_H,      KC_J,      KC_K,      KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             KC_DEL,
     KC_LSFT,    KC_Z,      KC_X,      KC_C,      KC_V,     KC_B,      KC_N,      KC_M,      KC_COMM,   KC_DOT,   KC_SLSH,                      KC_RSFT,  KC_UP,    MO(FN2),
     KC_LCTL,    KC_LOPTN,  KC_LCMMD,                                  KC_SPC,                                    KC_RCMMD,MO(MAC_FN1),KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),


	/*Layer 1
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
     *│ Control │   Fn1   │   Alt   │                     Space                       │  Alt  │Win/Fn1│Control│ ←     │ ↓     │ →     │
     *│58       │59       │60       │61                                               │62     │63     │64     │65     │66     │67     │
     *└─────────┴─────────┴─────────┴─────────────────────────────────────────────────┴───────┴───────┴───────┴───────┴───────┴───────┘
    */
[WIN_BASE] = LAYOUT_ansi_68(
     KC_GRV,     KC_1,      KC_2,      KC_3,      KC_4,     KC_5,      KC_6,      KC_7,      KC_8,      KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PSCR,
     KC_TAB,     KC_Q,      KC_W,      KC_E,      KC_R,     KC_T,      KC_Y,      KC_U,      KC_I,      KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_CALC,
     KC_CAPS,    KC_A,      KC_S,      KC_D,      KC_F,     KC_G,      KC_H,      KC_J,      KC_K,      KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             KC_DEL,
     KC_LSFT,    KC_Z,      KC_X,      KC_C,      KC_V,     KC_B,      KC_N,      KC_M,      KC_COMM,   KC_DOT,   KC_SLSH,                      KC_RSFT,  KC_UP,    MO(FN2),
     KC_LCTL,  MO(WIN_FN1), KC_LALT,                                   KC_SPC,                                    KC_RALT, TD(TD_L3R), KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),


	/*Layer 2
     *┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────────────┬───────┐
     *│ Esc   │ F1    │ F2    │ F3    │ F4    │ F5    │ F6    │ F7    │ F8    │ F9    │ F10   │ F11   │ F12   │               │       │
     *│00     │01     │01     │03     │04     │05     │06     │07     │08     │09     │10     │11     │12     │13             │14     │
     *├───────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───────────┼───────┤
     *│           │ Ms L B│ MsUp  │ Ms R B│       │       │       │       │  Ins  │ScrLock│ Pause │       │       │           │       │ 
     *│15         │16     │17     │18     │19     │20     │21     │22     │23     │24     │25     │26     │27     │28         │29     │
     *├───────────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴───────────┼───────┤
     *│    RGB      │ MsLeft│ MsDown│MsRight│       │       │       │       │       │       │       │       │                 │       │
     *│30           │31     │32     │33     │34     │35     │36     │37     │38     │39     │40     │41     │42               │43     │
     *├─────────────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴─────────┬───────┼───────┤
     *│                 │Bright↑│Bright↓│ RGB ↑ │ RGB ↓ │Bat Lvl│Mission│<<||>> │ Vol↓/M│ Vol↑  │ LPad  │             │ PgUp  │       │ 
     *│44               │45     │46     │47     │48     │49     │50     │51     │52     │53     │54     │55           │56     │57     │
     *├─────────┬───────┴─┬─────┴───┬───┴───────┴───────┴───────┴───────┴───────┴─────┬─┴─────┬─┴─────┬─┴─────┬───────┼───────┼───────┤
     *│         │         │         │                                                 │       │       │       │ Home  │ PgDwn │ End   │
     *│58       │59       │60       │61                                               │62     │63     │64     │65     │66     │67     │
     *└─────────┴─────────┴─────────┴─────────────────────────────────────────────────┴───────┴───────┴───────┴───────┴───────┴───────┘
    */
[MAC_FN1] = LAYOUT_ansi_68(
     KC_ESC,     KC_F1,     KC_F2,     KC_F3,     KC_F4,    KC_F5,     KC_F6,     KC_F7,     KC_F8,     KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,             _______,
     _______,  KC_MS_BTN1, KC_MS_UP,  KC_MS_BTN2, _______,  _______,   _______,   _______,   KC_INS,    KC_SCRL,  KC_PAUS,  _______,  _______,  _______,             _______,
     RGB_TOG, KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT,_______,  _______,   _______,   _______,   _______,   _______,  _______,  _______,            _______,             _______,
     _______,    KC_BRID,   KC_BRIU,   KC_MCTRL,  KC_LNPAD, BAT_LVL,  KC_MCTRL,TD(TD_PPPN), TD(TD_VOLM), KC_VOLU, KC_LNPAD,                     _______,  KC_PGUP,   _______,
     _______,    _______,   _______,                                   _______,                                   _______,  _______,  _______,  KC_HOME,  KC_PGDN,   KC_END),


	/*Layer 3
     *┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────────────┬───────┐
     *│ Esc   │ F1    │ F2    │ F3    │ F4    │ F5    │ F6    │ F7    │ F8    │ F9    │ F10   │ F11   │ F12   │               │       │
     *│00     │01     │01     │03     │04     │05     │06     │07     │08     │09     │10     │11     │12     │13             │14     │
     *├───────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───────────┼───────┤
     *│           │ Ms L B│ MsUp  │ Ms R B│       │       │       │       │  Ins  │ScrLock│ Pause │       │       │           │       │ 
     *│15         │16     │17     │18     │19     │20     │21     │22     │23     │24     │25     │26     │27     │28         │29     │
     *├───────────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴───────────┼───────┤
     *│  RGB On/Off │ MsLeft│ MsDown│MsRight│       │       │       │       │       │       │       │       │                 │       │
     *│30           │31     │32     │33     │34     │35     │36     │37     │38     │39     │40     │41     │42               │43     │
     *├─────────────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴─────────┬───────┼───────┤
     *│                 │Bright↑│Bright↓│ RGB ↑ │ RGB ↓ │Bat Lvl│       │<<||>> │ Vol↓/M│ Vol↑  │ File  │             │ PgUp  │       │ 
     *│44               │45     │46     │47     │48     │49     │50     │51     │52     │53     │54     │55           │56     │57     │
     *├─────────┬───────┴─┬─────┴───┬───┴───────┴───────┴───────┴───────┴───────┴─────┬─┴─────┬─┴─────┬─┴─────┬───────┼───────┼───────┤
     *│         │         │         │                                                 │       │       │       │ Home  │ PgDwn │ End   │
     *│58       │59       │60       │61                                               │62     │63     │64     │65     │66     │67     │
     *└─────────┴─────────┴─────────┴─────────────────────────────────────────────────┴───────┴───────┴───────┴───────┴───────┴───────┘
    */
[WIN_FN1] = LAYOUT_ansi_68(
     KC_ESC,     KC_F1,     KC_F2,     KC_F3,     KC_F4,    KC_F5,     KC_F6,     KC_F7,     KC_F8,     KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,             _______,     
     _______,  KC_MS_BTN1, KC_MS_UP,  KC_MS_BTN2, _______,  _______,   _______,   _______,   KC_INS,    KC_SCRL,  KC_PAUS,  _______,  _______,  _______,             _______,
     RGB_TOG, KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT,_______,  _______,   _______,   _______,   _______,   _______,  _______,  _______,            _______,             _______,
     _______,    KC_BRID,   KC_BRIU,   RGB_VAD,   RGB_VAI,  BAT_LVL,   KC_TASK, TD(TD_PPPN),TD(TD_VOLM),KC_VOLU,  KC_FILE,                      _______,  KC_PGUP,   _______,
     _______,    _______,   _______,                                   _______,                                   _______,  _______,  _______,  KC_HOME,  KC_PGDN,   KC_END),

	/*Layer 4
     *┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────────────┬───────┐
     *│ Esc   │ Num 1 │ Num 2 │ Num 3 │ Num 4 │ Num 5 │ Num 6 │ Num 7 │ Num 8 │ Num 9 │ Num 0 │ Num - │ Num + │               │       │
     *│00     │01     │01     │03     │04     │05     │06     │07     │08     │09     │10     │11     │12     │13             │14     │
     *├───────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───────────┼───────┤
     *│RGB On/Off││BTHost1│BTHost2│BTHost3│Dongle │       │       │       │       │       │       │       │       │           │       │ 
     *│15         │16     │17     │18     │19     │20     │21     │22     │23     │24     │25     │26     │27     │28         │29     │
     *├───────────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴───────────┼───────┤
     *│             │       │       │       │       │       │       │       │       │       │       │       │                 │       │
     *│30           │31     │32     │33     │34     │35     │36     │37     │38     │39     │40     │41     │42               │43     │
     *├─────────────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴─────────┬───────┼───────┤
     *│                 │       │       │       │       │RGB_spd│NumLock│Fn MIDI│NKRO   │       │       │             │       │       │ 
     *│44               │45     │46     │47     │48     │49     │50     │51     │52     │53     │54     │55           │56     │57     │
     *├─────────┬───────┴─┬─────┴───┬───┴───────┴───────┴───────┴───────┴───────┴─────┬─┴─────┬─┴─────┬─┴─────┬───────┼───────┼───────┤
     *│         │         │         │                                                 │       │       │       │       │       │       │
     *│58       │59       │60       │61                                               │62     │63     │64     │65     │66     │67     │
     *└─────────┴─────────┴─────────┴─────────────────────────────────────────────────┴───────┴───────┴───────┴───────┴───────┴───────┘
    */
[FN2] = LAYOUT_ansi_68(
     KC_ESC,     KC_P1,     KC_P2,     KC_P3,     KC_P4,    KC_P5,     KC_P6,     KC_P7,     KC_P8,     KC_P9,     KC_P0,   KC_PMNS,  KC_PPLS,   _______,             _______,
     RGB_TOG,    BT_HST1,   BT_HST2,   BT_HST3,   P2P4G,    _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,             _______,
     _______,    RGB_MOD,   RGB_VAI,   RGB_HUI,   RGB_SAI,  RGB_SPI,   _______,   _______,   _______,   _______,  _______,  _______,            _______,             _______,
     _______,    RGB_RMOD,  RGB_VAD,   RGB_HUD,   RGB_SAD,  RGB_SPD,   KC_NUM,    TG(MIDI),   NK_TOGG,   _______,  _______,                      _______,  _______,   _______,
     _______,    _______,   _______,                                   _______,                                   _______,  _______,  _______,  _______,  _______,   _______),
	 
	 /*Layer 5
     *┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────────────┬───────┐
     *│ T0    │ T1    │ T2    │ T3    │ T4    │ T5    │ T6    │ T7    │ T8    │ T9    │ T10   │       │       │               │       │
     *│00     │01     │01     │03     │04     │05     │06     │07     │08     │09     │10     │11     │12     │13             │14     │
     *├───────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───────────┼───────┤
     *│           │       │       │       │       │       │       │       │       │       │       │       │       │           │       │ 
     *│15         │16     │17     │18     │19     │20     │21     │22     │23     │24     │25     │26     │27     │28         │29     │
     *├───────────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴─┬─────┴───────────┼───────┤
     *│             │       │       │       │       │       │       │       │       │       │       │       │                 │       │
     *│30           │31     │32     │33     │34     │35     │36     │37     │38     │39     │40     │41     │42               │43     │
     *├─────────────┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴───┬───┴─────────┬───────┼───────┤
     *│                 │       │       │       │       │       │       │MIDIOff│       │       │       │             │       │       │ 
     *│44               │45     │46     │47     │48     │49     │50     │51     │52     │53     │54     │55           │56     │57     │
     *├─────────┬───────┴─┬─────┴───┬───┴───────┴───────┴───────┴───────┴───────┴─────┬─┴─────┬─┴─────┬─┴─────┬───────┼───────┼───────┤
     *│         │         │ Back    │                        GO                       │       │       │       │       │       │       │
     *│58       │59       │60       │61                                               │62     │63     │64     │65     │66     │67     │
     *└─────────┴─────────┴─────────┴─────────────────────────────────────────────────┴───────┴───────┴───────┴───────┴───────┴───────┘
    */
[MIDI] = LAYOUT_ansi_68(
     KC_F13,     KC_F14,    KC_F15,    KC_F16,    KC_F17,   KC_F18,     KC_F19,    KC_F20,    KC_F21,   KC_F22,   KC_F23,   _______,  _______,  _______,             _______,
     _______,    _______,   _______,   _______,   _______,  _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,             _______,
     _______,    _______,   _______,   _______,   _______,  _______,   _______,   _______,   _______,   _______,  _______,  _______,            _______,             _______,
     _______,    _______,   _______,   _______,   _______,  _______,   _______,   TG(MIDI),   _______,   _______,  _______,                      _______,  _______,   _______,
     _______,    _______,   KC_MRWD,                                   KC_F24,                                    _______,  _______,  _______,  _______,  _______,   _______),
};

// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	if (keycode == KC_Q && record->event.pressed) {
		 // Проверяем, зажат ли левый или правый Alt
        uint8_t mods = get_mods();
        // Только левый Alt, без других модификаторов
        uint8_t allowed_mods = MOD_BIT(KC_LALT);
        uint8_t forbidden_mods = MOD_BIT(KC_LCTL) | MOD_BIT(KC_LSFT) | 
                                 MOD_BIT(KC_LGUI) | MOD_BIT(KC_RALT);
            // Отправляем Alt+F4
			if ((mods & allowed_mods) && !(mods & forbidden_mods)) {
            tap_code16(KC_F4); // tap_code16 автоматически обрабатывает модификаторы
            return false;// Подавляем стандартное поведение Q
        }
           
    }
/*
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	if (keycode == KC_Q && record->event.pressed) {
		 // Проверяем, зажат ли левый или правый Alt
        uint8_t mods = get_mods();
        // Только левый Alt, без других модификаторов
        uint8_t allowed_mods = MOD_BIT(KC_LALT);
        uint8_t forbidden_mods = MOD_BIT(KC_LCTL) | MOD_BIT(KC_LSFT) | 
                                 MOD_BIT(KC_LGUI) | MOD_BIT(KC_RALT);
            // Отправляем Alt+F4
			if ((mods & allowed_mods) && !(mods & forbidden_mods)) {
            tap_code16(KC_F4); // tap_code16 автоматически обрабатывает модификаторы
            return false;// Подавляем стандартное поведение Q
        }
           
    }
	*/
	
	// Реализация MIDI клвавиш. Синтаксис: midi_send_cc(Девайс, uint8_t канал, uint8_t номер, uint8_t значение);
    switch (keycode) {
        case KC_F13: // T0
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 0);
            }
            return false;

        case KC_F14: // T1
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 13);
            }
            return false;
            
        case KC_F15: // T2
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 26);
            }
            return false;
            
        case KC_F16: // T3
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 39);
            }
            return false;
        
        case KC_F17: // T4
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 51);
            }
            return false;
            
        case KC_F18: // T5
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 64);
            }
            return false;
            
        case KC_F19: // T6
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 77);
            }
            return false;
            
        case KC_F20: // T7
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 89);
            }
            return false;
            
        case KC_F21: // T8
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 102);
            }
            return false;
            
        case KC_F22: // T9
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 115);
            }
            return false;
            
        case KC_F23: // T10
            if (record->event.pressed) {
                midi_send_cc(&midi_device, midi_config.channel, 16, 127);
            }
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
    // 3. Для всех остальных клавиш - стандартная обработка
    return true;
}



void keyboard_post_init_user(void) {

     // Подсветка #ff783d Тёплый белый (RGB (255, 190, 143) HSV (25, 44, 100) HEX (ffbe8f)
     // #ff82a8 Холодный белый
     rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR); //_noeeprom
    rgb_matrix_sethsv(24, 34, 100); //_noeeprom
}


// Индикация
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
	if (get_highest_layer(layer_state) != 4) {
        blink_timer = 0;
        led_blink_state = false;
    }
/*CapsLock Indication */ 
    if (host_keyboard_led_state().caps_lock) {                  

        RGB_MATRIX_INDICATOR_SET_COLOR(00, 200, 0, 0); // Ё
        RGB_MATRIX_INDICATOR_SET_COLOR(16, 200, 0, 0); // Top
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

        RGB_MATRIX_INDICATOR_SET_COLOR(30, 250, 0, 0); //CapsLock
		RGB_MATRIX_INDICATOR_SET_COLOR(31, 200, 0, 0); //Mid
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

        RGB_MATRIX_INDICATOR_SET_COLOR(45, 200, 0, 0); // Bottom
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
	
    // Layer 2
    if (get_highest_layer(layer_state) == 2) {
		RGB_MATRIX_INDICATOR_SET_COLOR(00, 98, 255, 187); //Esc
		RGB_MATRIX_INDICATOR_SET_COLOR(01, 98, 255, 187); //F1
		RGB_MATRIX_INDICATOR_SET_COLOR(02, 98, 255, 187); //F2
		RGB_MATRIX_INDICATOR_SET_COLOR(03, 98, 255, 187); //F3
		RGB_MATRIX_INDICATOR_SET_COLOR(04, 98, 255, 187); //F4
		RGB_MATRIX_INDICATOR_SET_COLOR(05, 98, 255, 187); //F5
		RGB_MATRIX_INDICATOR_SET_COLOR(06, 98, 255, 187); //F6
		RGB_MATRIX_INDICATOR_SET_COLOR(07, 98, 255, 187); //F7
		RGB_MATRIX_INDICATOR_SET_COLOR(8, 98, 255, 187); //F8
		RGB_MATRIX_INDICATOR_SET_COLOR(9, 98, 255, 187); //F9
		RGB_MATRIX_INDICATOR_SET_COLOR(10, 98, 255, 187); //F10
		RGB_MATRIX_INDICATOR_SET_COLOR(11, 98, 255, 187); //F11
		RGB_MATRIX_INDICATOR_SET_COLOR(12, 98, 255, 187); //F12
		
		RGB_MATRIX_INDICATOR_SET_COLOR(23, 200, 20, 20); //Ins
		RGB_MATRIX_INDICATOR_SET_COLOR(24, 200, 20, 20); //Scroll Lock
		RGB_MATRIX_INDICATOR_SET_COLOR(25, 200, 20, 20); //Pause
		
		
		RGB_MATRIX_INDICATOR_SET_COLOR(16, 255, 130, 168); // Mouse left Button
		RGB_MATRIX_INDICATOR_SET_COLOR(17, 255, 130, 168); // Mouse Up
		RGB_MATRIX_INDICATOR_SET_COLOR(18, 255, 130, 168); // Mouse Right Button
		RGB_MATRIX_INDICATOR_SET_COLOR(31, 255, 130, 168); // Mouse left
		RGB_MATRIX_INDICATOR_SET_COLOR(32, 255, 130, 168); // Mouse Down
		RGB_MATRIX_INDICATOR_SET_COLOR(33, 255, 130, 168); // Mouse Right

		RGB_MATRIX_INDICATOR_SET_COLOR(45, 255, 86, 3); // Bottom 
		RGB_MATRIX_INDICATOR_SET_COLOR(46, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(47, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(48, 255, 86, 3); 
		RGB_MATRIX_INDICATOR_SET_COLOR(49, 50, 255, 50);
		RGB_MATRIX_INDICATOR_SET_COLOR(50, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(51, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(52, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(53, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(54, 255, 86, 3);
		

	}
    // Layer 3
    if (get_highest_layer(layer_state) == 3) {
		RGB_MATRIX_INDICATOR_SET_COLOR(00, 255, 86, 3); //Esc
		RGB_MATRIX_INDICATOR_SET_COLOR(01, 98, 255, 187); //F1
		RGB_MATRIX_INDICATOR_SET_COLOR(02, 98, 255, 187); //F2
		RGB_MATRIX_INDICATOR_SET_COLOR(03, 98, 255, 187); //F3
		RGB_MATRIX_INDICATOR_SET_COLOR(04, 98, 255, 187); //F4
		RGB_MATRIX_INDICATOR_SET_COLOR(05, 98, 255, 187); //F5
		RGB_MATRIX_INDICATOR_SET_COLOR(06, 98, 255, 187); //F6
		RGB_MATRIX_INDICATOR_SET_COLOR(07, 98, 255, 187); //F7
		RGB_MATRIX_INDICATOR_SET_COLOR(8, 98, 255, 187); //F8
		RGB_MATRIX_INDICATOR_SET_COLOR(9, 98, 255, 187); //F9
		RGB_MATRIX_INDICATOR_SET_COLOR(10, 98, 255, 187); //F10
		RGB_MATRIX_INDICATOR_SET_COLOR(11, 98, 255, 187); //F11
		RGB_MATRIX_INDICATOR_SET_COLOR(12, 98, 255, 187); //F12
		
		RGB_MATRIX_INDICATOR_SET_COLOR(23, 200, 0, 0); //Ins
		RGB_MATRIX_INDICATOR_SET_COLOR(24, 200, 0, 0); //Scroll Lock
		RGB_MATRIX_INDICATOR_SET_COLOR(25, 200, 0, 0); //Pause
		
		RGB_MATRIX_INDICATOR_SET_COLOR(16, 255, 130, 168); // Mouse left Button
		RGB_MATRIX_INDICATOR_SET_COLOR(17, 255, 130, 168); // Mouse Up
		RGB_MATRIX_INDICATOR_SET_COLOR(18, 255, 130, 168); // Mouse Right Button
		RGB_MATRIX_INDICATOR_SET_COLOR(31, 255, 130, 168); // Mouse left
		RGB_MATRIX_INDICATOR_SET_COLOR(32, 255, 130, 168); // Mouse Down
		RGB_MATRIX_INDICATOR_SET_COLOR(33, 255, 130, 168); // Mouse Right

		RGB_MATRIX_INDICATOR_SET_COLOR(45, 255, 86, 3); // Bottom 
		RGB_MATRIX_INDICATOR_SET_COLOR(46, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(47, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(48, 255, 86, 3); 
		RGB_MATRIX_INDICATOR_SET_COLOR(49, 50, 255, 50);
		RGB_MATRIX_INDICATOR_SET_COLOR(50, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(51, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(52, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(53, 255, 86, 3);
		RGB_MATRIX_INDICATOR_SET_COLOR(54, 255, 86, 3);
		
		RGB_MATRIX_INDICATOR_SET_COLOR(56, 255, 130, 168); //Arrow ↑
		RGB_MATRIX_INDICATOR_SET_COLOR(65, 255, 130, 168); //Arrow ←
		RGB_MATRIX_INDICATOR_SET_COLOR(66, 255, 130, 168); //Arrow ↓
		RGB_MATRIX_INDICATOR_SET_COLOR(67, 255, 130, 168); //Arrow →

    }
    // Layer 4
    else if (get_highest_layer(layer_state) == 4) {

		RGB_MATRIX_INDICATOR_SET_COLOR(15, 98, 255, 187); //RGB TOG
		RGB_MATRIX_INDICATOR_SET_COLOR(16, 0, 0, 168); //беспроводное соединение BT-1
		RGB_MATRIX_INDICATOR_SET_COLOR(17, 0, 0, 168); //беспроводное соединение BT-2
		RGB_MATRIX_INDICATOR_SET_COLOR(18, 0, 0, 168); //беспроводное соединение BT-3
		RGB_MATRIX_INDICATOR_SET_COLOR(19, 0, 0, 168); //беспроводное соединение свисток
		
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
		// === Индикация NumLock: зелёный при вкл., мигающий красный при выкл. ===
        led_t led_state = host_keyboard_led_state();
        if (led_state.num_lock) {
            RGB_MATRIX_INDICATOR_SET_COLOR(NUM_LOCK_LED_INDEX, 0x00, 0xFF, 0x00); // зелёный
            blink_timer = 0;
            led_blink_state = false;
        } else {
            if (timer_elapsed(blink_timer) > BLINK_INTERVAL_MS) {
                led_blink_state = !led_blink_state;
                blink_timer = timer_read();
            }
            if (led_blink_state) {
                RGB_MATRIX_INDICATOR_SET_COLOR(NUM_LOCK_LED_INDEX, 0xFF, 0x00, 0x00); // красный
            } else {
                RGB_MATRIX_INDICATOR_SET_COLOR(NUM_LOCK_LED_INDEX, 0x00, 0x00, 0x00); // тёмный
            }
        }
    }
	// Layer 5
    else if (get_highest_layer(layer_state) == 5) {

		RGB_MATRIX_INDICATOR_SET_COLOR(00, 0, 255, 0); //
		RGB_MATRIX_INDICATOR_SET_COLOR(1, 0, 255, 0); //
		RGB_MATRIX_INDICATOR_SET_COLOR(2, 0, 255, 0); //
		RGB_MATRIX_INDICATOR_SET_COLOR(3, 0, 255, 0); //
		RGB_MATRIX_INDICATOR_SET_COLOR(4, 0, 255, 0); //
		RGB_MATRIX_INDICATOR_SET_COLOR(5, 0, 255, 0);
		RGB_MATRIX_INDICATOR_SET_COLOR(6, 0, 255, 0);
		RGB_MATRIX_INDICATOR_SET_COLOR(7, 0, 255, 0);
		RGB_MATRIX_INDICATOR_SET_COLOR(8, 0, 255, 0);
		RGB_MATRIX_INDICATOR_SET_COLOR(9, 0, 255, 0);
		RGB_MATRIX_INDICATOR_SET_COLOR(10, 0, 255, 0);
		RGB_MATRIX_INDICATOR_SET_COLOR(51, 200, 0, 0);

		RGB_MATRIX_INDICATOR_SET_COLOR(60, 0, 255, 0);
		RGB_MATRIX_INDICATOR_SET_COLOR(61, 0, 255, 0);
		

    }
    return false;
}

