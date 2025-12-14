#include <avr/io.h>
#include <avr/interrupt.h>

void simulate_100ms(void);
void simulate_300ms(void);
void the_long(void);
void the_short(void);
volatile int counter = 0;
volatile uint8_t long_request = 0;
volatile uint8_t short_request = 0;

int main(void) {

    // Button INT0 (PD2)
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);   // pull-up
    // Button INT1 (PD3)
    DDRD &= ~(1 << PD3);
    PORTD |= (1 << PD3);   // pull-up
    // LED PD7
    DDRD |= (1 << PD7);

    // INT0 falling edge
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    // INT1 falling edge
    EICRA |= (1 << ISC11);
    EICRA &= ~(1 << ISC10);


    EIMSK |= (1 << INT0);
    EIMSK |= (1 << INT1);

    // Timer1 CTC, prescaler 64
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // turn on the CTC mode(WGM12)
    OCR1A = 24999; // 100 ms at 16 MHz

    sei();

    while (1) {
        if (long_request) {
            long_request = 0;
            the_long();
        }

        if (short_request) {
            short_request = 0;
            the_short();
        }
    }
}

ISR(INT0_vect)
{
    long_request = 1;
}

ISR(INT1_vect)
{
    short_request = 1;
}

// 100 ms delay using Timer1 (polling)
void simulate_100ms(void)
{
    TCNT1 = 0;
    while (TCNT1 < OCR1A);
}

// 300 ms = 3 × 100 ms
void simulate_300ms(void)
{
    counter = 0;
    while (counter < 3) {
        simulate_100ms();
        counter++;
    }
}

void the_long(void){
    PORTD |= (1 << PD7);
    simulate_300ms();
    PORTD &= ~(1 << PD7);
};
void the_short(void){
    PORTD |= (1 << PD7);
    simulate_100ms();
    PORTD &= ~(1 << PD7);
};
