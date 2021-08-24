
#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 8000000UL			/* Define CPU Frequency  8MHz */
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#define LCD_Data_Dir DDRB		/* Define LCD data port direction */
#define LCD_Command_Dir DDRC		/* Define LCD command port direction register */
#define LCD_Data_Port PORTB		/* Define LCD data port */
#define LCD_Command_Port PORTC		/* Define LCD data port */
#define RS PC0				/* Define Register Select (data/command reg.)pin */
#define RW PC1				/* Define Read/Write signal pin */
#define EN PC2				/* Define Enable signal pin */

#include <avr/io.h>			/* Include AVR std. library file */
#include<avr/interrupt.h>//In eclipce we have to use this lib



void _delay_ms(unsigned long num){
	for (int i = 0 ; i< 63 * num ; i++);
	//The idea of this function is to delay according to the following eqn.
	// num of seconds in ml = Number of iterations / time of one itteration = 1ms /8 * 8000 ms = 63
	//where 8 is the number of lines in assembly and 8000ms is F_CPU

}
void LCD_Command(unsigned char cmnd)
{
	LCD_Data_Port= cmnd;
	LCD_Command_Port &= ~(1<<RS);	/* RS=0 command reg. */
	LCD_Command_Port &= ~(1<<RW);	/* RW=0 Write operation */
	LCD_Command_Port |= (1<<EN);	/* Enable pulse */
	_delay_ms(0.001);
	LCD_Command_Port &= ~(1<<EN);
	_delay_ms(3);
}

void LCD_Char (unsigned char char_data)	/* LCD data write function */
{
	LCD_Data_Port= char_data;
	LCD_Command_Port |= (1<<RS);	/* RS=1 Data reg. */
	LCD_Command_Port &= ~(1<<RW);	/* RW=0 write operation */
	LCD_Command_Port |= (1<<EN);	/* Enable Pulse */
	_delay_ms(0.001);
	LCD_Command_Port &= ~(1<<EN);
	_delay_ms(1);
}

void LCD_Init (void)			/* LCD Initialize function */
{
	LCD_Command_Dir = 0xFF;		/* Make LCD command port direction as o/p */
	LCD_Data_Dir = 0xFF;		/* Make LCD data port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */

	LCD_Command (0x38);		/* Initialization of 16X2 LCD in 8bit mode */
	LCD_Command (0x0C);		/* Display ON Cursor OFF */
	LCD_Command (0x06);		/* Auto Increment cursor */
	LCD_Command (0x01);		/* Clear display */
	LCD_Command (0x80);		/* Cursor at home position */
}

void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void LCD_String_xy (char row, char pos, char *str)/* Send string to LCD with xy position */
{
	if (row == 0 && pos<16)
	LCD_Command((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<16)
	LCD_Command((pos & 0x0F)|0xC0);	/* Command of first row and required position<16 */
	LCD_String(str);		/* Call LCD string function */
}

void LCD_Clear()
{
	LCD_Command (0x01);		/* clear display */
	LCD_Command (0x80);		/* cursor at home position */
}
void UART_Init( unsigned int ubrr){//Here we init. UART
	//Set baud rate
	UBRRH = (unsigned char)(ubrr >> 8);
	UBRRL = (unsigned char)ubrr ;
	//Enable receiver and transmitter
	UCSRB |= (1<<RXEN)|(1<<TXEN) | (1<<RXCIE);
	//SET FORMAT 8DATA 2 STOP BITS
	UCSRC =  (3<<UCSZ0);
	sei();
}
void UART_Transmit(unsigned char data){
	while (!(UCSRA & (1 << UDRE))); //Wait for empty transmit buffer
	UDR = data ;
}
unsigned char UART_Receive(){
	//Wait for data
	while(!(UCSRA & (1<<RXC)));
	//Return data
	return UDR ;
}
void main_state(){
	LCD_Clear();
	LCD_String_xy(0 , 0 ,"Choose a question");
	LCD_String_xy(1,0,"1-busy 2-hungry");
	_delay_ms(1);
}
void waiting_state(unsigned char butt){
	LCD_Clear();
	if (butt == '1') {
		LCD_String_xy(0 , 0 , "Asked if busy");

	}
	else if(butt == '2'){
		LCD_String_xy(0 , 0 ,"Asked if hungry");
	}
	LCD_String_xy(1,0,"Waiting for Ans");
	_delay_ms(1000);


}
void answer_state(unsigned char butt){
	LCD_Clear();
	if (butt == '1'){
		LCD_String_xy(0 , 0 , "Yes,I am");
		_delay_ms(10000);
		main_state();
		return ;

	}
	if (butt == '2'){
			LCD_String_xy(0 , 0 , "No, I'm not");
			_delay_ms(10000);
			main_state();
			return ;

		}

}
void INT0_Init(void)
{
	SREG  &= ~(1<<7);                   // Disable interrupts by clearing I-bit
	DDRD  &= (~(1<<PD2));               // Configure INT0/PD2 as input pin
	GICR  |= (1<<INT0);                 // Enable external interrupt pin INT0
	MCUCR |= (1<<ISC00) | (1<<ISC01);   // Trigger INT0 with the raising edge
	SREG  |= (1<<7);                    // Enable interrupts by setting I-bit
}
void INT1_Init(void)
{
	SREG  &= ~(1<<7);                   // Disable interrupts by clearing I-bit
	DDRD  &= (~(1<<PD3));               // Configure INT0/PD2 as input pin
	GICR  |= (1<<INT1);                 // Enable external interrupt pin INT0
	MCUCR |= (1<<ISC00) | (1<<ISC01);   // Trigger INT0 with the raising edge
	SREG  |= (1<<7);                    // Enable interrupts by setting I-bit
}
ISR(INT0_vect)
{
	waiting_state('1');
	UART_Transmit('1');//Transmit 1
	return ;
}
ISR(INT1_vect)
{
	waiting_state('2');
	UART_Transmit('2');//Transmit 1
	return ;
}


#endif /* MAIN_H_ */
