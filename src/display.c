#include "display.h"
#include "avr/io.h"
#include "avr/interrupt.h"

#include "timer.h"
#include "button.h" 

volatile uint8_t left_byte = DISP_OFF;
volatile uint8_t right_byte = DISP_OFF;

void spi_init(void)
{
    PORTA.OUTCLR = PIN1_bm;
    PORTA.DIRSET = PIN1_bm;

    PORTC.DIRSET = PIN0_bm | PIN2_bm;

    PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc;
    
    SPI0.CTRLB = SPI_SSD_bm;        // Client select disable bitmask, unbuffered
    SPI0.INTCTRL = SPI_IE_bm;       // Interrupt enable
    SPI0.CTRLA = SPI_MASTER_bm | SPI_ENABLE_bm;    // enable SPI as output
}

// Displays the left and right byte on the 7-segment display
void display(const uint8_t left, const uint8_t right)
{
    left_byte = left | DISP_LHS;
    right_byte = right;
}

// Displays the bars on the 7-segment display
void display_bars(uint8_t step)
{
    static uint8_t patterns[4][2] = {{DISP_BAR_LEFT, DISP_OFF},
                                     {DISP_BAR_RIGHT, DISP_OFF},
                                     {DISP_OFF, DISP_BAR_LEFT},
                                     {DISP_OFF, DISP_BAR_RIGHT}};
    display(patterns[step][0], patterns[step][1]);
}

ISR(SPI0_INT_vect)
{
    // create rising edge on DISP LATCH net
    PORTA.OUTCLR = PIN1_bm;
    PORTA.OUTSET = PIN1_bm;
    SPI0.INTFLAGS = SPI_IF_bm; // clear the interrupt flag
}