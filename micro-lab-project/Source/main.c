#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "../Header/lcd.h"

// ------------------------- Global Variables Definitions -------------------------
unsigned char nums[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
char time[17];

uint8_t second, minute, hour;
uint16_t day;
// ------------------------- Global Variables Definitions -------------------------


void lcd_str_at(uint8_t x, uint8_t y, const char* data) {
	lcd_setcursor(x, y);
	lcd_string(data);
}

void lcd_char_at(uint8_t x, uint8_t y, uint8_t data) {
	lcd_setcursor(x, y);
	lcd_data(data);
}

void initialization() {
	DDRB = 0x00;  // Input for PIR sensor.

	DDRC = 0xFF;
	PORTC = 0xFF;

	lcd_init();

	timer1_init();

	sei();  // Enable global interrupt.
}

// initialize timer, interrupt and variable
void timer1_init() {
    TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12);  // set up timer with prescaler = 1024
    TCNT1 = 0;
    OCR1A = 969;
    TIMSK |= (1 << OCIE1A);  // enable compare match interrupt

    second = minute = hour = day = 0;
}

ISR(TIMER1_COMPA_vect) {
	// A second forward.
	second++;
	minute += second / 60;        second %= 60;
	hour += minute / 60;          minute %= 60;
	day += hour / 24;             hour %= 24;

	// Viewing on LCD.
	sprintf(time, "Time:%2u %2.2u:%2.2u:%2.2u", day, hour, minute, second);
	lcd_str_at(0, 1, time);
}

int main() {
	// TODO mode 1- alarm, 2- auto light on off
	// TODO add temprature and light sensor in second row
	// TODO add mode in line 3
	// TODO if alarm mode, how many movements detected. if autolighting status of lamp.

	// TODO if between lcd command happening interrupt comes and in that interuppt we use another lcd command,
	//      bad things will happen.

	initialization();

	while (1) {
		// ------------------------- LED Blinking PORTC -------------------------
//		PORTC = 0xFF;
//		_delay_ms(50);
//
//		PORTC = 0x00;
//		_delay_ms(50);
//		// ------------------------- LED Blinking PORTC -------------------------
//
//
//		// ------------------------- PIR Sensor -------------------------
//		if ((PINB & (1 << PB0)) == (1 << PB0))
//			lcd_str_at(0, 4, "Yes");
//		else
//			lcd_str_at(0, 4, "No!");
		// ------------------------- PIR Sensor -------------------------
	}
	return 0;
}
