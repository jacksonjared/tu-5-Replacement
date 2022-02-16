/**
 * @file tu5Replacement.c
 * @author jared jackson
 * @brief Interfaces with a ys-811a and injects the requested CTCSS tone
 * @version 0.1
 * @date 2021-09-14
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public License,
 * v. 2.0. If a copy of the MPL was not distributed with this file, You can
 * obtain one at http://mozilla.org/MPL/2.0/.
 * 
 */

/// The CPU clock frequency
#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include <stdint.h>

/// A sine wave in 8bit integers
const uint8_t sine[256] = {
	1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 19,
	21, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 43, 45, 47, 50, 52, 55, 57, 60,
	63, 65, 68, 71, 74, 77, 79, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 112,
	116, 119, 122, 125, 128, 131, 134, 137, 140, 144, 147, 150, 153, 156, 159,
	162, 165, 168, 171, 174, 177, 179, 182, 185, 188, 191, 193, 196, 199, 201,
	204, 206, 209, 211, 213, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234,
	235, 237, 239, 240, 241, 243, 244, 245, 246, 248, 249, 250, 250, 251, 252,
	253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254,
	253, 253, 252, 251, 250, 250, 249, 248, 246, 245, 244, 243, 241, 240, 239,
	237, 235, 234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 213, 211, 209,
	206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 179, 177, 174, 171, 168,
	165, 162, 159, 156, 153, 150, 147, 144, 140, 137, 134, 131, 128, 125, 122,
	119, 116, 112, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79, 77, 74, 71,
	68, 65, 63, 60, 57, 55, 52, 50, 47, 45, 43, 40, 38, 36, 34, 32, 30, 28, 26,
	24, 22, 21, 19, 17, 16, 15, 13, 12, 11, 10, 8, 7, 6, 6, 5, 4, 3, 3, 2, 2, 2,
	1, 1, 1
};

/// Scaling factor to match the generated frequency to the desired frequency
const uint32_t scaling = 10 * F_CPU / 256;

/// What point in the sine wave should be output
volatile uint8_t phase = 0;

/// Every period, advance the sine wave by one step
ISR(TIMER1_CAPT_vect) {
	PORTD = sine[phase++];
}

/**
 * When the PTT is activated, start the tone output. When the PTT is released,
 * stop the tone output and reset the sine wave to zero 
 */
ISR(PCINT0_vect) {
	if (PINB & ((1<<PB0))) {
		TCCR1B |= ((1<<CS10));
	} else {
		TCCR1B &= ~((1<<CS10));
		phase = 0;
		PORTD = 0b0;
	}
}

int main(void) {

	/// set up timer 1 in CTC mode
	TCCR1B |= ((1<<WGM13)) | ((1<<WGM12));
	ICR1 |= 0xffff;
	TIMSK1 |= ((1<<ICIE1));

	/// IO setup
	DDRB = 0b0;
	PORTB = 0b0;
	DDRC = 0b0;
	PORTC = 0b0;
	DDRD = 0b11111111;
	PORTD = 0b0;

	/// PCINT setup for PB0
	PCICR |= ((1<<PCIE0));
	PCMSK0 |= ((1<<PCINT0));

	/// enable global interrupts
	sei();

	/**
	 * Reads the input from the main radio proseccor and sets the tone frequency
	 * accordingly. Also serves as the loop that wastes time waiting for an
	 * interrupt to fire.
	 */
	while (1) {
		switch(PINC & ~((1<<6))) {
			case 0x3f: 
				ICR1 = scaling / 670;
				break;
			case 0x2f: 
				ICR1 = scaling / 719;
				break;
			case 0x37: 
				ICR1 = scaling / 744;
				break;
			case 0x0f: 
				ICR1 = scaling / 770;
				break;
			case 0x3b: 
				ICR1 = scaling / 797;
				break;
			case 0x27: 
				ICR1 = scaling / 825;
				break;
			case 0x33: 
				ICR1 = scaling / 854;
				break;
			case 0x07: 
				ICR1 = scaling / 885;
				break;
			case 0x3d: 
				ICR1 = scaling / 915;
				break;
			case 0x2b: 
				ICR1 = scaling / 948;
				break;
			case 0x0b: 
				ICR1 = scaling / 1000;
				break;
			case 0x23: 
				ICR1 = scaling / 1035;
				break;
			case 0x03: 
				ICR1 = scaling / 1072;
				break;
			case 0x2d: 
				ICR1 = scaling / 1109;
				break;
			case 0x0d: 
				ICR1 = scaling / 1148;
				break;
			case 0x25: 
				ICR1 = scaling / 1188;
				break;
			case 0x05: 
				ICR1 = scaling / 1230;
				break;
			case 0x29: 
				ICR1 = scaling / 1273;
				break;
			case 0x09: 
				ICR1 = scaling / 1318;
				break;
			case 0x21: 
				ICR1 = scaling / 1365;
				break;
			case 0x01: 
				ICR1 = scaling / 1413;
				break;
			case 0x2e: 
				ICR1 = scaling / 1462;
				break;
			case 0x0e: 
				ICR1 = scaling / 1514;
				break;
			case 0x26: 
				ICR1 = scaling / 1567;
				break;
			case 0x06: 
				ICR1 = scaling / 1622;
				break;
			case 0x2a: 
				ICR1 = scaling / 1679;
				break;
			case 0x0a: 
				ICR1 = scaling / 1738;
				break;
			case 0x22: 
				ICR1 = scaling / 1799;
				break;
			case 0x02: 
				ICR1 = scaling / 1862;
				break;
			case 0x2c: 
				ICR1 = scaling / 1928;
				break;
			case 0x0c: 
				ICR1 = scaling / 2035;
				break;
			case 0x24: 
				ICR1 = scaling / 2107;
				break;
			case 0x04: 
				ICR1 = scaling / 2181;
				break;
			case 0x28: 
				ICR1 = scaling / 2257;
				break;
			case 0x08: 
				ICR1 = scaling / 2336;
				break;
			case 0x20: 
				ICR1 = scaling / 2418;
				break;
			case 0x00: 
				ICR1 = scaling / 2503;
				break;
			default: 
				ICR1 = scaling / 600;
				break;
		}
	}

}