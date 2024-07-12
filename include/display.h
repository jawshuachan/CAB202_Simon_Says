#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint8_t left_byte;
extern volatile uint8_t right_byte;

void spi_init();
void display(const uint8_t left, const uint8_t right);
void display_bars(uint8_t step);


#define DISP_SEG_B 0b01101111
#define DISP_SEG_C 0b01111011
#define DISP_SEG_E 0b01111110
#define DISP_SEG_F 0b00111111
#define DISP_SEG_G 0b01110111

#define DISP_BAR_LEFT (DISP_SEG_E & DISP_SEG_F)
#define DISP_BAR_RIGHT (DISP_SEG_B & DISP_SEG_C)

#define DISP_SEG_0 0b00001000
#define DISP_SEG_1 0b01101011
#define DISP_SEG_2 0b01000100
#define DISP_SEG_3 0b01000001
#define DISP_SEG_4 0b00100011
#define DISP_SEG_5 0b00010001
#define DISP_SEG_6 0b00110000
#define DISP_SEG_7 0b01001011
#define DISP_SEG_8 0b00000000
#define DISP_SEG_9 0b00000001

#define DISP_ALL 0b00000000
#define DISP_OFF 0b01111111

#define DISP_LHS (1 << 7) 

