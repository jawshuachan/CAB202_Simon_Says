#include <stdint.h>
#include <util/delay.h>

#define OCTAVE_MIN -3
#define OCTAVE_MAX 3

#define ESHARP_PER 9891
#define CSHARP_PER 11779
#define A_PER 7407
#define ELOW_PER 19724

extern volatile int8_t octave;
extern volatile uint8_t selected_tone;

void buzzer_init(void);
void inc_freq(void);
void dec_freq(void);
void play_tone(uint8_t tone);
void stop_tone(void);