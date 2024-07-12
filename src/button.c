#include "avr/io.h"
#include "avr/interrupt.h"
#include <stdint.h>

#include "buzzer.h"
#include "timer.h"
#include "display.h"

// Variables to store the state of the push buttons
volatile uint8_t pb_state = 0xFF;
volatile uint8_t pb_state_c = 0xFF;
volatile uint8_t pb_state_r = 0xFF;

// Variables to store the debounced state of the push buttons
volatile uint8_t pb_debounced_state = 0xFF;

// Variable to compare the pushbutton with the instruction value
volatile uint8_t pb_step_cmp = 0xFF; 

// Variables to store the rising and falling edge of pushbuttons
volatile uint8_t pb_changed;
volatile uint8_t pb_falling;
volatile uint8_t pb_rising;
volatile uint8_t pb_released;
volatile uint8_t pb_current;

void button_init(void)
{
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm; // enable pull-up resistor on S1
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm; // enable pull-up resistor on S2
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm; // enable pull-up resistor on S3
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // enable pull-up resistor on S4
}
