/*
 * display.h
 *
 *  Created on: Oct 20, 2022
 *      Author: rok4550
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

void LCD_SetCursor(uint16_t x, uint16_t y);
void LCD_DrawPixel(uint16_t color);
void LCD_WriteLetter(uint16_t x, uint16_t y, uint16_t colorfg, uint16_t colorbg, char letter);
void LCD_WriteString(uint16_t x, uint16_t y, uint16_t colorfg, uint16_t colorbg, char* letStr);
void Bits2Pixel(uint16_t line, uint16_t colorfg, uint16_t colorbg);
void LCD_ClearDisplay(uint16_t color);
void LCD_Output16BitWord(uint16_t data);
void LCD_WriteReg(uint16_t command, uint16_t data);
void LCD_WriteData(uint16_t data);
void LCD_WriteCommand(uint16_t data);
void LCD_Init (void);


#endif /* INC_DISPLAY_H_ */
