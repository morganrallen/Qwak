#include <avr/interrupt.h>
#include <avr/delay.h>

uint8_t pulse = 0;
uint8_t period = 0;
uint8_t lastPeriod = 0;
int up = 0; //down
uint8_t rise = 0;

int main() {
	DDRA = 0xFF;
	//running
	PORTA |= _BV(PA0);

	TCCR1B |= _BV(CS11) | _BV(CS10); // Clock Select 16000000U/64 4us

	GICR |= _BV(INT0);
	MCUCR |= _BV(ISC00) | _BV(ISC01); // start on rising
	sei();

	uint16_t low = 10000;
	uint16_t high = 0;
	uint16_t center = 0;

	while(1) {
		if(pulse == 1) {
			if(period > high)
				high = period;
			if(period < low)
				low = period;
			if(center == 0)
				center = period;

			if((period > low && period < high) && period != center)
				PORTA |= _BV(PA1);
			else
				PORTA &= ~_BV(PA1);

			pulse = 1;
		}
	};
}

ISR(INT0_vect) {
	if(up == 0) {
		rise = TCNT1;
		MCUCR &= ~_BV(ISC00); // switch to falling edge
		up = 1;
	} else {
		period = TCNT1 - rise;

		MCUCR |= _BV(ISC00); // start on rising
		up = 0;
		
		// up period is over
		pulse = 1;
	}
}
