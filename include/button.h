#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/io.h>

extern volatile uint8_t pb_state;
extern volatile uint8_t pb_state_c;
extern volatile uint8_t pb_state_r;

extern volatile uint8_t pb_debounced_state;
extern volatile uint8_t pb_step_cmp;

extern volatile uint8_t pb_changed;
extern volatile uint8_t pb_falling;
extern volatile uint8_t pb_rising;
extern volatile uint8_t pb_released;
volatile uint8_t pb_current;

void button_init(void);