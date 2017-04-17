/* 
 * File:   lcd.h
 * Author: Ahmed
 *
 * Created on February 12, 2017, 10:52 PM
 */

#include "stdint.h"

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

#define F										0<<2		//5x8 dots display
#define N										1<<3		// 2 lines display
#define RS                                      1<<2
#define EN                                      1<<3
#define LCD_DATA_PINS                           ((1<<4) | (1<<5) | (1<<6) | (1<<7))
#define DCB                                     0x04		// display on ,cursor on, blinking on
#define ID                                      0x02		// increment right
#define S										0x00		// shift off

    void LCD_Init(void);
    void LCD_Clear(void);
    void LCD_GoToXy( uint8_t, uint8_t );
    void LCD_DispChar(char);
    void LCD_DispString(char *);

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

