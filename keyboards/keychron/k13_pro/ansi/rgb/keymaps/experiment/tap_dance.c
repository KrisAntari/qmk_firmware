// tap_dance.c
#include "tap_dance.h"
#include "action_layer.h"

static struct {
    const td_config_t *config;
    uint16_t last_key;
    bool is_holding;
} td_state;

void td_finished(tap_dance_state_t *state, void *user_data) {
    td_state.config = (const td_config_t *)user_data;
    
    if (state->pressed) { // Обработка удержания
        if (!td_state.is_holding) {
            td_state.is_holding = true;
            
            // Выбираем действие в зависимости от количества тапов
            switch(td_state.tap_count) {
                case 1: td_state.last_key = td_state.config->hold_1; break;
                case 2: td_state.last_key = td_state.config->hold_2; break;
                case 3: td_state.last_key = td_state.config->hold_3; break;
                case 4: td_state.last_key = td_state.config->hold_4; break;
                case 5: td_state.last_key = td_state.config->hold_5; break;
            }
			register_code16(td_state.last_key);
        }
		
    } else { // Обработка тапов
        
        // Выбираем действие в зависимости от количества тапов
        switch(state->count) {
            case 1: tap_code16(td_state.config->tap_1); break;
            case 2: tap_code16(td_state.config->tap_2); break;
            case 3: tap_code16(td_state.config->tap_3); break;
            case 4: tap_code16(td_state.config->tap_4); break;
            case 5: tap_code16(td_state.config->tap_5); break;
        }

    }
}

// Обработчик сброса состояния
void td_reset(tap_dance_state_t *state, void *user_data) {
    if (td_state.is_holding) {
        unregister_code16(td_state.last_key);
        td_state.is_holding = false;
    }
}
