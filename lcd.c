#include <xc.h>
#include "lcd.h"
#include "gpio.h"

#define _XTAL_FREQ 32000000

// LCD connected to PC3-PC6 for DB4-DB7 respectively
// LCD RS, R/W connected to PC0, PC1 &PC2 respectively
// EN pin is PA5


//choose between the two types of displays available:
#define LCD_TYPE_TOPWAY 
//#define LCD_TYPE_HD44780 

static void SendCommand( uint8_t command_id )
{
	// send highier nibble
	GPIOPinWrite( GPIO_PORTC, RS, 0x00 );
	GPIOPinWrite( GPIO_PORTC, EN, 0xff );
	GPIOPinWrite( GPIO_PORTC, LCD_DATA_PINS, command_id );
	__delay_us(2);
	GPIOPinWrite( GPIO_PORTC, EN, 0);
	__delay_us(2);
	
	// send lower nibble
	GPIOPinWrite( GPIO_PORTC, RS, 0x00 );
	GPIOPinWrite( GPIO_PORTC, EN, 0xff );
	GPIOPinWrite( GPIO_PORTC, LCD_DATA_PINS, (command_id<<4) );	
	__delay_us(2);
	GPIOPinWrite( GPIO_PORTC, EN, 0);
	
	__delay_us(100);
}

static void SendData( uint8_t data)
{
		// send highier nibble
	GPIOPinWrite( GPIO_PORTC, RS, 0xff );
	GPIOPinWrite( GPIO_PORTC, EN, 0xff );
	GPIOPinWrite( GPIO_PORTC, LCD_DATA_PINS, data );
	__delay_us(2);
	GPIOPinWrite( GPIO_PORTC, EN, 0);
	__delay_us(2);
	
	// send lower nibble
	GPIOPinWrite( GPIO_PORTC, RS, 0xff );
	GPIOPinWrite( GPIO_PORTC, EN, 0xff );
	GPIOPinWrite( GPIO_PORTC, LCD_DATA_PINS, (data<<4) );	
	__delay_us(2);
	GPIOPinWrite( GPIO_PORTC, EN, 0);
	
	__delay_us(100);
}

void LCD_Init(void)
{
    GPIOPinTypeGPIOOutput( GPIO_PORTC, 0xfc );
    GPIOPinWrite( GPIO_PORTC, 0xfc, 0x00 );
    
    __delay_ms(100);
    
#ifdef LCD_TYPE_HD44780
	//function set
	SendCommand(0x33);

	//function set
	SendCommand( 0x32 );
	
	//function set
	SendCommand( 0x28 );

	// Display, Cursor, Blinking
	SendCommand( 0x0c );

#else if LCD_TYPE_TOPWAY

    //function set
	SendCommand(0x03);

	//function set
	SendCommand( 0x20 | N | F );
	
	//function set
	SendCommand( 0x20 | N | F );

	// Display, Cursor, Blinking
	SendCommand( 1<<3 | DCB );
    
#endif
	//clear display
	SendCommand( 0x01 );
    
	__delay_ms(100);

	//entry mode
	SendCommand( 1<<2 | ID | S );
	
	LCD_GoToXy(0,0);
    
    
}

void LCD_Clear(void)
{
	SendCommand( 0x01 );
}	

void LCD_GoToXy(uint8_t x, uint8_t y)
{		
	//move cursor according to x & y
	// if line one is chosen addresses span from 00h to 0Fh
	if(y==0)
	{
		SendCommand( 0x80 | x );
	}
	// if line two is chosen addresses span from 40h to 4Fh
	else if (y==1)
	{
		SendCommand( 0x80 | 0x40 | x );
	}
	else {};
	
}	

void LCD_DispChar(char letter)
{
	SendData( letter );
}

void LCD_DispString(char *StrPtr)
{
	while( *StrPtr != '\0' )
	{
			SendData( *StrPtr );
			StrPtr ++;
	}			
}
