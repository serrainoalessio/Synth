#ifndef NOTE_FREQ_HEADER_GUARD
#define NOTE_FREQ_HEADER_GUARD

#include <avr/pgmspace.h> // for PROGRMEM

#define FREQ(key_id) pgm_read_word_near(note_freq + key_id)

uint16_t note_freq[128] PROGMEM =
{ 9, 9, 10, 10, 11, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 20, 22, 23, 24, 26, 27, 29, 31, 32,
  34, 36, 38, 41, 43, 46, 48, 51, 54, 58, 61, 65, 69, 73, 77, 82, 86, 92, 97, 103, 109, 115, 122,
  129, 137, 145, 154, 163, 173, 183, 194, 206, 218, 231, 244, 259, 274, 291, 308, 326, 346, 366,
  388, 411, 435, 461, 489, 518, 549, 581, 616, 652, 691, 732, 776, 822, 871, 923, 978, 1036, 1097,
  1163, 1232, 1305, 1383, 1465, 1552, 1644, 1742, 1845, 1955, 2071, 2195, 2325, 2463, 2610, 2765,
  2930, 3104, 3288, 3484, 3691, 3910, 4143, 4389, 4650, 4927, 5220, 5530, 5859, 6207, 6577, 6968,
  7382, 7821, 8286, 8779, 9301, 9854, 10440, 11060, 11718, 12415, };
/* Frequency is not expressed in hertz. There are 4096 samples of sine, we want, for example, to play a La4
 * This sine repeats 440 times in a second.
 * If we read each sample every 1024 clock cycle, we produce an entire wave in 1024*4096 clock cycles, wich
 * is about 0.263 seconds, freq ca. 3.814Hz
 * Let be C the clocks per second, T the clock cycles between two sampling, S the max sampling, and F the base
 * frequency of the note and X is the note_freq value
 * F = C*X/(T*S)   ===>  X = F*T*S/C
 */

#endif // NOTE_FREQ_HEADER_GUARD defined
