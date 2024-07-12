#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#include "display.h"
#include "timer.h"
#include "button.h"
#include "sequence.h"

// TIMING ------------------------------------------------------------------------------------------------------------

volatile uint16_t elapsed_time = 0;
volatile uint16_t delay = 250;

void timer_init(void)
{
    // TCB0
    // Generates 1ms interrupt to keep track of elapsed time
    TCB0.CCMP = 3333;                   // Set interval for 1 ms (3333 clocks @ 3.333 MHz)
    TCB0.INTCTRL = TCB_CAPT_bm;         // CAPT interrupt enable
    TCB0.CTRLA = TCB_ENABLE_bm;         // enable TCB0

    // interrupt every 5ms
    TCB1.CCMP = 16667;
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.CTRLA = TCB_ENABLE_bm;
}

ISR(TCB0_INT_vect)
{
    elapsed_time++;
    TCB0.INTFLAGS = TCB_CAPT_bm;  // Clear interrupt flag
}

// PUSHBUTTON HANDLING -----------------------------------------------------------------------------------------------

static void debounce(void)
{
    static uint8_t count0 = 0;
    static uint8_t count1 = 0;

    // sample pushbutton state
    uint8_t sample = pb_debounced_state ^ PORTA.IN;

    count1 = (count1 ^ count0) & sample;
    count0 = ~count0 & sample;

    pb_debounced_state ^= (count0 & count1);
}

// DISPLAY MULTIPLEXING -----------------------------------------------------------------------------------------------

void pwm_init(void)
{
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm;    // single slope mode, enable compare channel 0
    TCA0.SINGLE.PER = 1;                                                            // random cycle for initialisation
    TCA0.SINGLE.CMP0 = 0;                                                           // set duty cycle to 0% intially
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;                                      // enable TCA0
}


static void spi_write(void)
{
    static uint8_t side = 0;
    if(side)
    {
        SPI0.DATA = left_byte;
        side = 0;
    }
    else
    {
        SPI0.DATA = right_byte;
        side = 1;
    }
}

ISR(TCB1_INT_vect)
{
    debounce();
    spi_write();
    TCB1.INTFLAGS = TCB_CAPT_bm; // clear the interrupt flag
}

// DELAY SEQUENCE -----------------------------------------------------------------------------------------------------

volatile uint16_t delay;
volatile uint16_t delay_new;
volatile uint8_t delay_stage;

void adc_init(void)
{
    ADC0.CTRLA = ADC_ENABLE_bm;                                         // enable ADC
    ADC0.CTRLB = ADC_PRESC_DIV2_gc;                                     // configure /2 clock prescaler
    ADC0.CTRLC = (4 << ADC_TIMEBASE_gp)| (ADC_REFSEL_VDD_gc);           // set ADC clock to 1/2 system clock, reference voltage to VDD
    ADC0.CTRLE = 64;                                                    // set ADC resolution to 8-bit
    // ADC0.CTRLF = ADC_FREERUN_bm;                                        // enable free running mode
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;                                   // select ADC channel AIN2
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc;    // configure 8-bit single conversion mode
}

void update_playback_delay(void)
{
    // start
    ADC0.COMMAND = ADC_START_IMMEDIATE_gc;

    // wait for conversion result
    while(!(ADC0.INTFLAGS & ADC_RESRDY_bm))
        ;
    delay = (ADC0.RESULT * 1750/255) + 250; // use ADC0.RESULT to get a time between 250ms and 2000ms

    // clear interrupt flag
    ADC0.INTFLAGS = ADC_RESRDY_bm;
}