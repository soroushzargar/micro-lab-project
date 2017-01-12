#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "../Header/lcd.h"
#include "../Header/utils.h"
#include "../Header/spi.h"
#include "../Header/mfrc522.h"


// ------------------------- Global Variables Definitions -------------------------
char data[17];

uint8_t second, minute, hour;
uint8_t day;

uint16_t mov_det_count;

uint8_t light, temperature;

uint8_t SelfTestBuffer[64];
uint8_t my_isfahan_cart[5] = {0x84, 0x51, 0x7d, 0xd9, 0x71}, bytes_to_comp = 5, match;
uint8_t locked = 0x00;
// ------------------------- Global Variables Definitions -------------------------


// ------------------------- Working With LCD -------------------------
void lcd_str_at(uint8_t x, uint8_t y, const char* data) {
	lcd_setcursor(x, y);
	lcd_string(data);
}

void lcd_char_at(uint8_t x, uint8_t y, uint8_t data) {
	lcd_setcursor(x, y);
	lcd_data(data);
}
// ------------------------- Working With LCD -------------------------


// ------------------------- Timer 1 -------------------------
void timer1_view() {
	// Viewing on LCD.
	sprintf(data, "Time:%2u %02u:%02u:%02u", day, hour, minute, second);
	lcd_str_at(0, 1, data);

	// PIR Sensor Status
	//	if ((PINB & (1 << PB2)) == (1 << PB2))
	//		lcd_str_at(13, 3, "Yes");
	//	else
	//		lcd_str_at(13, 3, "No!");

	// Light & Temperature Sensor
	sprintf(data, "L:%02u,T:%02u,", light, temperature);
	lcd_str_at(0, 2, data);

	// Lock Status
	if (locked) lcd_str_at(0, 3, "**** Locked ****");
	else lcd_str_at(0, 3, "*** Unlocked ***");
}

void timer1_init() {
    TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12);  // set up timer with prescaler = 1024
    TCNT1 = 0;
    OCR1A = 968;
    TIMSK |= (1 << OCIE1A);  // enable compare match interrupt

    second = minute = hour = day = 0;

    timer1_view();
}

ISR(TIMER1_COMPA_vect) {
	// A second forward.
	second++;
	minute += second / 60;        second %= 60;
	hour += minute / 60;          minute %= 60;
	day += hour / 24;             hour %= 24;

	timer1_view();
}
// ------------------------- Timer 1 -------------------------


// ------------------------- PIR Sensor -------------------------
void pir_view() {
	sprintf(data, "#M:%03u", ++mov_det_count);
	lcd_str_at(10, 2, data);
}

void ext_int2_init() {
	// Interrupt on rising edge
	MCUCSR |= (1 << ISC2);
	GIMSK |= (1 << INT2); // Interrupt to activate INT2
	mov_det_count = -1;

	pir_view();
}

ISR(INT2_vect) {
	pir_view();
}

void adc_init() {
	ADMUX  |= (1 << REFS0) | (1 << REFS1);
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
}
// ------------------------- PIR Sensor -------------------------

void initialization() {
	lcd_init();

	timer1_init();

	ext_int2_init();

	adc_init();

	spi_init();

	mfrc522_init();

	// Ports configuration
	DDRA = 0x00;
	DDRB  &= (~(1 << PB2));  // Input for PIR sensor to INT2.

	sei();  // Enable global interrupt.
}

int main() {
	// TODO Change INT2 to another INTx because of PORTB SPI->RFID mission.
	// TODO Write a function to show all data on lcd with timer0 triggering it.
	// TODO Write a function to show row 4 of lcd information with very long sring. Animating string that move to left.

	initialization();

	uint8_t byte;
	uint8_t str[MAX_LEN];

	byte = mfrc522_read(ComIEnReg);
	mfrc522_write(ComIEnReg,byte|0x20);
	byte = mfrc522_read(DivIEnReg);
	mfrc522_write(DivIEnReg,byte|0x80);

	while (1) {
		byte = mfrc522_request(PICC_REQALL,str);

		if(byte == CARD_FOUND)
		{
			byte = mfrc522_get_card_serial(str);
			if(byte == CARD_FOUND) {
				match = 0xff;
				for (byte = 0; byte < bytes_to_comp; ++byte)
					if (str[byte] != my_isfahan_cart[byte]) {
						match = 0x00;
						break;
					}
				if (match) locked = ~locked;
			}
		}

		// Reading light sensor
		ADMUX &= (~(1 << MUX0));
		ADCSRA |= (1 << ADSC);
		_delay_ms(80);
		light = ADC >> 4;
		ADCSRA &= (~(1 << ADSC));

		_delay_ms(20);

		// Reading temperature sensor
		ADMUX |= (1 << MUX0);
		ADCSRA |= (1 << ADSC);
		_delay_ms(80);
		temperature = ADC >> 4;
		ADCSRA &= (~(1 << ADSC));
	}

	return 0;
}
