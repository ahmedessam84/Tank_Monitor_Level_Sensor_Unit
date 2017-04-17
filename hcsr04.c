
#include <xc.h>
#include <stdint.h>
#include "gpio.h"
#include "timer1.h"

#define _XTAL_FREQ 32000000

void Hcsr04_Init(void)
{
    Timer1_Init();

    TRISBbits.RB4 = 1;         //RB4 as Input PIN (ECHO)
    TRISDbits.RD0 = 0;         //RD0 as Output Trigger
    LATDbits.LD0 = 0;
    
    //enable interrupts
    INTCONbits.RBIF = 0;       // clear flag
    INTCONbits.RBIE = 1;       // enable interrupt on change for port b

    INTCONbits.PEIE = 1;       // enable peripheral interrupt
    INTCONbits.GIE = 1;        // enable global interrupts
    
    
    
}

void Hcsr04_Trigger(void)
{
    LATDbits.LD0 = 1;
    __delay_us(10);
    LATDbits.LD0 = 0;
}


/*
void interrupt CaptureISR(void)
{

    
    // capture time when change on RB7 happened
    if( INTCONbits.RBIF == 1 )
    {    
        // if RB4 state is logic 1 then the interrupt was due to a rising edge
        if( PORTBbits.RB4 == 1 )
        {
            //reset and start timer, start of pulse
            TMR1 = 0;
            TMR1ON = 1;
        }
        else
        {
            //stop timer, end of pulse
            TMR1ON = 0;
        }
        
        //clear interrupt flag
        INTCONbits.RBIF = 0;
    }
    else{};

}
*/