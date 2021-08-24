#include"main.h"


int main(){
	INT0_Init();              // Enable external INT0
	INT1_Init();              // Enable external INT1
	LCD_Init();			/* Initialize LCD */
	UART_Init(MYUBRR);
	DDRA &= ~(1<<PA0);
	PORTA =  (1<<PA0);
	DDRA &= ~(1<<PA1);
	PORTA =  (1<<PA1);
	main_state();

	while(1){
		unsigned char data = UART_Receive();
		if(data) answer_state(data);

	}

	return 0;
}

