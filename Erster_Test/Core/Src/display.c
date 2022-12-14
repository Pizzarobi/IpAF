/*
 * display.c
 *
 *  Created on: Oct 20, 2022
 *      Author: rok4550
 */
#include "STM32F4xx.h"
//#include "main.h"
#include <inttypes.h>
#include <string.h>
#include "fonts.h"
#include "display.h"
#include "cmsis_os.h"
#include "usb_host.h"

void LCD_SetCursor(uint16_t x, uint16_t y){
	LCD_WriteReg(0x004E, x); // Set x-pos
	LCD_WriteReg(0x004F, y); // Set y-pos
}

void LCD_DrawPixel(uint16_t color){
	LCD_WriteReg(0x0022,color);
}

void LCD_WriteLetter(uint16_t x, uint16_t y, uint16_t colorfg, uint16_t colorbg, char letter){
	int offset = letter*16*2;
	int i = 0;
	uint16_t line = 0;
	LCD_SetCursor(x,y);
	//printf("Ascii Code for %c: %d , %d\n",letter,letter,offset);
	for(i=offset; i<offset+16*2; i++){
		LCD_SetCursor(x,y);
		line = console_font_12x16[i] << 4 | console_font_12x16[i+1] >> 4;
		Bits2Pixel(line, colorfg, colorbg);
		y++;
		i++;
	}
}

void LCD_WriteString(uint16_t x, uint16_t y, uint16_t colorfg, uint16_t colorbg, char* letStr){
	int n = 0;
	while(letStr[n] != 0){
		LCD_WriteLetter(x,y,colorfg,colorbg,letStr[n]);
		x+=12;
		n++;
	}
}

void Bits2Pixel(uint16_t line, uint16_t colorfg, uint16_t colorbg){
	int str[13];
	int n = 0;
	while(n<12){
		str[n] = 0;
		n++;
	}
	n = 11;
	while(line){
		if(line&1)
			str[n] = 1;
		else
			str[n] = 0;
		line >>=1;
		n--;
	}
	str[12]=0;

	n = 0;
	while(n<12){
		if(str[n]==0)
			LCD_DrawPixel(colorfg);
		else
			LCD_DrawPixel(colorbg);
		n++;
	}


//    if (line >> 1) {
//        Bits2Pixel(line >> 1);
//    }
//		if(line&1){
//			LCD_DrawPixel(0xFFFF);
//		}else {
//			LCD_DrawPixel(0xFFFF);
//		}
}

void LCD_ClearDisplay(uint16_t color){
	int i = 0;
	LCD_SetCursor(0,0);

	while(i<76800){
		LCD_DrawPixel(color);
		i++;
	}
}

void  LCD_Output16BitWord(uint16_t data)
{
	GPIOD->ODR |= (data << 14); // 1er setzen
    GPIOD->ODR &= (data << 14) | 0x3FFF; //0er setzen
    GPIOD->ODR |= ((data >> 5) & 0x0700);
    GPIOD->ODR &= ((data >> 5) & 0x0700) | ~(0x0700);
    GPIOD->ODR |= (data >> 2) & 0x3;
    GPIOD->ODR &= ((data >> 2) & 0x3) | ~(0x3);

    GPIOE->ODR |= (data << 3) & ~(0x007F);
    GPIOE->ODR &= ((data << 3) & ~(0x007F)) | 0x007F;
}

void LCD_WriteReg(uint16_t command, uint16_t data){
	LCD_WriteCommand(command);
	LCD_WriteData(data);
}

void LCD_WriteData(uint16_t data){
	GPIOD->ODR &= ~(0x80); // set PD7 ~CS to 0
	GPIOE->ODR |= 0x8; // set PE3 (D/~C) to 1 (Data)
	GPIOD->ODR &= ~(0x20); // Pushimpuls WR = 0
	LCD_Output16BitWord(data);
	GPIOD->ODR |= 0x20; // WR = 1
	GPIOD->ODR |= 0x80; // Chipselect = 1
}

void LCD_WriteCommand(uint16_t data){
	GPIOD->ODR &= ~(0x80); // set PD7 ~CS to 0
	GPIOE->ODR &= ~(0x8); // set PE3 (D/~C) to 0 (Command)
	GPIOD->ODR &= ~(0x20); // set WR PD5 = 0
	LCD_Output16BitWord(data);
	GPIOD->ODR |= 0x20; // set WR PD5 = 1
	GPIOD->ODR |= 0x80; // set ~CS PD7 = 1
}

void LCD_Init (void){
	// Display-Reset
	GPIOD->ODR &= ~(0x8);
	osDelay(pdMS_TO_TICKS(1));
	GPIOD->ODR |= 0x8;
	osDelay(pdMS_TO_TICKS(1));


	LCD_WriteReg(0x0010, 0x0001); /* Enter sleep mode */
	LCD_WriteReg(0x001E, 0x00B2); /* Set initial power parameters. */
	LCD_WriteReg(0x0028, 0x0006); /* Set initial power parameters. */
	LCD_WriteReg(0x0000, 0x0001); /* Start the oscillator.*/
	LCD_WriteReg(0x0001, 0x72EF); /* Set pixel format and basic display orientation */
	LCD_WriteReg(0x0002, 0x0600);
	LCD_WriteReg(0x0010, 0x0000); /* Exit sleep mode.*/
	//30ms warten weniger geht meist auch
	osDelay(pdMS_TO_TICKS(30));;

	LCD_WriteReg(0x0011, 0x6870); /* Configure pixel color format and MCU interface parameters.*/
	LCD_WriteReg(0x0012, 0x0999); /* Set analog parameters */
	LCD_WriteReg(0x0026, 0x3800);
	LCD_WriteReg(0x0007, 0x0033); /* Enable the display */
	LCD_WriteReg(0x000C, 0x0005); /* Set VCIX2 voltage to 6.1V.*/
	LCD_WriteReg(0x000D, 0x000A); /* Configure Vlcd63 and VCOMl */
	LCD_WriteReg(0x000E, 0x2E00);
	LCD_WriteReg(0x0044, (240-1) << 8); /* Set the display size and ensure that the GRAM window
	is set to allow access to the full display buffer.*/
	LCD_WriteReg(0x0045, 0x0000);
	LCD_WriteReg(0x0046, 320-1);
	LCD_WriteReg(0x004E, 0x0000); /*Set cursor to 0,0 */
	LCD_WriteReg(0x004F, 0x0000);


	//GPIOD->ODR |= 0x2000; // PD13 = 1 Backlight anschalten
}

