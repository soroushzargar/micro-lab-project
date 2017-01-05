# Micro Lab Project

### Compiling and Programming AVR with USBASP board in Ubuntu

1. Install `gcc-avr avr-libc uisp avrdude` packages.
2. Write a simple code and name it `main.c` like:

    ```C
#include <avr/io.h>
#include <util/delay.h>
int main() {
    DDRD = 0xFF; // Output port.

    while (1) {
        PORTD = 0xFF; // Turn on LED.
        _delay_ms(500);

        PORTD = 0x00; // Turn of LED.
        _delay_ms(500);
    }

    return 0;
}
```

3. Run commands below in your terminal:
  1. `avr-gcc -w -Os -DF_CPU=1000000UL -mmcu=atmega32 -c -o main.o main.c`
  2. `avr-gcc -w -mmcu=atmega32 main.o -o main`
  3. `avr-objcopy -O ihex -R .eeprom main main.hex`
  4. `sudo avrdude -F -V -c usbasp -p m32 -P usb -U flash:w:main.hex`
