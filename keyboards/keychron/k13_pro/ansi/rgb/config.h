/*Авторское право 2023 @ Keychron (https://www.keychron.com)
*
* Эта программа является свободным программным обеспечением: вы можете распространять ее и/или модифицировать
* она доступна на условиях GNU General Public License, опубликованных
* Фондом свободного программного обеспечения (Free Software Foundation), либо в версии 2 Лицензии, либо
(по вашему выбору) в любой более поздней версии.
*
* Эта программа распространяется в надежде, что она будет полезна,
* но БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ; даже без подразумеваемых гарантий
КОММЕРЧЕСКОЙ ЦЕННОСТИ или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННОЙ ЦЕЛИ. Более подробную информацию смотрите в разделе
* GNU General Public License.
*
* Вы должны были получить копию GNU General Public License
* вместе с этой программой. Если нет, смотрите <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef RGB_MATRIX_ENABLE
/* RGB Matrix Driver Configuration */
#    define DRIVER_COUNT 2
#    define DRIVER_ADDR_1 0b1110111
#    define DRIVER_ADDR_2 0b1110100
#    define DRIVER_1_LED_TOTAL 47
#    define DRIVER_2_LED_TOTAL 43
#    define RGB_MATRIX_LED_COUNT (DRIVER_1_LED_TOTAL + DRIVER_2_LED_TOTAL)

/* Установите значение infinit, которое по умолчанию используется в режиме USB */
#    define RGB_MATRIX_TIMEOUT RGB_MATRIX_TIMEOUT_INFINITE

/* Позволяет выключать драйвер для экономии энергии */
#    define RGB_MATRIX_DRIVER_SHUTDOWN_ENABLE

/* Выключите подсветку, если значение яркости низкое */
#    define RGB_MATRIX_BRIGHTNESS_TURN_OFF_VAL 48

#    define LOW_BAT_IND_INDEX 82

// Режимы анимации RGB-матрицы. Explicitly enabled
// Полный список эффектов смотрите здесь:
// https://docs.qmk.fm/#/feature_rgb_matrix?id=rgb-matrix-effects

#    define RGB_MATRIX_KEYPRESSES			
#    define RGB_MATRIX_FRAMEBUFFER_EFFECTS	
//#	define RGB_MATRIX_SOLID_COLOR = 1, Попытка назначить цвет подсветки. Провал.


/* Используйте первые 9 каналов светодиодного драйвера */
#    define PHASE_CHANNEL MSKPHASE_9CHANNEL

/* Set LED driver current */
#    define SNLED27351_CURRENT_TUNE \
        { 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12 }
		
		
/* УСТАНОВКА ЗАДЕРЖКИ НАЖАТИЯ КЛАВИШ. НУЖНО ДЛЯ TAP_DANCE */
#	define TAPPING_TERM 200
#	define TAPPING_TOGGLE 2 // Количество нажатий для переключения
#	define TAPPING_TERM_PER_KEY
#	define IGNORE_MOD_TAP_INTERRUPT // Важно для предотвращения ложных срабатываний
#	define RETRO_TAPPING // Для удобства двойных нажатий
#	define PERMISSIVE_HOLD
#	define TAP_CODE_DELAY 10 // Задержка отправки кодов (10ms)
/* One Shot Layer: */

//#	define ONESHOT_TAP_TOGGLE 1 // Активировать слой до следующего нажатия

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
#define DYNAMIC_KEYMAP_LAYER_COUNT 6


#endif
