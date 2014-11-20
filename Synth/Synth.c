#ifndef __AVR_ATmega328P__ 
    #define __AVR_ATmega328P__  // microcontroller
#endif
#define F_CPU 16000000      // frequency of the cpu

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "note_table.h"
#include "repeat.h"

#define INVERSE_LOGIC 0 // No inverse logic, idle status is HIGH, first bit is LOW, etc..

#define NOP "nop\n\t" // asm no-operation, just waste one clock cycle
#define TX_NOPS _128(NOP) // <<<<------------------------------- INCREASE/REDUCE TX SPEED <<<< --------------------
#define CX_NOPS _28(NOP) // <<<<-------------------------------- CENTERS THE READS <<<< --------------------

volatile uint8_t mex;

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

    EIFR &= _BV(INTF1) | _BV(INTF0); // reset interrupts
    sei(); // re-enable interrupts
}

ISR(INT1_vect) { // pin 3
    //PORTD |= _BV(PIND4);

}

int main() {
    // first: set ports
#if (INVERSE_LOGIC == 0)
    EICRA = _BV(ISC11) | _BV(ISC01); // every edge fall
#else
    EICRA = _BV(ISC11) | _BV(ISC10) |
            _BV(ISC01) | _BV(ISC00); // every edge rise
#endif
    EIMSK = _BV(INT1) | _BV(INT0); // enable interrupts for pins

    DDRD = _BV(PIND4) | _BV(PIND5) | _BV(PIND6) | _BV(PIND7); // ----------------------------------- DELETE MEEEEE !!!
    DDRB = _BV(PINB0) | _BV(PINB1) | _BV(PINB2) | _BV(PINB3); // ----------------------------------- DELETE MEEEEE !!!
    //PORTD &= ~(_BV(PIND5));
    PORTD = 0;
    PORTB = 0;

    //DDRB = _BV(PINB1) | _BV(PINB2); // sets pins 9 and 10 as output
    
    // second: init variables
    mex = 0;
    
    sei(); // enable interrupts
    
    while (1) { // expected 55
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
        
    }
    
    return 0;
}
