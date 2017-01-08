#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "../Header/lcd.h"

// ------------------------- Global Variables Definitions -------------------------
char data[17];

uint8_t second, minute, hour;
uint8_t day;

uint16_t mov_det_count;

uint16_t light, temperature;
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
//	if ((PINB & (1 << PB2)) == (1 << PB2))
//		lcd_str_at(13, 3, "Yes");
//	else
//		lcd_str_at(13, 3, "No!");

	// Light Sensor
	sprintf(data, "Light: %9u", light);
	lcd_str_at(0, 2, data);

	// Temperature Sensor
	sprintf(data, "Temperature:%4u", temperature);
	lcd_str_at(0, 3, data);
}

void ext_int2_init() {
	// Interrupt on rising edge
	MCUCSR |= (1 << ISC2);

	GIMSK |= (1 << INT2); // Interrupt to activate INT2
//	GICR |= (1 << INT2); // Activate interrupt to INT2

	mov_det_count = 0;

	sprintf(data, "Num of Mov: %4u", mov_det_count);
	lcd_str_at(0, 4, data);
}

ISR(INT2_vect) {
	sprintf(data, "Num of Mov: %4u", ++mov_det_count);
	lcd_str_at(0, 4, data);
}

void adc_init() {
	ADMUX  |= (1 << REFS0) | (1 << REFS1);
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
}

void initialization() {
	DDRA = 0x00;
//	PORTA = 0xFF;

	DDRB = 0x00;  // Input for PIR sensor to INT2.
	PORTB = 0xFF;

	DDRC = 0xFF;
	PORTC = 0xFF;

	MCUCSR |= (1 << JTD);  // Disable J Tag

	lcd_init();

	timer1_init();

	ext_int2_init();

	adc_init();

	sei();  // Enable global interrupt.
}

int main() {
	// TODO Add temprature and light sensor in second row.
	// TODO Add RFID

	initialization();

	while (1) {
		// Reading light sensor
		ADMUX &= (~(1 << MUX0));
//		ADMUX |= (1 << REFS1);
		ADCSRA |= (1 << ADSC);
		_delay_ms(200);
		light = ADC;
		ADCSRA &= (~(1 << ADSC));

		_delay_ms(100);

		// Reading temperature sensor
		ADMUX |= (1 << MUX0);
//		ADMUX &= (~(1 << REFS1));
		ADCSRA |= (1 << ADSC);
		_delay_ms(200);
		temperature = ADC;
		ADCSRA &= (~(1 << ADSC));
	}

	return 0;
}
