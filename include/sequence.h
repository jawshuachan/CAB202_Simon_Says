#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define STUDENT_ID 0x11870150

extern volatile uint8_t sequence_index;
extern volatile uint8_t user_score;
extern volatile uint16_t sequence_length;

extern volatile uint32_t SEED;
extern volatile uint32_t STATE_LFSR;
extern volatile uint32_t MASK;
extern volatile uint8_t STEP;

void next_step(void);
void reset_LFSR(void);
void set_LFSR(void);