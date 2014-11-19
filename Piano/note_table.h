#ifndef NOTETABLE_HEADER_GUARD
#define NOTETABLE_HEADER_GUARD

#include <avr/pgmspace.h> // for PROGRMEM
#include "note.h" // Note codes

#define GET_NOTE(key_id) pgm_read_byte_near(note_table + key_id)

#define PIANO_CONTROLLER_ID 1 // Change this to write the code for the second piano controller

uint8_t note_table[19] PROGMEM = 
#if (PIANO_CONTROLLER_ID == 1) // Check piano controller ID
{                                                              NOTE_Solb4, NOTE_Sol4, NOTE_Lab4, NOTE_La4, NOTE_Sib4, NOTE_Si4,
 NOTE_Do5, NOTE_Reb5, NOTE_Re5, NOTE_Mib5, NOTE_Mi5, NOTE_Fa5, NOTE_Solb5, NOTE_Sol5, NOTE_Lab5, NOTE_La5, NOTE_Sib5, NOTE_Si5,
 NOTE_Do6, };
#elif (PIANO_CONTROLLER_ID == 2)
{                                                                                                                     NOTE_Si2,
 NOTE_Do3, NOTE_Reb3, NOTE_Re3, NOTE_Mib3, NOTE_Mi3, NOTE_Fa3, NOTE_Solb3, NOTE_Sol3, NOTE_Lab3, NOTE_La3, NOTE_Sib3, NOTE_Si3,
 NOTE_Do4, NOTE_Reb4, NOTE_Re4, NOTE_Mib4, NOTE_Mi4, NOTE_Fa4, };
#endif  // No other piano controllers

#endif // NOTETABLE_HEADER_GUARD defined
