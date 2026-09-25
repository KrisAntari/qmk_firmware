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
BOOTMAGIC_ENABLE = yes				# Для прошивки. Вариант Light не компилится с ошибкой "is not a valid type of magic".
TAP_DANCE_ENABLE = yes				# Включить TapDance
RGB_MATRIX_ENABLE = yes
#RGB_MATRIX_TIMEOUT = 300000			# выключать RGB через 5 мин простоя (300000 мс)
RGB_MATRIX_MAXIMUM_BRIGHTNESS = 128	# ограничить макс. яркость (0-255)
DYNAMIC_MACRO_ENABLE = no			# Запись макросов на лету
UNICODE_ENABLE = yes				
# VIA_ENABLE = yes            # Без RAW_ENABLE не работает, а эндпоинтов на всех не хватает.
# COMBO_ENABLE = yes          # Включить Combo. Ничему не мешает, но пока не используется и функции не объявлены. 
