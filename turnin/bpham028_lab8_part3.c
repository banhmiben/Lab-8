/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/DrDJ1WBDRDw
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum states {start, init, melody, release} state;
unsigned char cnt = 0x00;
unsigned char i = 0x00;
double freq[14] = {261.63, 261.63, 392.00, 392.00, 440.00, 440.00, 392.00, 349.23, 349.23, 329.63, 329.63, 293.66,293.66,261.63};

void TimerSet(unsigned long M) {
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}


void TimerOn() {

	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;

}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {

	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}

}

void set_PWM(double frequency) {
	static double current_frequency;

	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else {OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT3 = 0;
		current_frequency = frequency;
	}

}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

void melody_tick() {

	switch(state) {
		case(start):
			state = init;
			break;
		case(init):
			if ((~PINA & 0x01) == 0x01) {
				state = melody;
			} else {
				state = init;
			} break;
		case(melody):
			if (cnt < 51) {
				state = melody;
			} else {
				state = release;
			} break;
		case(release):
			if ((~PINA & 0x01) == 0x00) {
				state = init;
			} else {
				state = release;
			} break;
		default:
			state = start;
			break;
	}

	switch(state) {
		case(start):
			break;
		case(init):
			set_PWM(0);
			i = 0;
			cnt = 0;
			break;
		case(melody):
			if (cnt < 3) {
				set_PWM(freq[i]);
			} else if (cnt < 6) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 9) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 12) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 15) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 18) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 30) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 33) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 36) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 39) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 42) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 45) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 48) {
				++i;
				set_PWM(freq[i]);
			} else if (cnt < 51) {
				++i;
				set_PWM(freq[i]);
			} ++cnt;
			break;
		case(release):
			set_PWM(0);
			i = 0x00;
			cnt = 0x00;
			break;
		default:
			break;
	}
}


int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(100);
	TimerOn();
	PWM_on();
	while(1) {
		melody_tick();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	return 0;
}
