
#include "gpio.h"
#include <xc.h>
#include <stdint.h>


#define SET_REG_DATA(REG,MASK,DATA)    REG = (REG & ~(MASK)) | (MASK & DATA)	//inserts data into selected register bits
#define GET_REG_DATA(REG,MASK)         REG & MASK								//Reads data from selected register pins


// for the following functions use port names PA, PB, PC, PD & PE

    void GPIOPinTypeGPIOInput(uint8_t Port,uint8_t Mask)
    {
        switch (Port)
        {
            case GPIO_PORTA: 
                SET_REG_DATA( TRISA, Mask, 0xff );
                break;    
            
            case GPIO_PORTB:
                SET_REG_DATA( TRISB, Mask, 0xff );
                break;
            
            case GPIO_PORTC:
                SET_REG_DATA( TRISC, Mask, 0xff );
                break;
                
            case GPIO_PORTD:
                SET_REG_DATA( TRISD, Mask, 0xff );
                break;
                
            case GPIO_PORTE:
                SET_REG_DATA( TRISE, Mask, 0xff );
                break;
        }
    }
    
    void GPIOPinTypeGPIOOutput( uint8_t Port, uint8_t Mask )
    {
        switch (Port)
        {
            case GPIO_PORTA: 
                SET_REG_DATA( TRISA, Mask, 0x00 );
                break;    
            
            case GPIO_PORTB:
                SET_REG_DATA( TRISB, Mask, 0x00 );
                break;
            
            case GPIO_PORTC:
                SET_REG_DATA( TRISC, Mask, 0x00 );
                break;
                
            case GPIO_PORTD:
                SET_REG_DATA( TRISD, Mask, 0x00 );
                break;
                
            case GPIO_PORTE:
                SET_REG_DATA( TRISE, Mask, 0x00 );
                break;
        }
    }
    
    void GPIOPinWrite( uint8_t Port, uint8_t Mask, uint8_t Data )
    {
        switch (Port)
        {
            case GPIO_PORTA: 
                SET_REG_DATA( LATA, Mask, Data );
                break;    
            
            case GPIO_PORTB:
                SET_REG_DATA( LATB, Mask, Data );
                break;
            
            case GPIO_PORTC:
                SET_REG_DATA( LATC, Mask, Data );
                break;
                
            case GPIO_PORTD:
                SET_REG_DATA( LATD, Mask, Data );
                break;
                
            case GPIO_PORTE:
                SET_REG_DATA( LATE, Mask, Data );
                break;
        }
    }
    
    uint8_t GPIOPinRead(uint8_t Port, uint8_t Mask)
    {
        uint8_t data;
        switch (Port)
        {
            case GPIO_PORTA: 
                data = GET_REG_DATA( PORTA, Mask );
                break;    
            
            case GPIO_PORTB:
                data = GET_REG_DATA( PORTB, Mask );
                break;
            
            case GPIO_PORTC:
                data = GET_REG_DATA( PORTC, Mask );
                break;
                
            case GPIO_PORTD:
                data = GET_REG_DATA( PORTD, Mask );
                break;
                
            case GPIO_PORTE:
                data = GET_REG_DATA( PORTE, Mask );
                break;
        }
        
        return data;
        
    }