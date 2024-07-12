#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

#include "uart.h"
#include "states.h"
#include "timer.h"
#include "buzzer.h"
#include "sequence.h"

volatile uint8_t uart_input = 0;
volatile uint8_t uart_reset = 0;

void uart_init(void) 
{
    USART0.BAUD = 1389;                             // 9600 baud @ 3.333 MHz
    USART0.CTRLA = USART_RXCIE_bm;                  // Enable RX complete interrupt
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;   // Enable TX/RX
}

// get input from uart
uint8_t uart_getc(void) 
{
    while(!(USART0.STATUS & USART_RXCIF_bm));
    ; // wait for data to be received
    return USART0.RXDATAL;
}

// write out the data in uart
void uart_putc(char c) 
{
    while(!(USART0.STATUS & USART_DREIF_bm))
    ; // waits for TXDATA to be empty
    USART0.TXDATAL = c;
}

// write out the string in uart
void uart_puts(char *s) 
{
    while(*s != '\0')
    {
        uart_putc(*s);
        s++;
    }
}

// Function to convert a hex character to an integer
static uint8_t hexchar_to_int(char c)
{
    if('0' <= c && c <= '9')
        return c - '0';
    else if('a' <= c && c <= 'f')
        return 10 + c - 'a';
    else
        return 16;
}

ISR(USART0_RXC_vect) // interrupt service routine for uart
{
    static SERIAL_STATES serial_state = AWAITING_COMMAND;
    static uint8_t chars_received = 0;
    static uint16_t payload = 0;
    static uint8_t payload_valid = 0;
    char rx_data = USART0.RXDATAL;

    switch(serial_state)
    {
        case AWAITING_COMMAND:
            if(rx_data == '1' || rx_data == 'q')
                uart_input = 1;
            else if(rx_data == '2' || rx_data == 'w')
                uart_input = 2;
            else if(rx_data == '3' || rx_data == 'e')
                uart_input = 3;
            else if(rx_data == '4' || rx_data == 'r')
                uart_input = 4;
            else if(rx_data == ',' || rx_data == 'k')
                inc_freq();
            else if(rx_data == '.' || rx_data == 'l')
                dec_freq();
            else if(rx_data == '0' || rx_data == 'p')
                uart_reset = 1;
            else if(rx_data == '9' || rx_data == 'o')
            {
                // reset flags
                payload_valid = 1;
                chars_received = 0;
                payload = 0;
                
                // serial state transition
                serial_state = AWAITING_PAYLOAD;
            }
            break;
        case AWAITING_PAYLOAD:
        {
            // parse payload
            uint8_t parsed_result = hexchar_to_int(rx_data);

            if(parsed_result != 16)
            {
                payload = (payload << 4) | parsed_result;
            }
            else    
                payload_valid = 0;
            // check if payload is complete
            if(++chars_received == 8)
            {
                if(payload_valid)
                {
                    SEED = payload;
                }
                serial_state = AWAITING_COMMAND;
            }
            break;
        }
        default: 
            serial_state = AWAITING_COMMAND;
            break;
    }
}