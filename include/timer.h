#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

extern volatile uint16_t elapsed_time;
extern volatile uint16_t delay;

extern volatile uint8_t delay_stage;  
extern volatile uint16_t delay_new;
extern volatile uint8_t user_score;

void timer_init(void);
void pwm_init(void);
void adc_init(void);
void update_playback_delay(void);