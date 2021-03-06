/*
 * noicegen.c
 *
 * Copyright 2014 Edward V. Emelianoff <eddy@sao.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "noicegen.h"
//#include "main.h"

/**** sound banks ****/
// x_pi = [0:15]/15*pi; x_2pi = [0:15]/15*2*pi;
// sawtooth: V = sawtooth(x_pi)+1; waveforms(V, "sawtooth")
static const U8 sawtooth[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
// sine: V = sin(x_2pi)/2+0.5; waveforms(V, "sine")
static const U8 sine[16] = {8, 11, 14, 16, 16, 15, 13, 10, 6, 3, 1, 0, 0, 2, 5, 8};
// exp: waveforms(exp([0:15]/4), "exp")
static const U8 exp[16] = {0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 6, 7, 10, 12, 16};
// log: waveforms(1-exp([0:15]/4), "log")
static const U8 log[16] = {16, 16, 16, 16, 15, 15, 15, 14, 14, 13, 12, 10, 9, 6, 4, 0};
//pseudorand: waveforms(rand(1,16), "pseudorand")
static const U8 pseudorand[16] = {1, 3, 11, 11, 9, 5, 0, 9, 6, 8, 14, 12, 6, 13, 16, 14};
static U8 *banks[5] = {sawtooth, sine, exp, log, pseudorand};
// currend sound bank
unsigned char *current_bank = sawtooth;

void configure_timers(){
	/**** TIMERS TIM1 - 1MHz, TIM2 - 1MHz ****/
	TIM1_PSCRH = 0; // this timer have 16 bit prescaler
	TIM1_PSCRL = 3; // LSB should be written last as it updates prescaler
	TIM2_PSCR = 4;
	// Timer1 is PWM sound level generator
	// Timer2 runs with F*16 to change voltage level (F - frequency of sound)
	TIM1_ARRH = 0;
	TIM1_ARRL = 16;
	TIM1_CCR1H = 0; TIM1_CCR1L = 8; // default: 50%
	// channel 1 generates PWM pulses
	TIM1_CCMR1 = 0x60; // OC1M = 110b - PWM mode 1 ( 1 -> 0)
	//TIM1_CCMR1 = 0x70; // OC1M = 111b - PWM mode 2 ( 0 -> 1)
	TIM1_CCER1 =  1; // Channel 1 is on. Active is high
	//TIM1_CCER1 =  3; // Channel 1 is on. Active is low
	// default period: near 32ms
	TIM2_ARRH = 127; TIM2_ARRL = 0;
	// interrupts: update for timer 2, none for timer 1
	TIM1_IER = 0;
	TIM2_IER = TIM_IER_UIE;
	// enable PWM output for timer1
	TIM1_BKR |= 0x80; // MOE
}
/**
 * Changes current sound "Bank" to i-th in banks
 */

void change_snd_bank(U8 i){
	current_bank = banks[i];
}
