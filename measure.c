#include <xc.h>
#include "hcsr04.h"
#include <stdint.h>

#define PRESCALER   8
#define _XTAL_FREQ  32000000

uint16_t clockcounts = 0;
const uint16_t speed = 17000;
uint16_t distance = 0;


/******************************measure****************************************/
// sensor triggered once and a reading is returned
/*****************************************************************************/

uint16_t measure(void)
{
    
    Hcsr04_Trigger();
    // timer1 TMR1 register has pulse width duration
    distance = (uint16_t)(((float) TMR1 * PRESCALER) * ((float)speed / (float)(_XTAL_FREQ/4)));
    
    return distance;
    
}


/********************************AvgMeasure************************************/
// the sensor is triggered 10 times and an average measurement is calculated
// to obtain a more accurate reading
/******************************************************************************/

uint16_t AvgMeasure(void)
{
    uint16_t average_dist = 0;
    uint8_t counter = 0;
    
    while(counter<10)
    {
        //TMR1 = 0;
        
        Hcsr04_Trigger();
        
        // wait a bit for the interrupt to capture the echo duration
        __delay_ms(20);
        
        GIE= 0 ; // enter critical section
        distance = TMR1; //(TMR1L | (TMR1H<<8)); //Reads Timer Value
        GIE = 1; // exit critical section
        
        distance = distance/58.82;              //Converts Time to Distance
        average_dist += distance; 
        counter++;
        
        //__delay_ms(10);
        
    }
    
    average_dist /= counter ;
    
    return average_dist;
    
}



/*
 
 40 ------------- 1 us
 
 17000cm--------- 1000000 us
        --------- 1000 us
 
 
 
 
 */