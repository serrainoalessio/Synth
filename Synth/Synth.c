#ifndef __AVR_ATmega328P__ 
    #define __AVR_ATmega328P__  // microcontroller
#endif
#define F_CPU 16000000      // frequency of the cpu

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

//#include "note_table.h" isn't necessary
#include "repeat.h"
#include "wavetable.h"
#include "note_freq.h"

#define INVERSE_LOGIC 0 // No inverse logic, idle status is HIGH, first bit is LOW, etc..

#define NOP "nop\n\t" // asm no-operation, just waste one clock cycle
#define TX_NOPS _64(NOP) // <<<<------------------------------- INCREASE/REDUCE TX SPEED <<<< --------------------
#define CX_NOPS _12(NOP) // <<<<-------------------------------- CENTERS THE READS <<<< --------------------

volatile uint8_t mex, have_mex;
volatile uint8_t next, update;
volatile uint16_t sustain; // from 0 to 125

/*
uint8_t random();
uint8_t reg(float s);
*/

ISR(TIMER1_COMPA_vect){
    OCR0A = next; // updates sound
    update = 1; // have to work out next sound
    //PORTD ^= (_BV(PIND4)); // debug only
}

// External interrupts
ISR(INT0_vect) { // pin 2 falling edge
    static uint8_t message;
    uint8_t bitmask, nbitmask;
    
    cli(); // first clear interrupts
    asm volatile(CX_NOPS); // center reads

    bitmask = 1;
    
    while (bitmask) { // read mex
        // wait for one cycle
        asm volatile(TX_NOPS);
        nbitmask = ~bitmask;
        if (PIND & _BV(PIND2)) // pin is high, write 1
            message |= bitmask;
        else // to pair if is executed the code above
            message &= nbitmask;
        bitmask <<= 1;
    }

    // message ready
    mex = message;
    have_mex = 1;

    EIFR &= _BV(INTF1) | _BV(INTF0); // reset interrupts
    sei(); // re-enable interrupts
}

ISR(INT1_vect) { // pin 3
    static uint8_t message;
    uint8_t bitmask, nbitmask;
    
    cli(); // first clear interrupts
    asm volatile(CX_NOPS); // center reads

    bitmask = 1;
    
    while (bitmask) { // read mex
        // wait for one cycle
        asm volatile(TX_NOPS);
        nbitmask = ~bitmask;
        if (PIND & _BV(PIND3)) // pin is high, write 1
            message |= bitmask;
        else // to pair if is executed the code above
            message &= nbitmask;
        bitmask <<= 1;
    }

    // message ready
    mex = message;
    have_mex = 1;

    EIFR &= _BV(INTF1) | _BV(INTF0); // reset interrupts
    sei(); // re-enable interrupts
}

int main() {
    uint8_t tone1, tone2;
    uint16_t phase1, phase2;
    uint16_t freq1, freq2;
    uint16_t volume1, volume2; // this may require a bit more time
    uint16_t button_idle;
    uint8_t sound_select;
    
    // first: set ports
#if (INVERSE_LOGIC == 0)
    EICRA = _BV(ISC11) | _BV(ISC01); // every edge fall
#else
    EICRA = _BV(ISC11) | _BV(ISC10) |
            _BV(ISC01) | _BV(ISC00); // every edge rise
#endif
    EIMSK = _BV(INT1) | _BV(INT0); // enable interrupts for pins

    //DDRD = _BV(PIND4) | _BV(PIND5) | _BV(PIND6) | _BV(PIND7); // ----------------------------------- DELETE MEEEEE !!!
    //DDRB = _BV(PINB0) | _BV(PINB1) | _BV(PINB2) | _BV(PINB3); // ----------------------------------- DELETE MEEEEE !!!
    //PORTD &= ~(_BV(PIND5));
    PORTD = 0;
    PORTB = 0;
    PORTC = 0;

    //DDRB = _BV(PINB1) | _BV(PINB2); // sets pins 9 and 10 as output
    DDRD = _BV(PIND6);

    // timers
    GTCCR = _BV(TSM) | _BV(PSRASY) | _BV(PSRSYNC); // stops all timers
    /*
    TCCR1A = _BV(WGM10) | _BV(COM1A1); // 8-bit pwm, 62.5kHz
    TCCR1B = _BV(WGM12) | _BV(CS10); // no prescaling
    TIMSK1 = 0; // No interrupts
    OCR1AH = 0; // not necessary
    OCR1AL = 0; // compare value
    TCNT1H = 0;
    TCNT1L = 0;
    */
    
    TCCR0A = _BV(WGM01) | _BV(WGM00) | _BV(COM0A1); // Fast pwm mode 62.5kHz, output on port OC0A (pin6)
    TCCR0B = _BV(CS00); // no prescaling
    TIMSK0 = 0; //No interupts
    OCR0A = 0; // compare value A
    OCR0B = 0; // compare value B
    TCNT0 = 0; // starts timer from 0
    
    TCCR1B = _BV(WGM12) | _BV(CS10); // no prescaling, CTC mode
	TCCR1A = 0; // no output ports
	OCR1A = 1024; // compare value (0-65535)
	TIMSK1 = _BV(OCIE1A); // interrupt when timer reaches OCR1A
	//interrupts 19531.25 times at second
    
    // second: init variables
    mex = 0;
    next = 0;
    update = 1;
    tone1 = tone2 = 0;
    phase1 = phase2 = 0;
    freq1 = freq2 = 0;
    volume1 = volume2 = 0;

    sustain = 5000;
    sound_select = 1;
    button_idle = 0;
    
    sei(); // enable interrupts
    GTCCR = 0; // Restarts timers
    
    while (1) {
        if (update) { // compute next sound
            phase1 += freq1;
            phase1 %= TABLE_LEN;
            phase2 += freq2;
            phase2 %= TABLE_LEN;
            if (sound_select == 1)
                next = ((uint32_t)(WAVE1(phase1)/2)*volume1 + (uint32_t)(WAVE1(phase2)/2)*volume2)/sustain;
            else if (sound_select == 2)
                next = ((uint32_t)(WAVE2(phase1)/2)*volume1 + (uint32_t)(WAVE2(phase2)/2)*volume2)/sustain;
            if (volume1)
                if (tone1 == 0)
                    volume1--;
            if (volume2)
                if (tone2 == 0)
                    volume2--;
            
            if (volume1 == 0)
                tone1 = freq1 = phase1 = 0;
            if (volume2 == 0)
                tone2 = freq2 = phase2 = 0;
            update = 0; // updated
        }
        if (have_mex) {
            if (mex & 0x80) { // key on
                if (tone1) { // freq1 is already used
                    if (!tone2) {
                        tone2 = mex-0x80;
                        freq2 = FREQ(tone2); // use freq2
                        volume2 = sustain;
                    } else { // use freq1
                        tone2 = tone1;
                        freq2 = freq1;
                        volume2 = volume1; // copy data
                        tone1 = mex-0x80;
                        freq1 = FREQ(tone1);
                        volume1 = sustain;
                    }
               } else { // freq1 is not used
                    tone1 = mex-0x80;
                    freq1 = FREQ(tone1);
                    volume1 = sustain;
                }
            } else {
                if (mex == tone1) {
                    tone1 = 0; // key 1 off
                }
                if (mex == tone2) {
                    tone2 = 0; // key 2 off
                }
            }
            have_mex = 0;
        }

        if ((PINC & _BV(PINC5)) && (button_idle == 0)) {
            button_idle = 65536;
            sound_select++;
            if (sound_select > 2)
                sound_select = 1;
        }
        if (button_idle)
            button_idle--;
    }
    
    return 0;
}

/*

uint8_t random() { // pseudorandom number generation
    static uint8_t rand_seed = 0; // static means that the value of this vairable is remembered after the function ends
    static uint8_t multiplier = 53;
    static uint8_t adder = 3;
    rand_seed*=multiplier;
    rand_seed+=adder;
    if (rand_seed == 0) {
        adder+=2; // Each combination of odd adder
        if (adder == 1)
            multiplier+=4; // and multiplier mod 4 == 1 gives each one of the 256 possible values
    }
    return rand_seed;
}

uint8_t reg(float s) {
    uint8_t X = (uint8_t)s;
    uint8_t threeshold = (s - X)*256; 
    if (random() < threeshold)
        X++;
    return X;
}

*/

/*
void debug() {
    // code written for debug pruposes
        if (mex & 0x01) PORTD |= (_BV(PIND4));
        else PORTD &= ~(_BV(PIND4));
        
        if (mex & 0x02) PORTD |= (_BV(PIND5));
        else PORTD &= ~(_BV(PIND5));
        
        if (mex & 0x04) PORTD |= (_BV(PIND6));
        else PORTD &= ~(_BV(PIND6));
        
        if (mex & 0x08) PORTD |= (_BV(PIND7));
        else PORTD &= ~(_BV(PIND7));
        
        if (mex & 0x10) PORTB |= (_BV(PINB0));
        else PORTB &= ~(_BV(PINB0));
        
        if (mex & 0x20) PORTB |= (_BV(PINB1));
        else PORTB &= ~(_BV(PINB1));
        
        if (mex & 0x40) PORTB |= (_BV(PINB2));
        else PORTB &= ~(_BV(PINB2));
        
        if (mex & 0x80) PORTB |= (_BV(PINB3));
        else PORTB &= ~(_BV(PINB3));
} */
