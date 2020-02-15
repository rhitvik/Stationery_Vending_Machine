/*
 * coinAcceptor_innterruptProgram.cpp
 *
 * Created: 26-Jul-16 7:34:29 PM
 *  Author: RHITVIK
 */ 

// #include <avr/io.h>
// 
// int main(void)
// {
// 	GICR |= (1<<INT1);
// 	MCUCR |= (1<<ISC10) | (1<<ISC11);
// 	
//     while(1)
//     {
//         //TODO:: Please write your application code 
//     }
// }

#define F_CPU 1000000UL

#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

#define LightSwitch    5
#define ReadWrite      6
#define BiPolarMood    7

#define MrLCDsCrib            PORTB
#define DataDir_MrLCDsCrib	  DDRB

#define MrLCDsControl		  PORTA
#define DataDir_MrLCDsControl DDRA

void Check_IF_MrLCD_isBusy(void);
void Peek_A_Boo(void);
void Send_A_Command(char command);
void Send_A_Character(char character);
void Send_A_String(char *stringOfCharacters);
void Send_A_String_inst(char *stringOfCharacters);

void initialise_counter(void);
void stop_counter(void);
void disable_driver(void);
void accept_coin(void);
void return_coin(void);
void initiate_accept_process(void);
void initiate_return_process(void);
void initialise_PWM(void);
void initialise_PWM_coin(void);
void initialise_PWM_dispatch(void);
void sw_debounce_pressed(void);
void sw_debounce_released(void);
void stop_PWM_coin(void);
void stop_PWM_dispatch(void);
void dispatch_parameter(void);
void unable_dispatch_parameter(void);

void detect_coin(void);
void vend_STATIONERY(void);
void check_IF_successful(void);
void CHECK_STOCK(void); 

void resetParameters(void);

static volatile uint8_t count=0;  //for interrupt generation
static volatile uint8_t count1=0; //10 sec delay initializer
static volatile uint8_t FAIL = 0;

uint8_t p = 0;
uint8_t kick = 0;
uint8_t STOCK = 0;
   
uint8_t coin_10RS = 0;
uint8_t RS10_coin_no = 0;
uint8_t coin_count = 0;

uint8_t coin_05RS = 0;
uint8_t RS05_coin_no = 0;

int money = 0;

int pressed=0;
int pressed_1=0;
int pressed_2=0;

int metal_touch=0;
int dispatched=0;
int successful=0;
int aagaya=0;

int refresh = 0;
int refresh2 = 0;

int main(void)
{
	DDRA = 0b11111111;
	DDRC = 0xFF;
	DDRD = 0b11111100;
	DataDir_MrLCDsControl |= (1<<LightSwitch) | (1<<ReadWrite) | (1<<BiPolarMood);	
	
	PORTA |= (1<<PINA0) | (1<<PINA1) | (1<<PINA2) | (1<<PINA3); /*| (1<<PINA4); */
	PORTC = 0x00;
	PORTD = 0b00000011;
	
	DDRA |= (1<<PINA4);
	PORTA |=(1<<PINA4);

	//sei();	
		
	PORTC |= (1<<PINC0);
	initialise_PWM();
	
	_delay_ms(15);
	Send_A_Command(0x01);//clrscr();
	_delay_ms(2);
	Send_A_Command(0x38);
	_delay_us(50);
	Send_A_Command(0b00001100);
	_delay_us(50);
	
	PORTC = 0xFF;
	_delay_ms(500);
	PORTC = 0;
	_delay_ms(500);
	
	while (1)
	{			
		resetParameters();
		
		CHECK_STOCK();		
		
		Send_A_Command(0x01);//clrscr();
		_delay_ms(2);
		Send_A_String(" AUSTIN Pvt Ltd.");
		
		Send_A_Command(0xC0);//next line;
		_delay_ms(2);
		Send_A_String("Plz Insert Rs 10");	
				
		detect_coin();
		
		if (money==10)
		{
			//PORTC |= (1<<PINC3);
			if (STOCK == 1)
			{
				vend_STATIONERY();
				
				check_IF_successful();
				
				if (successful == 1)
				{
					OCR1B = 4500;
					_delay_ms(400);//ms 1000 was the prev val6
					stop_PWM_dispatch();
					unable_dispatch_parameter();
					
					initiate_accept_process();
				}
				else
				if(successful == 0)
				{
					stop_PWM_dispatch();
					unable_dispatch_parameter();
					
					Send_A_Command(0x01);//clrscr();
					_delay_ms(2);
					Send_A_String("Plz Collect Coin");
					
					Send_A_Command(0xC0);//next line;
					_delay_ms(2);
					Send_A_String("ERROR-TRY AGAIN");
					
					_delay_ms(1000);
					
					initiate_return_process();
				}
			}
			else
			if (STOCK == 0)
			{
				Send_A_Command(0x01);//clrscr();
				_delay_ms(2);
				Send_A_String("Plz Collect Coin");
				
				Send_A_Command(0xC0);//next line;
				_delay_ms(2);
				Send_A_String("  OUT OF STOCK  ");
				
				_delay_ms(1000);
				
				initiate_return_process();				
			}
		}
		else
		initiate_return_process();
	}			
}	

void disable_driver(void)
{
	PORTD &=~ (1<<PIND6);
	PORTD &=~ (1<<PIND7);
	PORTD &=~ (1<<PIND5);
}

void accept_coin(void)
{
	//clockwise
	PORTD |= (1<<PIND6);
	PORTD &=~ (1<<PIND7);
}
void return_coin(void)
{
	//anticlockwise
	PORTD |= (1<<PIND7);
	PORTD &=~ (1<<PIND6);
}

void initiate_return_process(void)
{
	stop_counter();
	_delay_ms(500);
	
	return_coin();
	_delay_ms(50);
	initialise_PWM_coin();
	_delay_ms(1000);////calibrate it further
	stop_PWM_coin();
	_delay_ms(50);
	disable_driver();
}
void initiate_accept_process(void)
{
	stop_counter();
	_delay_ms(500);
	
	accept_coin();
	_delay_ms(50);
	initialise_PWM_coin();
	_delay_ms(1000);////calibrate it further
	stop_PWM_coin();
	_delay_ms(50);
	disable_driver();
}

void initialise_PWM(void)
{
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<WGM12) |(1<<WGM13) |(1<<CS10);
	ICR1=19999; //top value
}
void initialise_PWM_coin(void)
{
	TCCR1A |= (1<<COM1A1);
	OCR1A = 5000; ///////put the ocr value
}
void initialise_PWM_dispatch(void)
{
	TCCR1A |= (1<<COM1B1);
	OCR1B = 3500; ///////put the ocr value 
}

void sw_debounce_pressed(void)
{
	//code
	unsigned int pressed_confidence_level=0;
	pressed_confidence_level++;
	if(pressed_confidence_level>=500 )//put a debouncing value///////////////
	{
		pressed_confidence_level=0;
	}
}
void sw_debounce_released(void)
{
	//code
	unsigned int released_confidence_level=0;
	released_confidence_level++;

	if(released_confidence_level>=500 )//put a debouncing value//////////////
	{
		released_confidence_level=0;
	}
}

void stop_PWM_coin(void)
{
	TCCR1A &=~ (1<<COM1A1);
	OCR1A = 0;
}
void stop_PWM_dispatch(void)
{
	TCCR1A &=~ (1<<COM1B1);
	OCR1B = 0;
}

void dispatch_parameter(void)
{
	_delay_ms(50);
	PORTD |= (1<<PIND2);
	PORTD &=~ (1<<PIND3);
	_delay_ms(50);
}
void unable_dispatch_parameter(void)
{
	_delay_ms(50);
	PORTD &=~ (1<<PIND2);
	PORTD &=~ (1<<PIND3);
	PORTD &=~ (1<<PIND4);
	_delay_ms(50);
}

void initialise_counter(void)
{
	TCCR0 |= (1<<CS01)|(1<<CS00);
	TIMSK |= (1<<TOIE0);
	TCNT0 = 0;
}
void stop_counter(void)
{
	TCCR0 &=~ (1<<CS00);
	TCCR0 &=~ (1<<CS01);
	TIMSK &=~ (1<<TOIE0);
	_delay_ms(1);
	TCNT0=0;
}

void detect_coin(void)
{
	while(2)
	{		
		if (bit_is_set(PINA,2))
		{
			if (pressed == 0)
			{
				pressed = 1;
				money = 10;
				PORTC |= (1<<PINC4);
				initialise_counter();
				
				if (STOCK == 1)
				{
					Send_A_Command(0x01);//clrscr();
					_delay_ms(2);
					Send_A_String("coin accepted");
					break;
				}
				else
				
				break;			
			}			
		}	
		
		else
		if(bit_is_clear(PINA,2))
		{	
			refresh++;
			if (refresh >= 10000)
			{
				refresh = 0;
				
				refresh2++;
				
				if (refresh2 >= 10000)
				{
					refresh2 = 0;
				
					Send_A_Command(0x01);//clrscr();
					_delay_ms(2);
					Send_A_String_inst(" AUSTIN Pvt Ltd.");
					
					Send_A_Command(0xC0);//next line;
					_delay_ms(2);
					Send_A_String_inst("Plz Insert Rs 10");
				}							
			}
			else		
			if (pressed==1) pressed=0;		
		}
	}
}
void vend_STATIONERY(void)
{
	while(3)
	{
		Send_A_Command(0x01);//clrscr();
		_delay_ms(2);
		Send_A_String("here we go");
		
		_delay_ms(1000);
				
		if (bit_is_set(PINA,0))
		{
			Send_A_Command(0x01);//clrscr();
			_delay_ms(2);
			Send_A_String("pick up first");
		}
		else
		if (bit_is_clear(PINA,0))
		{
			sw_debounce_released();
			//dispatched=1;
			dispatch_parameter();
			initialise_PWM_dispatch();
			
			Send_A_Command(0x01);//clrscr();
			_delay_ms(2);
			Send_A_String("please wait");
			
			PORTC |= (1<<PINC5);
			break;
		}
		
		
		//////ask to pick the package first
	}
}
void check_IF_successful(void)
{
	while(4)
	{
		if (FAIL == 1)
		{
			successful=0;
			break;
		}
		else
		if (bit_is_set(PINA,0))
		{
			dispatched = 1;
			successful = 1;
			
			Send_A_Command(0x01);//clrscr();
			_delay_ms(2);
			Send_A_String("please collect!!");
			
			break;
		} 
// 		else
// 		if (bit_is_clear(PINA,0))
// 		{
// 			if(successful == 1) successful = 0;
// 		}		
	}		
}
void CHECK_STOCK(void)
{
	Send_A_Command(0x01);//clrscr();
	_delay_ms(2);
	Send_A_String("checking stock");
	
	while(5)
	{
		if(bit_is_set(PINA,3)) //empty
		{
			STOCK = 1;
			
			Send_A_Command(0x01);//clrscr();
			_delay_ms(2);
			Send_A_String("stock present");
			_delay_ms(100);
			
			break;
		}
		else
		if(bit_is_clear(PINA,3)) //available
		{
			STOCK = 0;
			
			Send_A_Command(0x01);//clrscr();
			_delay_ms(2);
			Send_A_String("out of stock");
			_delay_ms(1000);
			
			PORTC |= (1<<PINC7);
			break;
		}
	}

}

void resetParameters(void)
{
	count=0;  //for interrupt generation
	count1=0; //10 sec delay initializer
	FAIL = 0;

	p = 0;
	kick = 0;
	STOCK = 0;
	
	coin_10RS = 0;
	RS10_coin_no = 0;
	coin_count = 0;

	coin_05RS = 0;
	RS05_coin_no = 0;

	money = 0;

	pressed=0;
	pressed_1=0;
	pressed_2=0;

	metal_touch=0;
	dispatched=0;
	successful=0;
	aagaya=0;
	
	refresh = 0;
	refresh2 = 0;
	
	stop_counter();
	PORTC = 0;
	
	Send_A_Command(0x01);//clrscr();
	_delay_ms(2);
	Send_A_String("parameters Reset");
	_delay_ms(500);
}

ISR(TIMER0_OVF_vect)
{		
	count++;
	if (count==61)
	{
		// 1 second has elapsed
		count=0;
				
		count1++;
		
		PORTC ^= (1<<PINC3);
		
		if (count1>=20)/////// calibrate it further
		{			
			//return coin if not dispatched  
			count1=0;
			FAIL=1;
		}		
	}
}

void Check_IF_MrLCD_isBusy(void)
{
	DataDir_MrLCDsCrib=0;
	MrLCDsControl |= 1<<ReadWrite;
	MrLCDsControl &=~ 1<<BiPolarMood;
	
	while(MrLCDsCrib >= 0x80)
	{
		Peek_A_Boo();
	}
	
	DataDir_MrLCDsCrib=0xFF;
}
void Peek_A_Boo(void)
{
	MrLCDsControl |= 1<<LightSwitch;
	_delay_us(10);/////CALIBRATE IT FURTHER
	MrLCDsControl &=~  1<<LightSwitch;
}
void Send_A_Command(char command)
{
	Check_IF_MrLCD_isBusy();
	MrLCDsCrib = command;
	MrLCDsControl &=~ (1<<ReadWrite|1<<BiPolarMood);
	Peek_A_Boo();
	MrLCDsCrib = 0;	
}
void Send_A_Character(char character)
{
	Check_IF_MrLCD_isBusy();
	MrLCDsCrib = character;
	MrLCDsControl &=~ (1<<ReadWrite);
	MrLCDsControl |= (1<<BiPolarMood);
	Peek_A_Boo();
	MrLCDsCrib = 0;
}
void Send_A_String(char *stringOfCharacters)
{
	while(*stringOfCharacters > 0)
	{
		Send_A_Character(*stringOfCharacters++);
		//_delay_ms(10);
	}
}
void Send_A_String_inst(char *stringOfCharacters)
{
	while(*stringOfCharacters > 0)
	{
		Send_A_Character(*stringOfCharacters++);
	}
}

