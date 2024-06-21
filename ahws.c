#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define FALSE	0
#define TRUE	1

#define LED_ON		PORTB |= 1
#define LED_OFF		PORTB &= 0xFE

#define PUMP_ON		PORTB |= 2
#define PUMP_OFF	PORTB &= 0xFD

#define CUP_IS_OFF	(PINB & 0x4)

#define TOT_SEC		46800 // 13 Hours
#define TOT_SHOTS	25 // 2000 ml / 80 ml = 25 shots - Interval 1872 seconds 31.2 minutes

// Without volatile mod all this variables will be optimised away and program won't work!
volatile uint16_t tot_sec = TOT_SEC;
volatile uint16_t interval;

void delay_ms(uint16_t ms)
{
	while(--ms)
	{
		_delay_ms(1);
	}
	_delay_ms(1);
}

void delay_s(uint8_t s)
{
	while(--s)
	{
		delay_ms(1000);
	}
	delay_ms(1000);
}

void blink_led(uint8_t num)
{
	while(num--)
	{
		PORTB ^= 1;
		_delay_ms(400);
	}
}

ISR(PCINT0_vect)
{
	
}

ISR(TIMER1_COMPA_vect)
{
	if(tot_sec > 0)
	{
		--tot_sec;
	}
	if(interval > 0)
	{
		--interval;
	}
}

int16_t main() // 80 ml ---> 75 seconds
{
	DDRB |= 3;		// Set Data Direction Register of Port B's pin 0 and pin 1 as output
	
	DDRB &= 0xFB;	// Set Data Direction Register of Port B's pin 2 as input
	PORTB |= 4;		// Activate Port B's pin 2 pull-up resistor
	//PCMSK0 |= 4;	// Enable Pin Change Interrupt on Port B's pin 2 later
	PCICR |= 1;		// Enable Pin Change Interrupt Control Register 0
	sei();			// Enable Global Interrupt
	
	// Set 16 bit Timer 1 Counter Control Register B Waveform Generation Mode Bit to enable
	// CTC (Clear Timer on Compare match with register OCR1A)
	// Also set Clock Select bit to enable clk/256 prescaler
	TCCR1B = (1 << WGM12) | (1 << CS12);
	OCR1A = 62500; // Set Timer 1 Output Compare Register A to 1 second of ticks with 256 prescaler 16M/256
	TIFR1 = (1 << TOV1) | (1 << OCF1A) | (1 << OCF1B); // Clear Timer/Counter 1 Interrupt Flag Register interrupts
	TIMSK1 = 1 << OCIE1A; // Enable Timer 1 Output Compare A Enable bit in Timer 1 Interrupt Mask Register
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	
	uint8_t shots_need = TOT_SHOTS;
	uint8_t cup_full = FALSE;
	
	while(1)
	{
		if(tot_sec == 0)
		{
			if(cup_full && CUP_IS_OFF)
			{
				tot_sec = TOT_SEC;
				shots_need = TOT_SHOTS;
				interval = 0;
				cup_full = FALSE;
				blink_led(7);
				
				PCMSK0 |= 4; // Enable pressure interrupt, disable timer interrupt
				TIMSK1 &= ~(1 << OCIE1A);
				
				sleep_cpu();
				
				PCMSK0 &= ~4; // Enable timer interrupt, disable pressure interrupt
				TIMSK1 = 1 << OCIE1A;
				continue;
			}
			else
			{
				LED_ON;
				goto fill_in;
			}
		}
		
		if(interval == 0)
		{
			LED_ON;
		}
		
		if(cup_full && CUP_IS_OFF)
		{
			interval = tot_sec/--shots_need;
			cup_full = FALSE;
			LED_OFF;
			delay_ms(30); // Debounce pressure sensor in order to not pour water with no cup
			continue;
		}
		
		fill_in:
		if(!cup_full && !CUP_IS_OFF)
		{
			delay_ms(5000);
			if(!CUP_IS_OFF)
			{
				PUMP_ON;
				delay_s(67);
				delay_ms(500);
				PUMP_OFF;
				cup_full = TRUE;
			}
		}
	}
	
	return 0;
}
