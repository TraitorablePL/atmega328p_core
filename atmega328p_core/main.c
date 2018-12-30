/*
 * atmega328p_core.c
 *
 * Created: 30.12.2018 16:07:56
 * Author : Trait
 */ 

#include <avr/io.h>
#include <util/delay.h>
 
// 7seg number masks 
static const uint8_t __flash DIGITS[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
	
// Whole display update frequency
const uint8_t DisplayFreq = 50;

// PD0 to PD6 are configured as a to g segments
// PB0 to PB3 are configured as 1st to 4th display segment

void Set7SegNumber(const uint8_t value){
	PORTD = DIGITS[value];
}

void Init7SegIO(void){
	DDRB = 0xF;
	DDRD = 0xFF;
}

void Clear7SegDisplay(void){
	PORTB = 0xF;
	PORTD = 0xFF;
}

void Update7SegDisplay(const uint8_t position){
	PORTB = 1<<position;
	PORTD = DIGITS[0];
	_delay_ms((1000/DisplayFreq)/4);
}

int main(void)
{
	Init7SegIO();
	
    while (1) 
	{
		Clear7SegDisplay();
		Update7SegDisplay(0);
		Update7SegDisplay(1);
		Update7SegDisplay(2);
		Update7SegDisplay(3);
    }
}