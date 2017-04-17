/******************************************************************/
/*clock to be used to supply the tick time to the OS using an ISR.
 MCU used : PIC18F458
 TIMER0 IN 8 BIT MODE AND PRESCALER 1:32
 FOSC/4= 32/4 = 8 MHz
 */
/******************************************************************/

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ                      32000000
#define configTICK_RATE_HZ				( ( unsigned int ) 1000 )
#define configCPU_CLOCK_HZ				( ( unsigned long ) _XTAL_FREQ )
#define portTIMER_FOSC_SCALE            ( ( uint32_t) 4 )


void Clock_Init( void )
{
    
    const uint32_t ulConstCompareValue = ( ( configCPU_CLOCK_HZ / portTIMER_FOSC_SCALE ) / configTICK_RATE_HZ );
    uint32_t ulCompareValue;
    
	/* Interrupts are disabled when this function is called.

	Setup CCP1 to provide the tick interrupt using a compare match on timer
	3.
     * 
	Clear the time count then setup timer. */
	TMR3H = ( uint8_t ) 0x00;
	TMR3L = ( uint8_t ) 0x00;

	/* Set the compare match value. */
	ulCompareValue = ulConstCompareValue;
	CCPR1L = ( uint8_t ) ( ulCompareValue & ( uint32_t ) 0xff );
	ulCompareValue >>= ( uint32_t ) 8;
	CCPR1H = ( uint8_t ) ( ulCompareValue & ( uint32_t ) 0xff );	

	CCP1CONbits.CCP1M0 = 1;     /*< Compare match mode. */
	CCP1CONbits.CCP1M1 = 1;     /*< Compare match mode. */
	CCP1CONbits.CCP1M2 = 0;     /*< Compare match mode. */
	CCP1CONbits.CCP1M3 = 1;     /*< Compare match mode. */
	PIE1bits.CCP1IE = 1;		/*< Interrupt enable. */
                                //PIR1bits.CCP1IF interrupt flag
    
	/* We are only going to use the global interrupt bit, so set the peripheral
	bit to true. */
	
    INTCONbits.GIE = 1;
   
    T3CONbits.T3RD16 = 1;    // configured as a 16 bit timer
    T3CONbits.T3ECCP1 = 1;   // Timer3 CCP mode selected
    T3CONbits.T3CKPS = 0;    // prescaler 00 = 1, 2, 4, 8
    T3CONbits.TMR3CS = 0;    // precsaler set to 1:32, timer timer will overflow
    T3CONbits.TMR3ON = 1;    // turn timer ON
    
}

/*
void interrupt SysTickISR(void)
{
    if( PIR1bits.CCP1IF == 1 )
    {
        // clear the flag
        PIR1bits.CCP1IF = 0;
    }
}
*/
