#include <avr/interrupt.h>
#include <stdio.h>

#include "button.h"
#include "buzzer.h"
#include "display.h"
#include "timer.h"
#include "uart.h"
#include "states.h"
#include "sequence.h"

// VARIABLES ---------------------------------------------------------

volatile STATES current_state = START;
volatile uint8_t tone_played;

uint8_t segments[] = {
    DISP_SEG_0,
    DISP_SEG_1,
    DISP_SEG_2,
    DISP_SEG_3,
    DISP_SEG_4,
    DISP_SEG_5,
    DISP_SEG_6,
    DISP_SEG_7,
    DISP_SEG_8,
    DISP_SEG_9
};

volatile uint8_t left_digit;
volatile uint8_t right_digit;


// INITIALISATION -----------------------------------------------------
void init(void)
{
    cli();          // disable global interrupts
    uart_init();
    spi_init();
    timer_init();
    pwm_init();
    adc_init();
    button_init();
    buzzer_init();
    sei();          // enable global interrupts
}

// Plays the tone associated with the current step
void outputs(uint8_t step)
{
    update_playback_delay();
    play_tone(step);
    display_bars(step);
    elapsed_time = 0;
}

// Turns off the buzzer and display
void outputs_off(void)
{
    stop_tone();
    display(DISP_OFF, DISP_OFF);
}

// USER SCORE ---------------------------------------------------------

void find_digits(uint8_t num)
{
    uint8_t d1 = 0, d2;
    d2 = num;
    while(d2 > 9)
    {
        d2 -= 10;
        d1++;
    }
    left_digit = d1;
    right_digit = d2;
}

void output_score(uint16_t score)
{
    if(user_score > 99)
    {
        user_score %= 100;
    }
    find_digits(user_score);
    if(left_digit == 0)
    {
        display(DISP_OFF, segments[right_digit]);
    }
    else
    {
        display(segments[left_digit], segments[right_digit]);
    }
}

// GAMESTATE MACHINE -------------------------------------------------

void gamestate_machine(void)
{
    while(1)
    { 
        // Handle pushbuttons
        pb_state_r = pb_state_c;                // store current state in previous state
        pb_state_c = pb_debounced_state;        // update current state

        pb_falling = pb_state_r & ~pb_state_c;  // detect falling edge
        pb_rising = ~pb_state_r & pb_state_c;   // detect rising edge

        if(uart_reset)
        {
            // check if new seed
            // LFSR = SEED
            reset_LFSR();
            sequence_index = 0;
            sequence_length = 1;

            outputs_off();
            octave = 0;
            current_state = START;
        }

        switch(current_state)
        {
            case START:
                if(sequence_index < sequence_length)
                {
                    next_step();                // update STEP
                    outputs(STEP);              // use STEP

                    sequence_index++;           // increase sequence index
                    delay_stage = 0;            // reset delay stage
                    current_state = SIMON;      // transition to SIMON stage
                }
                else
                {
                    reset_LFSR();                       // reset LFSR
                     
                    sequence_index = 0;                 // reset sequence index
                    uart_input = 0;                     // reset UART input
                    current_state = AWAITING_INPUT;     // transition to AWAITING_INPUT
                }
                break;

            case SIMON:
                if(delay_stage == 0 && elapsed_time > delay >> 1)
                {
                    outputs_off();             // turn off display
                    elapsed_time = 0;          // reset elapsed time
                    delay_stage = 1;
                }
                else if(delay_stage == 1 && elapsed_time > delay >> 1)
                {
                    current_state = START;
                }
                break;
            case AWAITING_INPUT:
                // PUSHBUTTON INPUT
                if(pb_falling & (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm))
                {
                    if(pb_falling & PIN4_bm)
                        pb_step_cmp = 1;
                    else if(pb_falling & PIN5_bm)
                        pb_step_cmp = 2;
                    else if(pb_falling & PIN6_bm)
                        pb_step_cmp = 3;
                    else if(pb_falling & PIN7_bm)
                        pb_step_cmp = 4;

                    outputs(pb_step_cmp - 1);
                    
                    // update flags
                    pb_released = 0;
                    current_state = HANDLING_INPUT;
                }
                // UART INPUT
                else if(uart_input != 0)
                {
                    pb_step_cmp = uart_input;
                    uart_input = 0;

                    if(pb_step_cmp == 1)
                        pb_rising = PIN4_bm;
                    else if(pb_step_cmp == 2)
                        pb_rising = PIN5_bm;
                    else if(pb_step_cmp == 3)
                        pb_rising = PIN6_bm;
                    else if(pb_step_cmp == 4)
                        pb_rising = PIN7_bm;

                    // play tone    
                    outputs(pb_step_cmp - 1);
                    
                    // update flags
                    pb_released = 0;
                    tone_played = 1;
                    current_state = HANDLING_INPUT;
                }
                break;
            case HANDLING_INPUT:
                if(!pb_released) // when pushbutton is released 
                {
                    if(pb_rising & PIN4_bm && pb_step_cmp == 1)
                    {
                        pb_released = 1;
                    }
                    else if(pb_rising & PIN5_bm && pb_step_cmp == 2)
                    {
                        pb_released = 1;
                    }
                    else if(pb_rising & PIN6_bm && pb_step_cmp == 3)
                    {
                        pb_released = 1;
                    }
                    else if(pb_rising & PIN7_bm && pb_step_cmp == 4)
                    {
                        pb_released = 1;
                    }

                    if(tone_played == 1)
                    {
                        pb_released = 1;
                    }
                }
                else
                {
                    // wait for at least 50% of the delay
                    if(elapsed_time >= delay >> 1)
                    {
                        outputs_off();
                        // check input
                        next_step();
                        sequence_index++;
                        if(STEP == pb_step_cmp - 1)
                        {
                            // SUCCESS STATE
                            if(sequence_index < sequence_length)
                            {
                                current_state = AWAITING_INPUT;
                            }
                            else
                            {
                                display(DISP_ALL, DISP_ALL); // display success pattern
                                elapsed_time = 0;

                                // reset sequence
                                reset_LFSR();
                                sequence_index = 0;
                                sequence_length++;

                                // transition to new game
                                current_state = SUCCESS;
                            } 
                        }
                        else
                        {
                            // FAIL STATE
                            display(DISP_SEG_G, DISP_SEG_G); // display fail pattern for first 50% of the delay
                            elapsed_time = 0;
                            current_state = FAIL;
                        }
                    }
                }
                break;
            case SUCCESS: // success pattern currently displayed
                if(elapsed_time > delay) // turn off display 
                {
                    outputs_off();
                    current_state = START;
                }
                break;
            case FAIL: // fail pattern currently displayed
                if(elapsed_time > delay) // change display to scores
                {
                    user_score = sequence_length;
                    output_score(user_score);
                    elapsed_time = 0;   
                    current_state = SCORE;
                }
                break;
            case SCORE: // score currently displayed
                if(elapsed_time >= delay >> 1) // turn off display
                {
                    outputs_off();
                    reset_LFSR();
                    sequence_index = 0;
                    for(int i = 0; i< sequence_length; i++) // loop through previous sequences to create new sequence
                    {
                        next_step();
                    }                
                    set_LFSR();  
                    sequence_length = 1;
                    sequence_index = 0;  
                    elapsed_time = 0;
                    current_state = START;
                }
                break;
            default:
                current_state = START;
                break;
        }
    }
}

int main(void)
{
    init();
    gamestate_machine();
}