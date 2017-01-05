#include <avr/io.h>
#include <util/delay.h>
#include "../Header/lcd.h"

int main() {
	unsigned char data[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}, pos0 = 1, pos1 = 0;


	lcd_init();

	DDRC = 0xff;

	while (1) {
		PORTC = 0xff;
		_delay_ms(200);

		PORTC = 0x00;
		_delay_ms(200);

		lcd_data(data[pos1]);
		lcd_data(data[pos0]);

		pos0++;
		pos1 += pos0 / 10;

		pos1 %= 10;
		pos0 %= 10;
	}
	return 0;
}
