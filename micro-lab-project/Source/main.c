#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "../Header/lcd.h"

// ------------------------- Global Variables Definitions -------------------------
unsigned char nums[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
char data[17];

uint8_t second, minute, hour;
uint16_t day;

unsigned char mode = 0x00;  // Alarm mode
//unsigned char mode = 0xFF;  // Auto lighting

uint8_t mov_det_count;
// ------------------------- Global Variables Definitions -------------------------


void lcd_str_at(uint8_t x, uint8_t y, const char* data) {
	lcd_setcursor(x, y);
	lcd_string(data);
}

void lcd_char_at(uint8_t x, uint8_t y, uint8_t data) {
	lcd_setcursor(x, y);
	lcd_data(data);
}

void timer1_init() {
    TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12);  // set up timer with prescaler = 1024
    TCNT1 = 0;
    OCR1A = 968;
    TIMSK |= (1 << OCIE1A);  // enable compare match interrupt

    second = minute = hour = day = 0;

    // Viewing on LCD.
	sprintf(data, "Time:%2u %2.2u:%2.2u:%2.2u", day, hour, minute, second);
	lcd_str_at(0, 1, data);
}

ISR(TIMER1_COMPA_vect) {
	// A second forward.
	second++;
	minute += second / 60;        second %= 60;
	hour += minute / 60;          minute %= 60;
	day += hour / 24;             hour %= 24;

	// Viewing on LCD.
	sprintf(data, "Time:%2u %2.2u:%2.2u:%2.2u", day, hour, minute, second);
	lcd_str_at(0, 1, data);

	// PIR Sensor Status
	if ((PINB & (1 << PB2)) == (1 << PB2))
		lcd_str_at(13, 3, "Yes");
	else
		lcd_str_at(13, 3, "No!");
}

void ext_int2_init() {
	// Interrupt on rising edge
	MCUCSR |= (1 << ISC2);

	GIMSK |= (1 << INT2); // Interrupt to activate INT2
	GICR |= (1 << INT2); // Activate interrupt to INT2

	mov_det_count = -1;

	sprintf(data, "Num of Mov: %4u", ++mov_det_count);
	lcd_str_at(0, 4, data);
}

ISR(INT2_vect) {
	_delay_ms(1);  // To catch interrupts that we don't want.

	sprintf(data, "Num of Mov: %4u", ++mov_det_count);
	lcd_str_at(0, 4, data);
}

void initialization() {
	DDRB = 0x00;  // Input for PIR sensor to INT2.
	PORTB = 0xFF;

	DDRC = 0xFF;
	PORTC = 0xFF;

	lcd_init();

	timer1_init();

	ext_int2_init();

	sei();  // Enable global interrupt.
}

int main() {
	// TODO mode 1- alarm, 2- auto light on off
	// TODO add temprature and light sensor in second row
	// TODO add mode in line 3
	// TODO if alarm mode, how many movements detected. if autolighting status of lamp.

	// TODO if between lcd command happening interrupt comes and in that interuppt we use another lcd command,
	//      bad things will happen.

	initialization();

	while (1);

	return 0;
}
