#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

uint16_t period = 0;
uint8_t lastPeriod = 0;
uint8_t rise = 0;

uint8_t pulse0 = 0;
int up0 = 0; //down
uint8_t pulse1 = 1;
int up1 = 0; //down
uint8_t pulse2 = 1;
int up2 = 0; //down


typedef struct
{
	uint16_t period0;
	uint16_t period1;
	uint16_t period2;
} PWMPeriods;

PWMPeriods pwms;

int main() {
	DDRA = 0xFF;
	//running
	PORTA |= _BV(PA0);

	TCCR1B |= _BV(CS11) | _BV(CS10); // Clock Select 16000000U/64 4us

	GICR |= _BV(INT0) | _BV(INT1) | _BV(INT2);
	MCUCR |= _BV(ISC00) | _BV(ISC01); // start on rising
	MCUCR |= _BV(ISC10) | _BV(ISC11); // start on rising
	MCUCSR |= _BV(ISC2); // start on rising
	sei();

	uint16_t low = 10000;
	uint16_t high = 0;
	uint16_t center = 0;

	// ~280 ~= 1.2-1.3
	// ~380 ~= 1.5-1.6
	// ~480 ~= 2.2
	while(1) {
		if(pulse0 == 1) {
			if(
				(pwms.period0 >= 380 && pwms.period0 <= 383) ||
				(pwms.period0 < 283) ||
				(pwms.period0 > 480)
			) {
				PORTA |= _BV(PA0);
			} else {
				PORTA &= ~_BV(PA0);
			};

			pulse0 = 0;
		}

		if(pulse1 == 1) {
			if(
				(pwms.period1 >= 377 && pwms.period1 <= 380) ||
				(pwms.period1 < 283) ||
				(pwms.period1 > 480)
			) {
				PORTA |= _BV(PA1);
			} else {
				PORTA &= ~_BV(PA1);
			};

			pulse1 = 0;
		}

		if(pulse2 == 1) {
			if(
				(pwms.period2 <= 283)
			) {
				PORTA &= ~_BV(PA2);
			} else {
				PORTA |= _BV(PA2);
			};

			pulse2 = 0;
		}
	};
}

ISR(INT0_vect) {
	if(up0 == 0) {
		pwms.period0 = TCNT1;
		MCUCR &= ~_BV(ISC00); // switch to falling edge
		up0 = 1;
	} else {
		pwms.period0 = TCNT1 - pwms.period0;

		MCUCR |= _BV(ISC00); // switch to rising
		up0 = 0;
		
		pulse0 = 1;
	}
}

ISR(INT1_vect) {
	if(up1 == 0) {
		pwms.period1 = TCNT1;
		MCUCR &= ~_BV(ISC10); // switch to falling edge
		up1 = 1;
	} else {
		pwms.period1 = TCNT1 - pwms.period1;

		MCUCR |= _BV(ISC10); // switch to rising
		up1 = 0;
		
		pulse1 = 1;
	}
}

ISR(INT2_vect) {
	if(up2 == 0) {
		pwms.period2 = TCNT1;

		GICR &= ~_BV(INT2); // disable before switching edge
		MCUCSR &= ~_BV(ISC2); // falling edge 
		GIFR |= _BV(INT2); // clear flag
		GICR |= _BV(INT2); // reenable
		
		up2 = 1;
	} else {
		pwms.period2 = TCNT1 - pwms.period2;
		GICR &= ~_BV(INT2); // disable before switching edge
		MCUCSR |= _BV(ISC2); // start on rising
		GIFR |= _BV(INT2); // clear flag
		GICR |= _BV(INT2); // reenable
		up2 = 0;
		
		pulse2 = 1;
	}
}
