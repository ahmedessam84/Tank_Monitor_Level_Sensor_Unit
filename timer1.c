
#include <xc.h>
#include <stdint.h>
#include "gpio.h"



void Timer1_Init(void)
{
    T1RD16 = 1;     // 16 BIT TIMER OPERATION, READ TMR1L TO GET THE 16 BIT VALUE
    TMR1CS = 0;      // INTERNAL CLOCK CHOSEN FOSC/4 = 10MHZ
    T1CKPS1 = 1;      // PRESCALAR SET TO 1:8
    T1CKPS0 = 1;
}
