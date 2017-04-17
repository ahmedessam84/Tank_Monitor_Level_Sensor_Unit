/*
 * File:   main.c
 * Author: Ahmed
 *
 * Created on February 12, 2017, 8:16 PM
 */

// PIC18F458 Configuration Bit Settings

// 'C' source line config statements

#define _XTAL_FREQ 32000000

// CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator with PLL enabled/Clock frequency = (4 x FOSC))
#pragma config OSCS = OFF       // Oscillator System Clock Switch Enable bit (Oscillator system clock switch option is disabled (main oscillator is source))

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bit (Brown-out Reset enabled)
#pragma config BORV = 25        // Brown-out Reset Voltage bits (VBOR set to 2.5V)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 128      // Watchdog Timer Postscale Select bits (1:128)

// CONFIG4L
#pragma config STVR = ON        // Stack Full/Underflow Reset Enable bit (Stack Full/Underflow will cause Reset)
#pragma config LVP = OFF         // Low-Voltage ICSP Enable bit (Low-Voltage ICSP enabled)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000200-001FFFh) not code protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot Block (000000-0001FFh) not code protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000200-001FFFh) not write protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0001FFh) not write protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000200-001FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from Table Reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0001FFh) not protected from Table Reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
/*********************************************************************
*
*                            Includes 
*
*********************************************************************/

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"
#include "gpio.h"
#include "hcsr04.h"
#include "measure.h"
#include "CAN.h"
#include "Leds.h"
#include "clock.h"
                            // master is node nr 0x00
#define NODE_NR     0x01    // assigned by a dip switch in the future
#define NODE_TYPE   0x02    // for master = 0x01
						    // for liquid level sensor  = 0x02
							// for pressure sensor = 0x03
                            // CAN ID = (NODE_TYPE<<8) | (NODE_ID)

// **reminder** in pic 0 is Output and 1 is Input
/*********************************************************************
*
*                            Global Variables 
*
*********************************************************************/

uint16_t dist;
uint8_t str_dist[4];
uint16_t average_dist =0;
uint8_t counter = 0;

volatile bool bRxMsgFlag = 0;
volatile bool AllowTxFlag = 0;
volatile uint8_t ui8Status = 0;
volatile uint32_t tickcntr = 0;
volatile uint32_t Sync_Arrival_Time = 0;


// This is where the rtr msg will be received
extern tCANMsgObject rxCANMessage;

/*********************************************************************/

void itoa_new(uint16_t val, char * str)
{
		uint16_t temp_val;
		temp_val = val;
		str[0] = (temp_val / 100) + 48;
		temp_val = (temp_val % 100);
		str[1] = (temp_val / 10) + 48;
		str[2] = (temp_val % 10) + 48;
		str[3] = '\0';
}


/*********************************************************************
*
*                            Main Function 
*
*********************************************************************/

int main(void)
{

   // Data to be sent back to the TIVA
   uint16_t ui16ReplyData = 0 ;
   
   // remote frame acceptance ID set to 0
   // if mask set to zero any msg will be received
   
   // receive msgs only from master
   // master id is always set to 001 0000 0001
   // most significant 3 bits are the type of node
   // least significant 8 bits are the node number
   // master sends the sync period as a 16 bit value in milliseconds
   uint16_t response_time = 0;
   uint16_t sync_period = 0;
   rxCANMessage.ui32MsgID = 0x100;
   rxCANMessage.ui32MsgIDMask = 0x7ff;
   rxCANMessage.pui8MsgData = (uint8_t *)&sync_period;

   // this is the reply msg we will use to reply back with
   tCANMsgObject Reply;

   // data frame transmitter ID set to :
   // most significant 3 bits of the the 11 bit ID are set to 001
   // since this is node 1 the remaining 8 bits are set to 0000 0001
   // so the entire ID is set to 001 0000 0001
   Reply.ui32MsgID = ((NODE_TYPE<<8) | (NODE_NR)) ;
   Reply.ui32MsgLen = sizeof(ui16ReplyData);
   Reply.pui8MsgData = (uint8_t *)(&ui16ReplyData);
    
   //Sensor Module Initialization
   Clock_Init();
   ComInit(); 
   LedsInit();
   Hcsr04_Init();
   LCD_Init();
  
   LCD_DispString("Height=      cm"); 
   
   while(1)
   {
       // measure average value of distance using sensor
       dist = AvgMeasure();
       
       // if measured distance is in range then display it
       if(dist<400)
       {
            itoa_new(dist, str_dist);
            LCD_GoToXy(8,0);
            LCD_DispString(str_dist);	
       }
       // else if out of range then display xxx
       else
       {
           LCD_GoToXy(8,0);
           LCD_DispString("XXX");
       }
       
/*********************CAN Msg Reception and transmission part*****************/
       
       // update the sensor reply data with the new measurement
       ui16ReplyData = dist;
       
        // if msg received store its value
        if( bRxMsgFlag == 1 )
        {
            GIE = 0; // enter critical section
            CANMessageGet( &rxCANMessage, 0 );
            // capture the time when the sync message was received
            Sync_Arrival_Time = tickcntr;
            GIE = 1; // exit critical section
            
            bRxMsgFlag = 0;
            
            // if msg received is the master sync signal
            // calculate the response time from the sync_period
            // the response time is in multiples of the node number
            if( rxCANMessage.ui32MsgID == 0x100)
            {
                AllowTxFlag = 1;
                response_time = sync_period * (NODE_NR);
                response_time = response_time/100;                
            }
                        
        }
       
       // only send the msg after the response time has passed
       if( AllowTxFlag == 1)
       {
               
            // if response time has passed send the reply
            if( ( tickcntr - Sync_Arrival_Time ) > response_time )
            {
                CANMessageSend( &Reply, CAN_DATA_FRAME );
                AllowTxFlag = 0;
            }
            
       }
        else{};
        
/******************************CAN STATUS LEDS********************************/        
        
        if(PIR3bits.ERRIF == 1)
        {
            RED_ON;
            GREEN_OFF;
            // clear the flag to recheck error again incase the source of error was corrected
            PIR3bits.ERRIF = 0;
        }
        else
        {
            GREEN_ON;
            RED_OFF;
        }
       
   }
   
    return 0;
}

void interrupt ISR(void)
{
/**************************CAN MSG reception ISR*******************************/
    
    ui8Status = CANIntStatus();
    
    // msg received in buffer 0 or 1 or both
    if( ((ui8Status & CAN_INT_RX0) == CAN_INT_RX0) || ((ui8Status & CAN_INT_RX1) == CAN_INT_RX1)  )
    {
        bRxMsgFlag = 1;
        CANIntClear(ui8Status);
    }
    else{}; // error
    
/********************Sensor Echo ISR - interrupt on change*********************/
    
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
    
 /********************************1ms tick************************************/   
    
    if( PIR1bits.CCP1IF == 1 )
    {
        // system tick every 1ms
        
        tickcntr++;
        
        // clear the flag
        PIR1bits.CCP1IF = 0;
        
    }
    else{};
    
}
