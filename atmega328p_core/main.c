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

//////////////////////////////////////////////////////////////////////////
// Initials
//////////////////////////////////////////////////////////////////////////

// Display mods
enum _DispMode{Counter, NoAction};
	
// Counter mods
enum _CountMode{Increase, Decrease};
 
// 7seg number masks 
static const uint8_t __flash DIGITS[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
	
//////////////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////////////

// Initial display mode
enum _DispMode DispMode = Counter;

// Initial counter mode
enum _CountMode CountMode = Increase;

// Display update frequency
const uint8_t DisplayFreq = 100;

// Initial counter value ( displayed 0 - 9999 )
uint16_t MainCounter = 9999;

// PD0 to PD6 are configured as a to g segments
// PB0 to PB3 are configured as 1st to 4th display segment

void UpdateCounterValue(void){
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

void Set7SegNumber(const uint8_t value){
	PORTD = DIGITS[value];
}

void Init7SegIO(void){
	DDRB = 0x1F;
	DDRD = 0xFF;
}

void Clear7SegDisplay(void){
	PORTB = 0xF;
	PORTD = 0xFF;
}

void Update7SegDisplay(const uint8_t position){
	uint8_t num_to_display;
	PORTB = 1<<position;
	switch (position)
	{
		case 0:
			num_to_display = MainCounter/1000;
			break;
		case 1:
			num_to_display = (MainCounter%1000)/100;
			break;
		case 2:
			num_to_display = (MainCounter%100)/10;
			break;
		case 3:
			num_to_display = MainCounter%10;
			break;
		default:
			break;
	}
	PORTD = DIGITS[num_to_display];
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
/*
void ChangeDisplayedValue(const enum _DispMode mode){
	
}
*/
int main(void)
{
	Init7SegIO();
	InitTimer1(625);
	
    while (1) 
	{
		
		if((PINB & (1<<PB7))==0) {
			if(CountMode == Increase){
				CountMode = Decrease;
			}
			else {
				CountMode = Increase;
			}
		}
		
		Clear7SegDisplay();
		Update7SegDisplay(0);
		Update7SegDisplay(1);
		Update7SegDisplay(2);
		Update7SegDisplay(3);
    }
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK){
	switch (DispMode)
	{
	case NoAction:
		break;
	case Counter:
		UpdateCounterValue();
		break;
	default:
		break;	
	}
}