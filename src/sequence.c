#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "sequence.h"
#include "display.h"
#include "button.h"
#include "buzzer.h"

// Variables to store the sequence
volatile uint16_t sequence_length = 1;
volatile uint8_t user_score = 1;
volatile uint8_t sequence_index = 0;

// Variables to store the LFSR
volatile uint32_t SEED = STUDENT_ID; 
volatile uint32_t STATE_LFSR = STUDENT_ID;
volatile uint32_t MASK = 0xE2023CAB;
volatile uint8_t STEP = 0;

// Generates the next step in the sequence
void next_step(void)
{
    uint8_t lsb = STATE_LFSR & 1;
    STATE_LFSR >>= 1;
    if(lsb == 1)
    {
        STATE_LFSR ^= MASK;
    }
    STEP = (STATE_LFSR & 0b11);
}

// Resets the sequence
void reset_LFSR(void)
{
    STATE_LFSR = SEED;
}

// Sets the LFSR at new seed
void set_LFSR(void)
{
    SEED = STATE_LFSR;
}
