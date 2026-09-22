# Экономим эндпоинты. Всего для пользователя доступно 5.

# NKRO_ENABLE = no				# Помимо NKRO отключает некоторую внутреннюю логику. 
# EXTRAKEY_ENABLE = no			# Отключает медиаклавиши и системные клавиши и ничего не экономит
CONSOLE_ENABLE = no				# Отключает отладочную консоль через USB	Экономится 2 USB-эндпоинта
RAW_ENABLE = no					# Отключает RAW HID							Экономится 2 USB-эндпоинта
VIRTSER_ENABLE = no				# Отключает виртуальный COM-порт			Экономится 2 USB-эндпоинта
JOYSTICK_ENABLE = no			# Отключает поддержку геймпада				Экономится 1 USB-эндпоинт
DIGITIZER_ENABLE = no			# Отключает поддержку графического планшета	Экономится 1 USB-эндпоинт
PROGRAMMABLE_BUTTON_ENABLE = no	# Отключает программируемые кнопки			Экономится 1 USB-эндпоинт

# Включаем нужное

MIDI_ENABLE = yes
BOOTMAGIC_ENABLE = yes			# Включение режима прошивки через esc
TAP_DANCE_ENABLE = yes			# Включить TapDance
SRC += tap_dance.c				# Где брать функции для компиляции для TD
RGB_MATRIX_ENABLE = yes
# VIA_ENABLE = yes            # Без RAW_ENABLE не работает, а эндпоинтов на всех не хватает. 
DYNAMIC_MACRO_ENABLE = yes  # Запись макросов на лету
# COMBO_ENABLE = yes          # Включить Combo. Ничему не мешает, но пока не используется и функции не объявлены. 
UNICODE_ENABLE = yes
