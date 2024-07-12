#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

extern volatile uint8_t uart_input;
extern volatile uint8_t uart_reset;

void uart_init(void);
uint8_t uart_getc(void);
void uart_putc(char c);
void uart_puts(char *s);