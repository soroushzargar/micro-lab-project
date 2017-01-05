#! /bin/bash

cd micro-lab-project/Source

avr-gcc -w -Os -DF_CPU=1000000UL -mmcu=atmega32 -c -o main.o main.c
avr-gcc -w -mmcu=atmega32 main.o -o main
avr-objcopy -O ihex -R .eeprom main main.hex
sudo avrdude -F -V -c usbasp -p m32 -P usb -U flash:w:main.hex
