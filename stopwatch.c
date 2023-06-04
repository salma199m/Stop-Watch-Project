#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

unsigned char flag1 = 0;
unsigned char arr[6] = { 0, 1, 2, 3, 4, 5 };       //6 seven-segment displays
unsigned char count[6] = { 0, 0, 0, 0, 0, 0 };    //counts of each seven-segment display

ISR(TIMER1_COMPA_vect)
{ //when compare match occurs the stop watch increments by 1 second
	flag1 = 1;
}

ISR(INT0_vect)   //reset the stop watch if push button(PD2) is pressed
{
	for(unsigned char i=0; i<6; i++)
	{
		count[i]=0;
	}
}

ISR(INT1_vect)
{
	TCCR1B &=~((1<<CS12)|(1<<CS10));    //no clock source
}

ISR(INT2_vect)            // INT Service Routine for INT2
{
	TCCR1B|= (1<<CS12)|(1<<CS10);
}


void INT2_Resumed (void) // External INT2 to Resume Stop watch
{
	DDRB &=~ (1<<PB2);         // Input Pin for INT1
	PORTB |=(1<<PB2);        // Enable Internal pull Up
	MCUCSR &=~ (1<<ISC2);   // with falling egd
	GICR |= (1<<INT2);     // Input Pin for INT1
}

void INT1_paused (void)
{
	DDRD &=~ (1<<PD3);                  // Input Pin for INT1
	PORTD &=~(1<<PD3);                 // Stop Internal pull Up
	MCUCR |= (1<<ISC11)|(1<<ISC10);   // Making INT1 With rising edge
	GICR  |= (1<<INT1);              // Enable INT1
}

void INT0_RESET(void){
	DDRD &=~ (1<<PD2);      // Input Pin for INT0
	PORTD |= (1<<PD2);     // ACTIVATE INTERNAL PULL UP
	MCUCR |=(1<<ISC01);   // Making INT0 With falling edge
	GICR  |= (1<<INT0);  // Enable INT0
}
void Timer1_CTC_Init(void)
{
	TCNT1=0;                               //start count from 0

	OCR1A = 1000;                          //f=1MHz , N=1024 >>>>>Ft=976=1000HZ

	TIMSK |= (1 << OCIE1A);                //Enable Timer1 Compare A Interrupt

	TCCR1A = (1 << FOC1A);                 // Using Compare Mode

	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);      //using compare mode prescaler 1024

}
int main(void)
{
	unsigned char i = 0;
	DDRC |=0X0F;
	PORTC &=0XF0;

	DDRA |=0X3F;
	PORTA |=0X3F;
	SREG |=(1<<7);
	INT0_RESET();
	INT1_paused();
	INT2_Resumed();
	Timer1_CTC_Init();



	while(1)
	{
		if (flag1 == 1) {
			flag1 = 0; //reset the flag so when the next interrupt occurs it becomes 1 again

			if(count[0]==9){
				count[0]=0;
				if(count[1]==5){
					count[1]=0;
					if(count[2]==9){
						count[2]=0;
						if(count[3]==5){
							count[3]=0;
							if(count[4]==9){
								count[4]=0;
								if(count[5]==5){
									count[5]=0;
								} else
									count[5]++;

							} else
								count[4]++;
						} else
							count[3]++;
					} else
						count[2]++;
				} else
					count[1]++;
			} else
				count[0]++;
		}
		while(1)
		{
			for(i = 0; i<6; i++)
			{
				/*at each interrupt loop on the 6 7-segments to display the counts with
							 delay 100us between them to appear as if all of them are enabled at the
							 same time */
				PORTA &= ~0X3F;
				PORTA |= (1 << arr[i]);
				PORTC = (PORTC & 0xC0) | (count[i] & 0x3F);
				_delay_us(100);
			}
			if (flag1 == 1) //at a new interrupt leave while(1) and increment the counter then enter while(1) again to display the new counts
				break;

		}


	}

}

