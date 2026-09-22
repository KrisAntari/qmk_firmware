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

#pragma once






#ifdef RGB_MATRIX_ENABLE
/* RGB Matrix driver configuration */
#    define RGB_MATRIX_LED_COUNT 68
#    define DRIVER_COUNT 2
#    define DRIVER_CS_PINS \
        { B8, B9 }

/* Scan phase of led driver set as MSKPHASE_9CHANNEL(defined as 0x03 in snled27351.h) */
#    define SNLED27351_PHASE_CHANNEL MSKPHASE_9CHANNEL
/* Set LED driver current */
#    define SNLED27351_CURRENT_TUNE \
        { 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8, 0xC8 } // Было { 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14 }


/* Set to infinit, which is use in USB mode by default */
#    define RGB_MATRIX_TIMEOUT RGB_MATRIX_TIMEOUT_INFINITE
/* Allow shutdown of led driver to save power */
#    define RGB_MATRIX_DRIVER_SHUTDOWN_ENABLE
/* Turn off backlight on low brightness to save power */
#    define RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL 48

/* Indications */
//#    define SCROLL_LOCK_INDEX 24
//#    define DIM_CAPS_LOCK //Гасит диод CapsLock когда модификатор включён
#    define CAPS_LOCK_INDEX 30
#    define BT_HOST_LED_MATRIX_LIST \
        { 16, 17, 18 }
#    define P2P4G_HOST_LED_MATRIX_LIST \
        { 19 }
#    define LOW_BAT_IND_INDEX \
        { 61 }

#    define RGB_MATRIX_KEYPRESSES
#    define RGB_MATRIX_FRAMEBUFFER_EFFECTS

/* УСТАНОВКА ЗАДЕРЖКИ НАЖАТИЯ КЛАВИШ. НУЖНО ДЛЯ TAP_DANCE */
#	define TAPPING_TERM 200
#	define TAPPING_TOGGLE 2 // Количество нажатий для переключения
#	define TAPPING_TERM_PER_KEY
//#	define IGNORE_MOD_TAP_INTERRUPT // Важно для предотвращения ложных срабатываний
#	define RETRO_TAPPING // Для удобства двойных нажатий
#	define PERMISSIVE_HOLD
#	define TAP_CODE_DELAY 10 // Задержка отправки кодов (10ms)

/* Включение поддержки MIDI */
#	define MIDI_BASIC
#	define MIDI_ADVANCED

/* Подсветка */
#define RGBLIGHT_LAYERS
#define RGBLIGHT_LAYER_BLINK
#define RGBLIGHT_LAYERS_OVERRIDE_RGB_OFF
#define RGBLIGHT_LAYERS_POST_UPDATE_DELAY 50 // Задержка обновления слоев (мс)


/* Insert LED  */
//#define INSERT_LED_INDEX 41

//Количество слоёв
//#define DYNAMIC_KEYMAP_LAYER_COUNT 6

#endif
