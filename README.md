Synth
=====

A simple Arduino piano and synthetizer

Compile with: avr-gcc -O2 -o [output].out ./piano.c
avr-objcopy -O ihex -R .eeprom [output].out [output].hex

push in your Arduino with avrdude.
Already compiled for Arduino Uno (Atmega328P)

Future versions will include a makefile
