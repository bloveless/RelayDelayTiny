# Name: Makefile
# Author: <insert your name here>
# Copyright: <insert your copyright message here>
# License: <insert your license reference here>

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected. We recommend that you leave it undefined and
#                add settings like this to your ~/.avrduderc file:
#                   default_programmer = "stk500v2"
#                   default_serial = "avrdoper"
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

DEVICE     = attiny85
CLOCK      = 8000000
PROGRAMMER = -c avrisp2
OBJECTS    = main.o
FUSES      = -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0xFF:m
INCLUDE    = -I /usr/local/CrossPack-AVR/avr/include
DEFINES    = -D__AVR_ATtiny85__

PATH = /usr/local/CrossPack-AVR/bin
AVRDUDE = $(PATH)/avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = $(PATH)/avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all: clean upload

build: 
	$(COMPILE) -o main.elf main.c

clean:
	rm -f main.hex main.elf $(OBJECTS)

link: build
	$(PATH)/avr-objcopy -j .text -j .data -O ihex main.elf main.hex

upload: link size
	$(AVRDUDE) -e -U flash:w:main.hex $(FUSES)

fuses: 
	$(AVRDUDE) $(FUSES)

check:
	$(AVRDUDE)
    
size:
	$(PATH)/avr-size --format=avr --mcu=$(DEVICE) main.elf
