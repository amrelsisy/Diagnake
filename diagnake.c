#include "C:\users\student\Desktop\etc\pcd8544.c"
#include "C:\users\student\Desktop\etc\pcd8544.h"
#include "C:\users\student\Desktop\interrupts\timer.h"
#include "C:\users\student\Desktop\bitmanip\bit.h"
#include <stdio.h>
#include <stdlib.h>

#define LCD_WIDTH  84
#define LCD_HEIGHT 48
#define CHAR_WIDTH  6
#define CHAR_HEIGHT 8


#define A4 4000
#define B4 00
#define C4 5000
#define D4 00
#define E4 8000
#define F4 00
#define G4 5000
#define H4 3500
#define I4 400
#define J4 3500
#define K4 800
#define L4 9000


int food_X = 0;
int food_Y = 0;
int gone = 0;
int P1_grow = 0;
unsigned period_length = 1000;

void set_PWM(double frequency) {
	
	
	// Keeps track of the currently set frequency
	// Will only update the registers when the frequency
	// changes, plays music uninterrupted.
	static double current_frequency;
	if (frequency != current_frequency) {

		if (!frequency) TCCR3B &= 0x08; //stops timer/counter
		else TCCR3B |= 0x03; // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) OCR3A = 0xFFFF;
		
		// prevents OCR3A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) OCR3A = 0x0000;
		
		// set OCR3A based on desired frequency
		else OCR3A = (short)(8000000 / (128 * frequency)) - 1;

		TCNT3 = 0; // resets counter
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

unsigned P1_head_x = 20;
unsigned P1_head_y = 20;
unsigned P1_x1 = 19;
unsigned P1_x2 = 18;
unsigned P1_x3 = 17;
unsigned P1_x4 = 16;
unsigned P1_x5 = 15;
unsigned P1_x6 = 14;
unsigned P1_x7 = 13;
unsigned P1_x8 = 12;
unsigned P1_x9 = 11;
unsigned P1_x10= 10;
unsigned p1_x11;
unsigned p1_x12;
unsigned p1_x13;
unsigned p1_x14;
unsigned p1_x15;


unsigned P1_y1 = 20;
unsigned P1_y2 = 20;
unsigned P1_y3 = 20;
unsigned P1_y4 = 20;
unsigned P1_y5 = 20;
unsigned P1_y6 = 20;
unsigned P1_y7 = 20;
unsigned P1_y8 = 20;
unsigned P1_y9 = 20;
unsigned P1_y10= 20;
unsigned P1_y11;
unsigned P1_y12;
unsigned P1_y13;
unsigned P1_y14;
unsigned P1_y15;


unsigned char dot = '.';
int pause = 0;
int ready_to_play = 0;

int count = 0;
int boost = 0;
int boosted = 0;
unsigned long score = 50000;
enum SM1_states{P1_welcome, P1_wait_paused, P1_pause, P1_init, P1_up_right, P1_up_left, P1_down_right, P1_down_left, P1_left, P1_right, P1_done}SM1_state;
void SM1_tick()
{
	switch(SM1_state)
	{
		case P1_welcome:
		if(!GetBit(PIND, 1))
		{
			SM1_state = P1_welcome;
			period_length = 200;
			TimerSet(period_length);
			count++;
		}
		else if(!GetBit(PIND, 3))
		{
			SM1_state = P1_welcome;
			period_length = 160;
			TimerSet(period_length);
			count++;
		}
		else if(!GetBit(PIND, 0))
		{
			SM1_state = P1_welcome;
			period_length = 110;
			TimerSet(period_length);
			count++;
		}		
		else if(count > 0)
		{
			SM1_state = P1_init;
			count = 0;
		}		
		else
			SM1_state = P1_welcome;
		break;
		
		case P1_init:
		if(!GetBit(PIND, 6))
		{
			SM1_state = P1_pause;
			pause = 1;
		}
		else if(((!GetBit(PIND, 0) && !GetBit(PIND, 1) || !GetBit(PIND, 0) && !GetBit(PIND, 2)) || !GetBit(PIND, 0) && !GetBit(PIND, 3)))
		SM1_state = P1_right;
		else if(((!GetBit(PIND, 1) && !GetBit(PIND, 2) || !GetBit(PIND, 1) && !GetBit(PIND, 3))))
		SM1_state = P1_right;
		else if(((!GetBit(PIND, 2) && !GetBit(PIND, 3))))
		SM1_state = P1_right;
		else if(!(GetBit(PIND, 0)))
		SM1_state = P1_right;
		else if(!(GetBit(PIND, 1)))
		SM1_state = P1_left;
		else if(!(GetBit(PIND, 2)))
		SM1_state = P1_up_right;
		else if (!(GetBit(PIND, 3)))
		SM1_state = P1_down_right;
		else
		SM1_state = P1_right;
		break;
		
		case P1_down_right:
		if(!GetBit(PIND, 5))
		{
			if(boost == 0)
			{
				period_length -= 60 ;
				TimerSet(period_length);
			}
			boost = 1;
		}
		else if(GetBit(PIND, 5) && boost == 1)
		{
			period_length += 60 ;
			TimerSet(period_length);
			boost = 0;
			boosted = 0;
		}
		if(P1_grow == 5)
			SM1_state = P1_done;
		else if(!GetBit(PIND, 6))
		{
			pause = 2;
			SM1_state = P1_pause;	
		}
		else if(!GetBit(PIND, 0) && !GetBit(PIND, 1))
		SM1_state = P1_down_right;
		else if(!(GetBit(PIND, 0)))
		SM1_state = P1_right;
		else
		SM1_state = P1_down_right;
		break;
		
		
		case P1_down_left:
		if(!GetBit(PIND, 5))
		{
			if(boost == 0)
			{
				period_length -= 60 ;
				TimerSet(period_length);	
			}
			boost = 1;
		}
		else if(GetBit(PIND, 5) && boost == 1)
		{
			period_length += 60 ;
			TimerSet(period_length);
			boost = 0;
		}
		if(P1_grow == 5)
			SM1_state = P1_done;
		else if(!GetBit(PIND, 6))
		{
			SM1_state = P1_pause;
			pause = 3;
		}
		else if(!GetBit(PIND, 0) && !GetBit(PIND, 1))
		SM1_state = P1_down_left;
		else if(!(GetBit(PIND, 1)))
		SM1_state = P1_left;
		else
		SM1_state = P1_down_left;
		break;
		
		
		case P1_up_right:
		if(!GetBit(PIND, 5))
		{
			if(boost == 0)
			{
				period_length -= 60 ;
				TimerSet(period_length);
			}
			boost = 1;
		}
		else if(GetBit(PIND, 5) && boost == 1)
		{
			period_length += 60 ;
			TimerSet(period_length);
			boost = 0;
		}
		if(P1_grow == 5)
			SM1_state = P1_done;
		else if(!GetBit(PIND, 6))
		{
			pause = 4;	
			SM1_state = P1_pause;
		}		
		else if(!GetBit(PIND, 0) && !GetBit(PIND, 1))
		SM1_state = P1_up_right;
		else if(!(GetBit(PIND, 0)))
		SM1_state = P1_right;
		else
		SM1_state = P1_up_right;
		break;
		
		
		case P1_up_left:
		if(!GetBit(PIND, 5))
		{
			if(boost == 0)
			{
				period_length -= 60 ;
				TimerSet(period_length);
			}
			boost = 1;
		}
		else if(GetBit(PIND, 5) && boost == 1)
		{
			period_length += 60 ;
			TimerSet(period_length);
			boost = 0;
		}
		if(P1_grow == 5)
			SM1_state = P1_done;
		else if(!GetBit(PIND, 6))
		{
			SM1_state = P1_pause;
			pause = 5;
		}
		else if(!GetBit(PIND, 0) && !GetBit(PIND, 1))
		SM1_state = P1_up_left;
		else if(!(GetBit(PIND, 1)))
		SM1_state = P1_left;
		else
		SM1_state = P1_up_left;
		break;
		
		
		case P1_right:
		if(!GetBit(PIND, 5))
		{
			if(boost == 0)
			{
				period_length -= 60 ;
				TimerSet(period_length);
			}
			boost = 1;
		}
		else if(GetBit(PIND, 5) && boost == 1)
		{
			period_length += 60 ;
			TimerSet(period_length);
			boost = 0;
		}
		if(P1_grow == 5)
			SM1_state = P1_done;
		else if(!GetBit(PIND, 6))
		{
			SM1_state = P1_pause;
			pause = 6;
		}
		else if(!GetBit(PIND, 2) && !GetBit(PIND, 3))
		SM1_state = P1_right;
		else if(!(GetBit(PIND, 2)))
		SM1_state = P1_up_right;
		else if(!(GetBit(PIND, 3)))
		SM1_state = P1_down_right;
		else if(!(GetBit(PIND, 1)))
		SM1_state = P1_left;
		else
		SM1_state = P1_right;
		break;
		
		case P1_left:
		if(!GetBit(PIND, 5))
		{
			if(boost == 0)
			{
				period_length -= 60 ;
				TimerSet(period_length);
			}
			boost = 1;
		}
		else if(GetBit(PIND, 5) && boost == 1)
		{
			period_length += 60 ;
			TimerSet(period_length);
			boost = 0;
		}
		if(P1_grow == 5)
			SM1_state = P1_done;
		else if(!GetBit(PIND, 6))
		{
			SM1_state = P1_pause;
			pause = 7;
		}
		else if(!GetBit(PIND, 2) && !GetBit(PIND, 3))
		SM1_state = P1_left;
		else if(!(GetBit(PIND, 2)))
		SM1_state = P1_up_left;
		else if(!(GetBit(PIND, 3)))
		SM1_state = P1_down_left;
		else if(!(GetBit(PIND, 0)))
		SM1_state = P1_right;
		else
		SM1_state = P1_left;
		break;
		
		case P1_pause:
		SM1_state = P1_wait_paused;
		break;
		
		case P1_wait_paused:
		if(!GetBit(PIND, 5))
			SM1_state = P1_welcome;
		else if(!GetBit(PIND, 6))
		{
			if(pause == 1)
				SM1_state = P1_init;
			else if(pause == 2)
				SM1_state = P1_down_right;
			else if(pause == 3)
				SM1_state = P1_down_left;
			else if(pause == 4)
				SM1_state = P1_up_right;
			else if(pause == 5)
				SM1_state = P1_up_left;
			else if(pause == 6)
				SM1_state = P1_right;
			else if(pause == 7)
				SM1_state = P1_left;		
		}
		else
		SM1_state = P1_wait_paused;
		break;
		
		case P1_done:
		if(!GetBit(PIND, 5))
		{
			SM1_state = P1_welcome;
		}		
		else
			SM1_state = P1_done;
		break;
		
		default:
		SM1_state = P1_welcome;
		break;
	}
	switch(SM1_state)
	{
		case P1_welcome:
		gotoXY(0, 0);
		LCDChar('W');
		gotoXY(7, 0);
		LCDChar('e');
		gotoXY(14, 0);
		LCDChar('l');
		gotoXY(21, 0);
		LCDChar('c');
		gotoXY(28, 0);
		LCDChar('o');
		gotoXY(35, 0);
		LCDChar('m');
		gotoXY(42, 0);
		LCDChar('e');
		
		gotoXY(50, 0);
		LCDChar('t');
		gotoXY(57, 0);
		LCDChar('o');
		
		gotoXY(0, 1);
		LCDChar('D');
		gotoXY(7, 1);
		LCDChar('I');
		gotoXY(14, 1);
		LCDChar('A');
		gotoXY(21, 1);
		LCDChar('G');
		gotoXY(28, 1);
		LCDChar('N');
		gotoXY(35, 1);
		LCDChar('A');
		gotoXY(42, 1);
		LCDChar('K');
		gotoXY(49, 1);
		LCDChar('E');
		
		gotoXY(0, 3);
		LCDChar('b');
		gotoXY(7, 3);
		LCDChar('y');
		gotoXY(14, 3);
		LCDChar(':');
		
		gotoXY(24, 3);
		LCDChar('A');
		gotoXY(31, 3);
		LCDChar('m');
		gotoXY(38, 3);
		LCDChar('r');
		
		gotoXY(0, 5);
		LCDChar('L');
		gotoXY(7, 5);
		LCDChar('e');
		gotoXY(14, 5);
		LCDChar('v');
		gotoXY(21, 5);
		LCDChar('e');
		gotoXY(28, 5);
		LCDChar('l');
		gotoXY(35, 5);
		LCDChar('?');
		gotoXY(45, 5);
		LCDChar('1');
		gotoXY(55, 5);
		LCDChar('2');
		gotoXY(65, 5);
		LCDChar('3');
		gotoXY(75, 5);
		break;
		
		case P1_wait_paused:
		ready_to_play = 0;
		gotoXY(0, 1);
		LCDChar('P');
		gotoXY(7, 1);
		LCDChar('a');
		gotoXY(14, 1);
		LCDChar('u');
		gotoXY(21, 1);
		LCDChar('s');
		gotoXY(28, 1);
		LCDChar('e');
		gotoXY(35, 1);
		LCDChar('d');
		break;
		
		case P1_done:
		ready_to_play = 0;
		gotoXY(0, 1);
		LCDChar('L');
		gotoXY(7, 1);
		LCDChar('e');
		gotoXY(14, 1);
		LCDChar('v');
		gotoXY(21, 1);
		LCDChar('e');
		gotoXY(28, 1);
		LCDChar('l');
		gotoXY(35, 1);
		LCDChar(':');
		if(score > 11000)
		{
			gotoXY(0, 3);
			LCDChar('L');
			gotoXY(7, 3);
			LCDChar('e');
			gotoXY(14, 3);
			LCDChar('g');
			gotoXY(21, 3);
			LCDChar('e');
			gotoXY(28, 3);
			LCDChar('n');
			gotoXY(35, 3);
			LCDChar('d');
		}	
		else if(score > 9500)
		{
			gotoXY(0, 3);
			LCDChar('A');
			gotoXY(7, 3);
			LCDChar('v');
			gotoXY(14, 3);
			LCDChar('e');
			gotoXY(21, 3);
			LCDChar('r');
			gotoXY(28, 3);
			LCDChar('a');
			gotoXY(35, 3);
			LCDChar('g');
			gotoXY(42, 3);
			LCDChar('e');
			
		}
		else if(score > 8000)
		{
			gotoXY(0, 3);
			LCDChar('N');
			gotoXY(7, 3);
			LCDChar('e');
			gotoXY(14, 3);
			LCDChar('w');
			gotoXY(21, 3);
			LCDChar('b');
			gotoXY(28, 3);
			LCDChar('i');
			gotoXY(35, 3);
			LCDChar('e');
		}		
		else
		{
				
			gotoXY(0, 3);
			LCDChar('T');
			gotoXY(7, 3);
			LCDChar('e');
			gotoXY(14, 3);
			LCDChar('r');
			gotoXY(21, 3);
			LCDChar('r');
			gotoXY(28, 3);
			LCDChar('i');
			gotoXY(35, 3);
			LCDChar('b');
			gotoXY(42, 3);
			LCDChar('l');
			gotoXY(49, 3);
			LCDChar('e');
			
				
		}
		
		break;
		
		case P1_init:
		score = 10000;
		P1_grow = 0;
		ready_to_play = 1;
		P1_head_x = 20;
		P1_head_y = 20;
		P1_x1 = 19;
		P1_x2 = 18;
		P1_x3 = 17;
		P1_x4 = 16;
		P1_x5 = 15;
		P1_x6 = 14;
		P1_x7 = 13;
		P1_x8 = 12;
		P1_x9 = 11;
		P1_x10= 10;

		P1_y1 = 20;
		P1_y2 = 20;
		P1_y3 = 20;
		P1_y4 = 20;
		P1_y5 = 20;
		P1_y6 = 20;
		P1_y7 = 20;
		P1_y8 = 20;
		P1_y9 = 20;
		P1_y10= 20;
		setPixel(P1_head_x, P1_head_y);
		setPixel(P1_x1, P1_y1);
		setPixel(P1_x2, P1_y2);
		setPixel(P1_x3, P1_y3);
		setPixel(P1_x4, P1_y4);
		setPixel(P1_x5, P1_y5);
		setPixel(P1_x6, P1_y6);
		setPixel(P1_x7, P1_y7);
		setPixel(P1_x8, P1_y8);
		setPixel(P1_x9, P1_y9);
		setPixel(P1_x10,P1_y10);
		break;
		
		case P1_down_right:
		ready_to_play = 1;
		P1_y15 = P1_y14;
		P1_y14 = P1_y13;
		P1_y13 = P1_y12;
		P1_y12 = P1_y11;
		P1_y11 = P1_y10;
		P1_y10= P1_y9;
		P1_y9 = P1_y8;
		P1_y8 = P1_y7;
		P1_y7 = P1_y6;
		P1_y6 = P1_y5;
		P1_y5 = P1_y4;
		P1_y4 = P1_y3;
		P1_y3 = P1_y2;
		P1_y2 = P1_y1;
		P1_y1 = P1_head_y;
		
		p1_x15 = p1_x14;
		p1_x14 = p1_x13;
		p1_x13 = p1_x12;
		p1_x12 = p1_x11;
		p1_x11 = P1_x10;		
		P1_x10= P1_x9;
		P1_x9 = P1_x8;
		P1_x8 = P1_x7;
		P1_x7 = P1_x6;
		P1_x6 = P1_x5;
		P1_x5 = P1_x4;
		P1_x4 = P1_x3;
		P1_x3 = P1_x2;
		P1_x2 = P1_x1;
		P1_x1 = P1_head_x;
		
		if(P1_head_y == 47)
		P1_head_y = 0;
		else
		P1_head_y++;
		
		if(P1_head_x == 84)
		P1_head_x = 0;
		else
		P1_head_x++;
		
		
		setPixel(P1_head_x, P1_head_y);
		setPixel(P1_x1,P1_y1);
		setPixel(P1_x2,P1_y2);
		setPixel(P1_x3,P1_y3);
		setPixel(P1_x4,P1_y4);
		setPixel(P1_x5,P1_y5);
		setPixel(P1_x6,P1_y6);
		setPixel(P1_x7,P1_y7);
		setPixel(P1_x8,P1_y8);
		setPixel(P1_x9,P1_y9);
		setPixel(P1_x10,P1_y10);
		if(P1_grow >= 1)
		setPixel(p1_x11,P1_y11);
		if(P1_grow >= 2)
		setPixel(p1_x12,P1_y12);
		if(P1_grow >= 3)
		setPixel(p1_x13,P1_y13);
		if(P1_grow >= 4)
		setPixel(p1_x14,P1_y14);
		if(P1_grow >= 5)
		setPixel(p1_x15,P1_y15);
			
		break;
		case P1_down_left:
		ready_to_play = 1;
		P1_y15 = P1_y14;
		P1_y14 = P1_y13;
		P1_y13 = P1_y12;
		P1_y12 = P1_y11;
		P1_y11 = P1_y10;
		P1_y10= P1_y9;
		P1_y9 = P1_y8;
		P1_y8 = P1_y7;
		P1_y7 = P1_y6;
		P1_y6 = P1_y5;
		P1_y5 = P1_y4;
		P1_y4 = P1_y3;
		P1_y3 = P1_y2;
		P1_y2 = P1_y1;
		P1_y1 = P1_head_y;
		
		p1_x15 = p1_x14;
		p1_x14 = p1_x13;
		p1_x13 = p1_x12;
		p1_x12 = p1_x11;
		p1_x11 = P1_x10;
		P1_x10= P1_x9;
		P1_x9 = P1_x8;
		P1_x8 = P1_x7;
		P1_x7 = P1_x6;
		P1_x6 = P1_x5;
		P1_x5 = P1_x4;
		P1_x4 = P1_x3;
		P1_x3 = P1_x2;
		P1_x2 = P1_x1;
		P1_x1 = P1_head_x;
		
		if(P1_head_y == 47)
		P1_head_y = 0;
		else
		P1_head_y++;
		
		if(P1_head_x == 0)
		P1_head_x = 84;
		else
		P1_head_x--;
		
		
		setPixel(P1_head_x, P1_head_y);
		setPixel(P1_x1,P1_y1);
		setPixel(P1_x2,P1_y2);
		setPixel(P1_x3,P1_y3);
		setPixel(P1_x4,P1_y4);
		setPixel(P1_x5,P1_y5);
		setPixel(P1_x6,P1_y6);
		setPixel(P1_x7,P1_y7);
		setPixel(P1_x8,P1_y8);
		setPixel(P1_x9,P1_y9);
		setPixel(P1_x10,P1_y10);
		if(P1_grow >= 1)
		setPixel(p1_x11,P1_y11);
		if(P1_grow >= 2)
		setPixel(p1_x12,P1_y12);
		if(P1_grow >= 3)
		setPixel(p1_x13,P1_y13);
		if(P1_grow >= 4)
		setPixel(p1_x14,P1_y14);
		if(P1_grow >= 5)
		setPixel(p1_x15,P1_y15);
		
		break;
		
		case P1_right:
		
		ready_to_play = 1;
		P1_y15 = P1_y14;
		P1_y14 = P1_y13;
		P1_y13 = P1_y12;
		P1_y12 = P1_y11;
		P1_y11 = P1_y10;
		P1_y10= P1_y9;
		P1_y9 = P1_y8;
		P1_y8 = P1_y7;
		P1_y7 = P1_y6;
		P1_y6 = P1_y5;
		P1_y5 = P1_y4;
		P1_y4 = P1_y3;
		P1_y3 = P1_y2;
		P1_y2 = P1_y1;
		P1_y1 = P1_head_y;
	
		p1_x15 = p1_x14;
		p1_x14 = p1_x13;
		p1_x13 = p1_x12;
		p1_x12 = p1_x11;
		p1_x11 = P1_x10;
		P1_x10= P1_x9;
		P1_x9 = P1_x8;
		P1_x8 = P1_x7;
		P1_x7 = P1_x6;
		P1_x6 = P1_x5;
		P1_x5 = P1_x4;
		P1_x4 = P1_x3;
		P1_x3 = P1_x2;
		P1_x2 = P1_x1;
		P1_x1 = P1_head_x;
		
		if(P1_head_x == 84)
		P1_head_x = 0;
		else
		P1_head_x++;
		
		setPixel(P1_head_x, P1_head_y);
		setPixel(P1_x1,P1_y1);
		setPixel(P1_x2,P1_y2);
		setPixel(P1_x3,P1_y3);
		setPixel(P1_x4,P1_y4);
		setPixel(P1_x5,P1_y5);
		setPixel(P1_x6,P1_y6);
		setPixel(P1_x7,P1_y7);
		setPixel(P1_x8,P1_y8);
		setPixel(P1_x9,P1_y9);
		setPixel(P1_x10,P1_y10);
		
		if(P1_grow >= 1)
		setPixel(p1_x11,P1_y11);
		if(P1_grow >= 2)
		setPixel(p1_x12,P1_y12);
		if(P1_grow >= 3)
		setPixel(p1_x13,P1_y13);
		if(P1_grow >= 4)
		setPixel(p1_x14,P1_y14);
		if(P1_grow >= 5)
		setPixel(p1_x15,P1_y15);
		break;
		
		
		case P1_up_right:
		ready_to_play = 1;
		P1_y15 = P1_y14;
		P1_y14 = P1_y13;
		P1_y13 = P1_y12;
		P1_y12 = P1_y11;
		P1_y11 = P1_y10;
		P1_y10= P1_y9;
		P1_y9 = P1_y8;
		P1_y8 = P1_y7;
		P1_y7 = P1_y6;
		P1_y6 = P1_y5;
		P1_y5 = P1_y4;
		P1_y4 = P1_y3;
		P1_y3 = P1_y2;
		P1_y2 = P1_y1;
		P1_y1 = P1_head_y;
		
		p1_x15 = p1_x14;
		p1_x14 = p1_x13;
		p1_x13 = p1_x12;
		p1_x12 = p1_x11;
		p1_x11 = P1_x10;
		P1_x10= P1_x9;
		P1_x9 = P1_x8;
		P1_x8 = P1_x7;
		P1_x7 = P1_x6;
		P1_x6 = P1_x5;
		P1_x5 = P1_x4;
		P1_x4 = P1_x3;
		P1_x3 = P1_x2;
		P1_x2 = P1_x1;
		P1_x1 = P1_head_x;
		
		if(P1_head_y == 0)
		P1_head_y = 47;
		else
		P1_head_y--;
		
		if(P1_head_x == 84)
		P1_head_x = 0;
		else
		P1_head_x++;
		
		setPixel(P1_head_x, P1_head_y);
		setPixel(P1_x1,P1_y1);
		setPixel(P1_x2,P1_y2);
		setPixel(P1_x3,P1_y3);
		setPixel(P1_x4,P1_y4);
		setPixel(P1_x5,P1_y5);
		setPixel(P1_x6,P1_y6);
		setPixel(P1_x7,P1_y7);
		setPixel(P1_x8,P1_y8);
		setPixel(P1_x9,P1_y9);
		setPixel(P1_x10,P1_y10);
		if(P1_grow >= 1)
		setPixel(p1_x11,P1_y11);
		if(P1_grow >= 2)
		setPixel(p1_x12,P1_y12);
		if(P1_grow >= 3)
		setPixel(p1_x13,P1_y13);
		if(P1_grow >= 4)
		setPixel(p1_x14,P1_y14);
		if(P1_grow >= 5)
		setPixel(p1_x15,P1_y15);
		break;
		
		case P1_up_left:
		ready_to_play = 1;
		P1_y15 = P1_y14;
		P1_y14 = P1_y13;
		P1_y13 = P1_y12;
		P1_y12 = P1_y11;
		P1_y11 = P1_y10;
		P1_y10= P1_y9;
		P1_y9 = P1_y8;
		P1_y8 = P1_y7;
		P1_y7 = P1_y6;
		P1_y6 = P1_y5;
		P1_y5 = P1_y4;
		P1_y4 = P1_y3;
		P1_y3 = P1_y2;
		P1_y2 = P1_y1;
		P1_y1 = P1_head_y;
		
		p1_x15 = p1_x14;
		p1_x14 = p1_x13;
		p1_x13 = p1_x12;
		p1_x12 = p1_x11;
		p1_x11 = P1_x10;
		P1_x10= P1_x9;
		P1_x9 = P1_x8;
		P1_x8 = P1_x7;
		P1_x7 = P1_x6;
		P1_x6 = P1_x5;
		P1_x5 = P1_x4;
		P1_x4 = P1_x3;
		P1_x3 = P1_x2;
		P1_x2 = P1_x1;
		P1_x1 = P1_head_x;
		
		if(P1_head_y == 0)
		P1_head_y = 47;
		else
		P1_head_y--;
		
		if(P1_head_x == 0)
		P1_head_x = 84;
		else
		P1_head_x--;
		
		setPixel(P1_head_x, P1_head_y);
		setPixel(P1_x1,P1_y1);
		setPixel(P1_x2,P1_y2);
		setPixel(P1_x3,P1_y3);
		setPixel(P1_x4,P1_y4);
		setPixel(P1_x5,P1_y5);
		setPixel(P1_x6,P1_y6);
		setPixel(P1_x7,P1_y7);
		setPixel(P1_x8,P1_y8);
		setPixel(P1_x9,P1_y9);
		setPixel(P1_x10,P1_y10);
		if(P1_grow >= 1)
		setPixel(p1_x11,P1_y11);
		if(P1_grow >= 2)
		setPixel(p1_x12,P1_y12);
		if(P1_grow >= 3)
		setPixel(p1_x13,P1_y13);
		if(P1_grow >= 4)
		setPixel(p1_x14,P1_y14);
		if(P1_grow >= 5)
		setPixel(p1_x15,P1_y15);
		break;
		
		
		case P1_left:
	
		ready_to_play = 1;
		P1_y15 = P1_y14;
		P1_y14 = P1_y13;
		P1_y13 = P1_y12;
		P1_y12 = P1_y11;
		P1_y11 = P1_y10;
		P1_y10= P1_y9;
		P1_y9 = P1_y8;
		P1_y8 = P1_y7;
		P1_y7 = P1_y6;
		P1_y6 = P1_y5;
		P1_y5 = P1_y4;
		P1_y4 = P1_y3;
		P1_y3 = P1_y2;
		P1_y2 = P1_y1;
		P1_y1 = P1_head_y;
	
		
		p1_x15 = p1_x14;
		p1_x14 = p1_x13;
		p1_x13 = p1_x12;
		p1_x12 = p1_x11;
		p1_x11 = P1_x10;
		P1_x10= P1_x9;
		P1_x9 = P1_x8;
		P1_x8 = P1_x7;
		P1_x7 = P1_x6;
		P1_x6 = P1_x5;
		P1_x5 = P1_x4;
		P1_x4 = P1_x3;
		P1_x3 = P1_x2;
		P1_x2 = P1_x1;
		P1_x1 = P1_head_x;
		
		if(P1_head_x == 0)
		P1_head_x = 84;
		else
		P1_head_x--;
		
		setPixel(P1_head_x, P1_head_y);
		setPixel(P1_x1,P1_y1);
		setPixel(P1_x2,P1_y2);
		setPixel(P1_x3,P1_y3);
		setPixel(P1_x4,P1_y4);
		setPixel(P1_x5,P1_y5);
		setPixel(P1_x6,P1_y6);
		setPixel(P1_x7,P1_y7);
		setPixel(P1_x8,P1_y8);
		setPixel(P1_x9,P1_y9);
		setPixel(P1_x10,P1_y10);
		
		if(P1_grow >= 1)
		setPixel(p1_x11,P1_y11);
		if(P1_grow >= 2)
		setPixel(p1_x12,P1_y12);
		if(P1_grow >= 3)
		setPixel(p1_x13,P1_y13);
		if(P1_grow >= 4)
		setPixel(p1_x14,P1_y14);
		if(P1_grow >= 5)
		setPixel(p1_x15,P1_y15);
		
		
		break;
		
		
		default:
		break;
	}
}


enum food_states{food_wait, food_eat, food_move}food_state;

void food_tick()
{
	
	switch(food_state)
	{
		case food_wait:
		if(P1_head_y > food_Y)
		{
			if(((P1_head_y - food_Y) < 5) && ((P1_head_x - food_X) < 5))
				food_state = food_move;		 
		}		
		else if(food_Y > P1_head_y)
		{
			if(((food_Y - P1_head_y) < 5) && ((food_X - P1_head_x) < 5)) 
				food_state = food_move;
		}				
		else if((P1_head_x == food_X) && (P1_head_y == food_Y))
			food_state = food_eat;
		else
			food_state = food_wait;	
		break;
		
		case food_eat:
			food_state = food_wait;
		break;
		
		default:
		food_state = food_wait;
		break;
	}
	switch(food_state)
	{
		case food_wait:
		gone = 1;
		setPixel(food_X, food_Y);
		break;
		
		case food_eat:
		P1_grow = P1_grow + 1;
		gone = 0;
		break;
		
		case food_move:
		gone = 0;
		break;
		
		default:
		break;
	}
}

enum sound_States{one, two, three, four, five, six, seven, eight, nine, ten, eleven, twelve}sound_state;

void sound_tick()
{
	switch(sound_state)
	{
		case one:
		sound_state = two;
		break;
		
		case two:
		sound_state = three;
		break;
		
		case three:
		sound_state = four;
		break;
		
		case four:
		sound_state = five;
		break;
		
		case five:
		sound_state = six;
		break;
		
		case six:
		sound_state = seven;
		break;
		
		case seven:
		sound_state = eight;
		break;
		
		case eight:
		sound_state = nine;
		break;
		
		case nine:
		sound_state = ten;
		break;
		
		case ten:
		sound_state = eleven;
		break;
		
		case eleven:
		sound_state = twelve;
		break;
		
		case twelve:
		sound_state = one;
		break;
		
		default:
		sound_state = one;
		break;
	}
	switch(sound_state)
	{
		case one:
		set_PWM(A4 * 5);
		break;
		
		case two:
		set_PWM(B4 * 5);
		break;
		
		case three:
		set_PWM(C4 * 5);
		break;
		
		case four:
		set_PWM(D4 * 5);
		break;
		
		case five:
		set_PWM(E4 * 5);
		break;
		
		case six:
		set_PWM(F4 * 5);
		break;
		
		case seven:
		set_PWM(G4 * 5);
		break;
		
		case eight:
		set_PWM(H4 * 5);
		break;
		
		case nine:
		set_PWM(I4 * 5);
		break;
		
		case ten:
		set_PWM(J4 * 5);
		break;
		
		case eleven:
		set_PWM(K4 * 5);
		break;
		
		case twelve:
		set_PWM(L4 * 5);
		break;
		
		
		default:
		break;
	}
}


int main(void) {

	DDRD = 0x00; PORTD = 0xff; //D is input (logic is flipped)
	DDRA = 0xff; PORTA = 0x00; //A is output
	DDRB = 0xFF; PORTB = 0xFF;
	LCDInit();
	LCDClear();
	PWM_on();
	set_PWM(0);
	//d0 -> right
	//d1 -> left
	//d2 -> up
	//d3 -> down
	
	period_length = 1;
	
	TimerOn();
	
	
	while(1)
	{
		if(gone == 0)
		{
			food_X = rand() % 84;
			food_Y = rand() % 48;
		}
		gone = 1;
	
		SM1_tick();
		if(ready_to_play == 1)
		{
			food_tick();
			score--;
		}
		sound_tick();
		while(!TimerFlag);
		TimerFlag = 0;
		LCDClear();
		
	}
}