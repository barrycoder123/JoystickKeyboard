#include "stm32l476xx.h"
#include "LCD.h"
#include <stdlib.h>

void Joystick_Initialization(void) {
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	//PAO center
	//PA1 left
	//PA2 right
	//PA3 up
	//PA5 down 
	
	// Configure Port A Pins 0,1,2,3,5 as Input Mode
	GPIOA->MODER &= ~(0xC03FFCFF);
	GPIOA->MODER |= 0x802AA000;
	
	// Set PUPDR to pull-down fixed our issue!
	GPIOA->PUPDR &= 0x00000CFF;
	GPIOA->PUPDR |= 0x000008AA;
 
}

void delay(void) {

	uint8_t i = 0;

	for (i = 0; i < 50000; i++);
}

int Increase_Letter(uint8_t letter) {
	
	letter++;
	
	if (letter == 27){
		letter = 0;
	}
	return letter;
}

int Decrease_Letter(int8_t letter) {
	
	letter--;
	
	if (letter < 0){
		letter = 26;
	}
	return letter;
}

int main(void) {

	//pointers
	uint8_t *curr_letter = malloc((100)* sizeof(uint8_t));
	uint8_t *message = curr_letter;
	uint8_t *entire_message = curr_letter;

	//declare array that holds possible characters	
	uint8_t characters[27] = {' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

	uint8_t letter = 0;
	int position = 0;
		
	uint8_t select;
	uint8_t left;
	uint8_t right;
	uint8_t up;
	uint8_t down;	
		
	LCD_Initialization();
	LCD_Display_Name();
	
	//initialize Joystick
	Joystick_Initialization();

	while(1){
			
		select = (GPIOA->IDR & 0x01);
		left = (GPIOA->IDR & 0x02);
		right = (GPIOA->IDR & 0x04);
		up = (GPIOA->IDR & 0x08);
		down = (GPIOA->IDR & 0x20);	

		if (select == 0x01) {
			
			while(1) {
				LCD_MovingString(entire_message,position+1);		
//				select = (GPIOA->IDR & 0x01);
//				if (select == 0x01) {
//					while (select == 0x01) {
//						//display the curr_letter and exit
//						select = (GPIOA->IDR & 0x01);
//					}
//					break;
//				}
			}
		}
		else if (left == 0x02) {
			
			//keep track of position on the screen
			position--;
			
			if (position >= 5) {
				message--;
			} 
			
			//delete a character
			*curr_letter = '\0';
			curr_letter--;
			
			if (position < 0) {
				
				position = 0;
				message = entire_message;
				curr_letter = entire_message;
			}
			
			//reads button once and waits
			while(left == 0x02) {
				left = (GPIOA->IDR & 0x02);
			}
		}
		else if (right == 0x04) {
			
			//go to the next letter
			curr_letter++;
			letter = 0; //start at A
			
			//keep track of position on the screen
			position++;
			
			if (position > 5) {
				message++;
			}
			
			//reads button once and waits
			while(right == 0x04) {
				right = (GPIOA->IDR & 0x04);
			}
			
		}
		else if (up == 0x08) {
			
			// increase the letter (handles wrap-around)
			letter = Increase_Letter(letter);

			// print curr_letter
			*curr_letter = characters[letter];
			LCD_DisplayString(message);
			
			// reads button once and waits
			while(up == 0x08) {
				up = (GPIOA->IDR & 0x08);
			}
		}
		else if (down == 0x20) {
			
			// decrease the letter (handles wrap-around)
			letter = Decrease_Letter(letter);
			
			// print curr_letter
			*curr_letter = characters[letter];
			LCD_DisplayString(message);
			
			// reads button once and waits
			while(down == 0x20) {
				down = (GPIOA->IDR & 0x20);
			}
		}
		else {

			LCD_DisplayString(message);
		}
	}
}