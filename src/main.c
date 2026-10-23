#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <stdint.h>
#define LED_DDR        DDRB
#define LED_PORT       PORTB
#define LED_PIN        PB0
#define BUTTON_DDR     DDRD
#define BUTTON_PORT    PORTD
#define BUTTON_INPUT   PIND
#define BUTTON_PIN     PD2
#define DEBOUNCE_TICKS 20U
static volatile uint8_t button_press_event;
ISR(TIMER2_COMP_vect)
{
    static uint8_t debounce_ticks;
    static uint8_t button_is_pressed;
    if ((BUTTON_INPUT & (1U << BUTTON_PIN)) == 0U) {
        if (debounce_ticks < DEBOUNCE_TICKS) {
            ++debounce_ticks;
        }
        if ((debounce_ticks == DEBOUNCE_TICKS) && (button_is_pressed == 0U)) {
            button_is_pressed = 1U;
            button_press_event = 1U;
        }
    } else {
        if (debounce_ticks > 0U) {
            --debounce_ticks;
        } else {
            button_is_pressed = 0U;
        }
    }
}
static void timer2_init(void)
{
    /* 8 MHz / 64 / (124 + 1) = 1 kHz (one interrupt per millisecond). */
    TCCR2 = (1U << WGM21) | (1U << CS22);
    OCR2 = 124U;
    TIMSK |= (1U << OCIE2);
}
int main(void)
{
    uint8_t led_is_on = 0U;
    LED_DDR |= (1U << LED_PIN);
    LED_PORT &= (uint8_t)~(1U << LED_PIN);
    BUTTON_DDR &= (uint8_t)~(1U << BUTTON_PIN);
    BUTTON_PORT |= (1U << BUTTON_PIN); /* Enable the internal pull-up. */
    timer2_init();
    sei();
    for (;;) {
        uint8_t press_event;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            press_event = button_press_event;
            button_press_event = 0U;
        }
        if (press_event != 0U) {
            led_is_on ^= 1U;
            if (led_is_on != 0U) {
                LED_PORT |= (1U << LED_PIN);
            } else {
                LED_PORT &= (uint8_t)~(1U << LED_PIN);
            }
        }
    }
}
