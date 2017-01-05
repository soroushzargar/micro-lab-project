#include <avr/io.h>
#include <util/delay.h>

int main() {
	DDRD = 0xff;

	while (1) {
		_delay_ms(25);
		PORTD = 0xff;

		_delay_ms(25);
		PORTD = 0x00;
	}
	return 0;
}
