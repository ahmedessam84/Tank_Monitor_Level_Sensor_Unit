
#include <xc.h>

// initializes two pins for green and red leds
// green led indicated sensor connected to can bus
// red led indicate sensor is not connected to can bus



// in pic MCUs direction logic: 0 = output and 1 = input

void LedsInit(void)
{
    // set two pins to output
    TRISDbits.RD3 = 0;
    TRISDbits.RD2 = 0;
    
    // turn both leds off
    LATDbits.LD2 = 0;
    LATDbits.LD3 = 0;
}
