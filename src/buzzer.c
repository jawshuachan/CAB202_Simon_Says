#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

#include "buzzer.h"
#include "uart.h"
#include "timer.h"
#include "display.h"

volatile uint8_t selected_tone = 0;
volatile uint8_t tone_flag = 0;
volatile int8_t octave = 0;

void buzzer_init(void)
{
    PORTB.DIRSET = PIN0_bm;
}

void inc_freq(void)
{
    if(octave < OCTAVE_MAX)
    {
        octave++;
        if(tone_flag)
        {
            play_tone(selected_tone);
        }
    }
}

void dec_freq(void)
{
    if(octave > OCTAVE_MIN)
    {
        octave--;
        if(tone_flag)
        {
            play_tone(selected_tone);
        }
    }
}

void update_tone(uint8_t tone)
{
    if (tone_flag)
        play_tone(tone);
    else
        selected_tone = tone;
}

void play_tone(uint8_t tone)
{
    static const uint32_t periods [4] = {ESHARP_PER, CSHARP_PER, A_PER, ELOW_PER};

    // PER = 2^3 * (fclk / f ) >> (octave + 3)
    uint16_t period = periods[tone] >> (octave);
    TCA0.SINGLE.PERBUF = period;
    TCA0.SINGLE.CMP0BUF = period >> 1;
    selected_tone = tone;
    tone_flag = 1;
}

void stop_tone(void)
{
    TCA0.SINGLE.CMP0BUF = 0;
    tone_flag = 0;
}