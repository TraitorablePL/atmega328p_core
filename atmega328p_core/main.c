/*
 * atmega328p_core.c
 *
 * Created: 30.12.2018 16:07:56
 * Author : Trait
 * MCU clock freq 16MHz
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#include "I2C.h"

//////////////////////////////////////////////////////////////////////////
// Initials
//////////////////////////////////////////////////////////////////////////

// Display mods
enum _DispMode{Counter, NoAction, Temperature};
	
// Counter mods
enum _CountMode{Increase, Decrease};
 
// 7seg number masks 
static const uint8_t __flash DIGITS[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
	
//////////////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////////////

// Initial display mode
enum _DispMode DispMode = Temperature;

// Initial counter mode
enum _CountMode CountMode = Increase;

// Display update frequency
const uint8_t DisplayFreq = 100;

// Initial temperature value 
uint8_t temp_val = 0;
// Initial counter value ( displayed 0 - 9999 )
uint16_t MainCounter = 9999;

// PD0 to PD6 are configured as a to g segments
// PC0 to PC3 are configured as 1st to 4th display segment

void UpdateCounterVal(void){
	switch (CountMode)
	{
		case Increase:
			MainCounter++;
			MainCounter %= 10000;
			break;
		case Decrease: 
			if (0==MainCounter) {
				MainCounter=9999;
			}
			else {
				MainCounter--;
			}
			break;
		default:
			break;
	}
};

void Init7SegDisp(void){
	DDRC = 0x3F;
	DDRB = 0x10;
	DDRD = 0xFF;
}

void Clear7SegDisp(void){
	PORTC = 0xF;
	PORTD = 0xFF;
}

void Update7SegDisp(const uint8_t position){
	uint16_t value = 0;
	uint8_t digit = 0;
	//Could be switch
	if (DispMode == Counter){
		value = MainCounter;
	}
	else if(DispMode == Temperature){
		value = temp_val;
	}
	
	PORTC = 1<<position;
	switch (position)
	{
		case 0:
			digit = value/1000;
			break;
		case 1:
			digit= (value%1000)/100;
			break;
		case 2:
			digit = (value%100)/10;
			break;
		case 3:
			digit = value%10;
			break;
		default:
			break;
	}
	PORTD = DIGITS[digit];
	_delay_ms((1000/DisplayFreq)/4);
}

void InitTimer1(const uint16_t period){
	// Enable global interrupts
	sei();
	// Enable compare match mode and set 1/256 base freq
	TCCR1B = (1<<WGM12)|(1<<CS12);
	// Enable Timer1 compare match interrupt
	TIMSK1 = 1<<OCIE1A;
	// Set user defined period to compare with timer
	OCR1A = period;
}

int main(void)
{
	Init7SegDisp();
	InitTimer1(62500);
	
	I2C_Init();
	
	AltitudeSens_WriteReg(0x26,0xB8);
	
	AltitudeSens_WriteReg(0x13,0x07);
	
	AltitudeSens_WriteReg(0x26,0xB9);
	
    while (1) 
	{
		//TODO DEBOUNCING
		if((PINB & (1<<PB7))==0) {
			
			temp_val = AltitudeSens_ReadReg(0x04);
			/*
			if(CountMode == Increase){
				CountMode = Decrease;
			}
			else {
				CountMode = Increase;
			}
			*/
		}
		
		Clear7SegDisp();
		Update7SegDisp(0);
		Update7SegDisp(1);
		Update7SegDisp(2);
		Update7SegDisp(3);
    }
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK){
	switch (DispMode)
	{
	case NoAction:
		break;
	case Counter:
		UpdateCounterVal();
		break;
	case Temperature:
		temp_val = AltitudeSens_ReadReg(0x04);
		break;	
	default:
		break;	
	}
}