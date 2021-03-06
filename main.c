/*
 * main.c
 *
 *  Created on: Nov 13, 2013
 *  Author: brennon
 */

/* The clock speed is defined in the makefile */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define RELAY PB0
#define BUTTON PB1
#define POT PB2

// Some macros that make the code more readable
#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)


volatile uint8_t enableRelay = 0;
void showValueOnLED(uint16_t value);

/*
uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
*/

void initADC()
{
    /* this function initialises the ADC
     
     ADC Notes
     
     Prescaler
     
     ADC Prescaler needs to be set so that the ADC input frequency is between 50 - 200kHz.
     
     Example prescaler values for various frequencies
     
     Clock   Available prescaler values
     ---------------------------------------
     1 MHz   8 (125kHz), 16 (62.5kHz)
     4 MHz   32 (125kHz), 64 (62.5kHz)
     8 MHz   64 (125kHz), 128 (62.5kHz)
     16 MHz   128 (125kHz)
     
     below example set prescaler to 128 for mcu running at 8MHz
     
     
     */
    
    ADMUX =
		(1 << ADLAR) |						// left shift result
		(0 << REFS1) |						// Sets ref. voltage to VCC, bit 1
		(0 << REFS0) |						// Sets ref. voltage to VCC, bit 0
		(0 << MUX3)  |						// use ADC1 for input (PB2), MUX bit 3
		(0 << MUX2)  |						// use ADC1 for input (PB2), MUX bit 2
		(0 << MUX1)  |						// use ADC1 for input (PB2), MUX bit 1
		(1 << MUX0);						// use ADC1 for input (PB2), MUX bit 0
    
    ADCSRA =
		(1 << ADEN)  |						// Enable ADC
		(1 << ADPS2) |						// set prescaler to 128, bit 2
		(1 << ADPS1) |						// set prescaler to 128, bit 1
		(1 << ADPS0);						// set prescaler to 128, bit 0
}

int main(void) {

	initADC();
	
	set_output(DDRB, RELAY);
	set_input(DDRB, BUTTON);
	set_input(DDRB, POT);

	GIMSK = 0b01100000;						// Turn on pin change interrupts
	PCMSK = 0b00000010;						// Turn on interrupts on pin PB1
	MCUCR = 0b00000010;						// Enable interrupts only on a rising edge
	sei();									// Enable global interrupts

	while(1) {
		if(enableRelay)
		{
			ADCSRA |= (1 << ADSC);			// start ADC measurement
			while(ADCSRA & (1 << ADSC) );	// wait unil conversion completes

			uint8_t analogRead = ADCH;

			if(1)
			{
				// will give us a value between 0 and 100
				uint16_t delayTime = (((analogRead * 100) / 255));
				delayTime = (delayTime * 3) + 300;

				output_high(PORTB, RELAY);

                while(delayTime > 0)
				{
					_delay_ms(2000);
					delayTime--;
				}

				output_low(PORTB, RELAY);
			}
			else {
				showValueOnLED(analogRead);
			}
            
            enableRelay = 0;
		}
	}
}

void showValueOnLED(uint16_t value)
{
	if(value >= 100)
	{
		uint8_t hundredsValue = value / 100;

		for(; hundredsValue > 0; hundredsValue--)
		{
			output_high(PORTB, RELAY);
			_delay_ms(250);
			output_low(PORTB, RELAY);
			_delay_ms(250);
		}

		_delay_ms(1000);

		value = value % 100;
	}

	output_high(PORTB, RELAY);
	_delay_ms(1000);
	output_low(PORTB, RELAY);
	_delay_ms(1000);

	if(value >= 10)
	{
		uint8_t tensValue = value / 10;

		for(; tensValue > 0; tensValue--)
		{
			output_high(PORTB, RELAY);
			_delay_ms(250);
			output_low(PORTB, RELAY);
			_delay_ms(250);
		}

		_delay_ms(1000);

		value = value % 10;
	}

	output_high(PORTB, RELAY);
	_delay_ms(1000);
	output_low(PORTB, RELAY);
	_delay_ms(1000);

	if(value >= 1)
	{
		uint8_t onesValue = value;

		for(; onesValue > 0; onesValue--)
		{
			output_high(PORTB, RELAY);
			_delay_ms(250);
			output_low(PORTB, RELAY);
			_delay_ms(250);
		}

		_delay_ms(1000);
	}
}

ISR(PCINT0_vect)
{
	enableRelay = 1;						// enable the relay back in the main function
}
