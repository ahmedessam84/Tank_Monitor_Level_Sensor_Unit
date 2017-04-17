/* 
 * File:   Leds.h
 * Author: Ahmed
 *
 * Created on March 4, 2017, 8:45 PM
 */

#ifndef LEDS_H
#define	LEDS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define RED_ON      LATDbits.LD2 = 1
#define RED_OFF     LATDbits.LD2 = 0

#define GREEN_ON    LATDbits.LD3 = 1
#define GREEN_OFF   LATDbits.LD3 = 0
    
    void LedsInit(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LEDS_H */

