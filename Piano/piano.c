#ifndef __AVR_ATmega328P__ 
    #define __AVR_ATmega328P__  // microcontroller
#endif
#define F_CPU 16000000      // frequency of the cpu

#include <avr/io.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "note_table.h"
#include "repeat.h"

#define PORTD_BITMASK _BV(PIND1) | _BV(PIND2) | _BV(PIND3) | _BV(PIND4) | _BV(PIND5) | _BV(PIND6) | _BV(PIND7)
#define PORTB_BITMASK _BV(PINB0) | _BV(PINB1) | _BV(PINB2) | _BV(PINB3) | _BV(PINB4) | _BV(PINB5)
#define PORTC_BITMASK _BV(PINC0) | _BV(PINC1) | _BV(PINC2) | _BV(PINC3) | _BV(PINC4) | _BV(PINC5)

#define SUST 250 // in loop units

#define INVERSE_LOGIC 0 // No inverse logic, idle status is HIGH, first bit is LOW, etc..
#if (INVERSE_LOGIC == 0) // normal mode
  #define PORTD0_SBI PORTD |= _BV(PORTD0)     // writes 1 on the output
  #define PORTD0_CBI PORTD &= ~(_BV(PORTD0))  // writes 0 on the output
#else // inverted mode
  #define PORTD0_SBI PORTD &= ~(_BV(PORTD0))  // writes 0 on the output
  #define PORTD0_CBI PORTD |= _BV(PORTD0)     // writes 1 on the output
#endif

#define NOP "nop\n\t" // asm no-operation, just waste one clock cycle
#define IN_NOPS _16(NOP) // <<<<------------------------------- INCREASE/DECREASE SENSITIVITY <<<< --------------------
#define THRESHOLD 31     // <<<<------------------------------- INCREASE/REDUCE THRESHOLD <<<< --------------------
#define TX_NOPS _64(NOP) // <<<<------------------------------- INCREASE/DECREASE TX SPEED <<<< --------------------

inline void send_message(uint8_t l_message);
uint8_t check_port(uint8_t in);
inline void discharge_ports();

int main() {
    uint8_t i, id;
    uint8_t timing[19]; // This controller control 18 keys
    uint8_t cnt[19]; // This controller control 18 keys
    uint32_t status[19]; // This controller control 18 keys
    
    // program setup
    discharge_ports();
    SREG = 0; // No interrupts
    
    DDRD |= _BV(PIND0); // NOTE: Only output pin is number 0
    PORTD0_SBI;

    for (i = 0; i < 19; i++) {
        timing[i] = 0;
        cnt[i]    = 0;
        status[i] = 0;
    }
    
    // main loop
    while(1) {
        // first step: check wether a key was pressed or not
        for (i = 0; i < 19; i++) {
            id = i + 1;
            if (check_port(id)) {
                if (!(status[i] & 0x80000000L)) // wasting a 0
                    cnt[i]++; // added one 1
                // else // wasting 1, cnt does not change
                status[i] <<= 1; // shift
                status[i] |= 1; // last added is 1
                
                if (cnt[i] > THRESHOLD) {
                    if (timing[i]) // key was already pressed
                        timing[i] = SUST;
                    else
                        timing[i] = SUST + 1;
                }
            } else {
                if (status[i] & 0x80000000L) // wasting a 1
                    cnt[i]--; // removed one 1
                // else // wasting 1, cnt does not change
                status[i] <<= 1; // shift, last byte is already 1
                
                if (timing[i])
                    timing[i]--;
            }
        }
        
        discharge_ports(); // after reading discharges ports for the next reading
        
        // second step: if some messages are true then send data to the synth
        for (i = 0; i < 19; i++)
            if (timing[i] == SUST + 1) {
                send_message(0x80 | GET_NOTE(i));
            } else if (timing[i] == 1) {
                send_message(0x00 | GET_NOTE(i));
            }

        //_delay_us(100); // wait some time
    }
    
    return 0;
}

inline void send_message(uint8_t message) {
    uint8_t mask = 1;

    // step 1: writes first bit, which is low
    PORTD0_CBI;
    asm volatile(TX_NOPS); // wait some time

    // Step 2: writes the Payload, first l_message, then h_message
    while (mask) {
        if (message & mask) { // message bit is 1
            PORTD0_SBI;
        } else { // message bit is 0
            PORTD0_CBI;
        }
        asm volatile(TX_NOPS); // wait some time
        mask <<= 1;
    }

    // step 3: reset port status
    PORTD0_SBI;
}

inline void discharge_ports()
{
	uint8_t bitmask_d, bitmask_b, bitmask_c;
	bitmask_d = PORTD_BITMASK;
	bitmask_b = PORTB_BITMASK;
	bitmask_c = PORTC_BITMASK;
	
	// Write 0 on the keyboard pins
	PORTD &= ~(bitmask_d);
	PORTB &= ~(bitmask_b);
	PORTC &= ~(bitmask_c);
	
	// and make output the input (to remove internal resistance)
	DDRD |= bitmask_d;
	DDRB |= bitmask_b;
	DDRC |= bitmask_c;
	
	//delayMicroseconds(10); // wait some time
}

uint8_t check_port(unsigned char in)
{
    volatile uint8_t * port, * ddr, * pin;
    uint8_t bitmask;
    uint8_t val; // return value
    
	// switch correct ports
    if (in >= 0 && in <= 7) {
        port = &PORTD;
        ddr  = &DDRD;
        pin  = &PIND;
        bitmask = _BV(in);
    } else if (in >= 8 && in <= 13) {
        port = &PORTB;
        ddr  = &DDRB;
        pin  = &PINB;
        bitmask = _BV(in-8);
    } else if (in >= 14 && in <= 19) {
        port = &PORTC;
        ddr  = &DDRC;
        pin  = &PINC;
        bitmask = _BV(in-14);
    } else
        return -1;

    // ports should be already discharged
    // interupts should be disabled
    
    *ddr &= ~(bitmask); // Make the port an input (connect internal resistor)
    *port |= bitmask; // writes 1 (connect to pull-up)
    
    //wait some time. each nop is 62.5nS on 16Mhz
    asm volatile(IN_NOPS);  // macro WAIT_NOPS writes a series of "nop", no-operation
    
    if (*pin & bitmask) // if pin is HIGH there is no contact
        val = 0;
    else // if pin is LOW there is a key pressure
        val = 1;
    
    // discharge port, it is important to leave the pis low if you want to do multiple readings
    *port &= ~(bitmask); // wirtes 0
    *ddr |= bitmask; // port is now an output (disconnect internal resistor)
    // re-enable interrupts
    
    return val;
}
