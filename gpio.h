/* 
 * File:   gpio.h
 * Author: Ahmed
 *
 * Created on February 12, 2017, 8:20 PM
 */

#ifndef GPIO_H
#define	GPIO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
#define GPIO_PORTA 0x00
#define GPIO_PORTB 0x01
#define GPIO_PORTC 0x02
#define GPIO_PORTD 0x03
#define GPIO_PORTE 0x04
   


    void GPIOPinTypeGPIOInput( uint8_t, uint8_t);
    void GPIOPinTypeGPIOOutput( uint8_t, uint8_t);
    void GPIOPinWrite(uint8_t, uint8_t, uint8_t);
    uint8_t GPIOPinRead(uint8_t, uint8_t);

    
#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

