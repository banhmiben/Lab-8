/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/GUbLjW9Uzr4
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned char power = 0x00;
unsigned char i = 0x00;
double freq[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};


unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum states {start, init, poweronoff, scaleup, scaledown, release} state;

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

void PMW_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

void sound_tick() {

	switch (state) {
		case(start):
			state = init;
			break;
		case(init):
			if ((~PINA & 0x07) == 0x01) {
				state = poweronoff;
			} else if ((~PINA & 0x07) == 0x02) {
				state = scaleup;
			} else if ((~PINA & 0x07) == 0x04) {
				state = scaledown;
			} else {
				state = init;
			} break;
		case(poweronoff):
			/*if ((~PINA & 0x07) == 0x01) {
				state = poweronoff;
			} else {
				state = release;
			} break;*/
			state = release;
			break;
		case(scaleup):
			/*if ((~PINA & 0x07) == 0x02) {
				state = scaleup;
			} else {
				state = release;
			} break;*/
			state = release;
			break;
		case(scaledown):
			/*if ((~PINA & 0x07) == 0x04) {
				state = scaledown;
			} else {
				state = release;
			} break;*/
			state = release;
			break;
		case(release):
			if ((~PINA & 0x07) == 0x00) {
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
			break;
		case(poweronoff):
			if (power == 0x00) {
				power = 0x01;
			} else {
				power = 0x00;
				set_PWM(0);
			} break;
		case(scaleup):
			if (i < 0x07) {
				++i;
			}
			if (power == 0x01) {
				set_PWM(freq[i]);
			}
			break;
		case(scaledown):
                        if (i > 0x00) {
                                --i;
                        }
                        if (power == 0x01) {
                                set_PWM(freq[i]);
                        } break;
		case(release):
			break;
		default:
			break;
	}
}



int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(25);
	TimerOn();
	PWM_on();
	while(1) {
		sound_tick();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	return 0;
}

