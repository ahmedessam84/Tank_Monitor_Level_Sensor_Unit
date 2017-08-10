
#include <pic18f458.h>

#include <xc.h>
#include <stdint.h>
#include "stdbool.h"
#include "CAN.h"


void CANInit(void)
{
    CIOCON = 0x20;          // tie CANTX to Vdd when idle to keep the bus in recessive mode		
    TRISBbits.RB3 = 1;      // set as input  CANRX
    TRISBbits.RB2 = 0;      // set as output CANTX
}


void CANSetMode(uint8_t mode)
{
    // Place CAN module in configuration mode, see CANCON register data
    SET_REG_DATA( CANCON, CAN_MODE_BITS, mode<<5 );                //REQOP bits <2:0> = 0b100
    while( ( GET_REG_DATA( CANSTAT, CAN_MODE_BITS ) != (mode<<5) ));    //Wait for op-mode bits in the
                                                                //CANSTAT register to = 0b100
                                                                //to indicate config mode OK
}



uint8_t CANGetMode()
{
    return GET_REG_DATA( CANSTAT, CAN_MODE_CONFIG );
}



void CANSetBaudRate(uint32_t baudrate)
{
    
    // Initialize CAN Bus bit rate timing. Assumes only four standard rates.
    if (baudrate==100000)  //  100 kbps @ 32 MHz 
    {
        BRGCON1 = 0x89; //1001 1001     //SJW=3TQ     BRP  9
        BRGCON2 = 0xB8; //1011 1000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0101     //PS2  6TQ
    } 
    else if (baudrate==125000) //  125 kbps @ 32 MHz
    {
        BRGCON1 = 0x87; //1000 0111     //SJW=3TQ     BRP  7
        BRGCON2 = 0xB8; //1011 1000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0101     //PS2  6TQ 
    }
    else if (baudrate==500000) //  500 kbps @ 32 MHz
    {
        BRGCON1 = 0x81; //1000 0001     //SJW=3TQ     BRP  1
        BRGCON2 = 0xB8; //1011 1000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0101     //PS2  6TQ
    }
    else if (baudrate==1000000)  //  1 Mbps (1000 kbps) @ 34 MHz
    {
          
        BRGCON1 = 0x80; //1000 0000     //SJW=3TQ     BRP  0
        BRGCON2 = 0xB8; //1011 1000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0101     //PS2  6TQ
    } 
    else                //default to 100 kbps if necessary
    {
        //  100 Kbps @ 32MHz  
        BRGCON1 = 0x89; //0001 1001     //SJW=3TQ     BRP  9
        BRGCON2 = 0xB8; //1010 0000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0010     //PS2  6TQ
    }
}




/******************************************************************************
 * This function sets the MASK registers according to the input parameters
 * ui8CanBfrNum:    Chooses which BUFFER MASK to fill
 * ui32MsgIDMask: Is the user value to be inserted in the Mask register
 * ui8IDType:   Indicates whether the register is a standard or extended ID type
 * bDoubleBuffer:  Set to 1 or 0 to choose whether data should be stored in buffer 1
 *                when buffer zero is full
 ******************************************************************************/

void CANSetMask( uint8_t ui8CanBfrNum, tCANMsgObject * pObject , uint8_t ui8IDType, uint8_t bDoubleBuffer )
{
    //choose register BUFFER mask to setup

    /***********************RECEIVE BUFFER 0 MASK*****************************/
    
    //if receive register 0 chosen then:
    if( ui8CanBfrNum == 0 )
    {
        SET_REG_DATA( RXB0CON, ((3<<5) | (1<<2)), ((ui8IDType<<5) | (bDoubleBuffer<<2)) );
        
        // STANDARD IDS ONLY i.e 11-bits or less
        if( pObject->ui32MsgID <= 0x7ff )
        {
            SET_REG_DATA( RXM0SIDH, 0xFF, pObject->ui32MsgIDMask>>3 );
            SET_REG_DATA( RXM0SIDL, 0xE0, pObject->ui32MsgIDMask<<5 );     
        }
        // EXTENDED IDS
        else
        {
            SET_REG_DATA( RXM0SIDH, 0xFF, pObject->ui32MsgIDMask>>21 );
            SET_REG_DATA( RXM0SIDL, 0x03, pObject->ui32MsgIDMask>>16 );
            SET_REG_DATA( RXM0EIDH, 0xFF, pObject->ui32MsgIDMask>>8 );
            SET_REG_DATA( RXM0EIDL, 0xFF, pObject->ui32MsgIDMask );
        }
        
    }
    else{};

/***************************RECEIVE BUFFER 1 MASK*****************************/
    
    // if receive register 1 then
    if ( ui8CanBfrNum == 1)
    {
        SET_REG_DATA( RXB1CON, (3<<5), ui8IDType<<5 );
        
        // STANDARD IDS ONLY i.e 11-bits or less
        if( pObject->ui32MsgID <= 0x7ff )
        {
            SET_REG_DATA( RXM1SIDH, 0xFF, pObject->ui32MsgIDMask>>3 );
            SET_REG_DATA( RXM1SIDL, 0xE0, pObject->ui32MsgIDMask<<5 );
        }   
        // EXTENDED IDS
        else
        {
            SET_REG_DATA( RXM1SIDH, 0xFF, pObject->ui32MsgIDMask>>21 );
            SET_REG_DATA( RXM1SIDL, 0x03, pObject->ui32MsgIDMask>>16 );
            SET_REG_DATA( RXM1EIDH, 0xFF, pObject->ui32MsgIDMask>>8 );
            SET_REG_DATA( RXM1EIDL, 0xFF, pObject->ui32MsgIDMask );
        }
        
    }
    else{};

}


/******************************************************************************
 * This function sets the filter registers according to the input parameters
 * ui8CanFltrNum:  chooses which filter to fill
 * value:       is the value to be inserted in the register
 * ui8IDType: indicates whether the register is a standard or extended ID type
 ******************************************************************************/

void CANSetFilter( uint8_t ui8CanFltrNum, tCANMsgObject * pObject )
{
    /*from the datasheet the addresses of the filter registers start from location
     * 0xf00 by register RXF0SIDH, RXF0SIDL, RXF0EIDH, then RXF0EIDL. 
     * 
     * 
     * it starts over again for the next filter and so on till the 5th filter.
     *
     * the filter number changes after we increment 4 places in memory 
     */
    
    volatile uint8_t * FILTER_BASE_ADDRESS = 0;
    // get filter base address form input parameter of function
    switch (ui8CanFltrNum)
    {
        case 0:
            FILTER_BASE_ADDRESS = &RXF0SIDH;
            break;
        case 1:
            FILTER_BASE_ADDRESS = &RXF1SIDH;
            break;
        case 2:
            FILTER_BASE_ADDRESS = &RXF2SIDH;
            break;
        case 3:
            FILTER_BASE_ADDRESS = &RXF3SIDH;
            break;
        case 5:
            FILTER_BASE_ADDRESS = &RXF4SIDH;
            break;
        case 6:
            FILTER_BASE_ADDRESS = &RXF5SIDH;
            break;
    }

    // Apply filter base accordingly to the new register pointers
    volatile uint8_t * RXFSIDH_PTR = FILTER_BASE_ADDRESS ;
    volatile uint8_t * RXFSIDL_PTR = FILTER_BASE_ADDRESS + 1 ;
    volatile uint8_t * RXFEIDH_PTR = FILTER_BASE_ADDRESS + 2 ;
    volatile uint8_t * RXFEIDL_PTR = FILTER_BASE_ADDRESS + 3 ;
    
    
/***************************** RX FILTER 'n' *******************************/
        
        // STANDARD ID FILTER SETTINGS
        if( pObject->ui32MsgID <= 0x7ff )
        {

                SET_REG_DATA( *(RXFSIDH_PTR), 0xFF, (pObject->ui32MsgID)>>3 );
                SET_REG_DATA( *(RXFSIDL_PTR), 0xE0, (pObject->ui32MsgID)<<5 ); 
        }
        // EXTENDED ID FILTER SETTINGS
        else
        {
            SET_REG_DATA( *(RXFSIDH_PTR), 0xFF, (pObject->ui32MsgID)>>21 );
            SET_REG_DATA( *(RXFSIDL_PTR), 0x03, (pObject->ui32MsgID)>>16 );
            SET_REG_DATA( *(RXFEIDH_PTR), 0xFF, (pObject->ui32MsgID)>>8 );
            SET_REG_DATA( *(RXFEIDL_PTR), 0xFF, (pObject->ui32MsgID) );
        }
    
}

/*******************************************************************************
 * This function is used to receive a frame from the RX Buffer of the controller
 * and store it in a tCANMsgObject struct. it also clears any RX interrupt by 
 * setting bClrPendingInt to '1'
 ******************************************************************************/

uint8_t CANMessageGet(tCANMsgObject *psMsgObject, bool bClrPendingInt)
{
    uint8_t RXMsgFlag;          // Temporary storage for message flag
    RXMsgFlag = 0x00;           // Set message flag to zero to start
    
    //make sure no previous data is stored in flags variable
    psMsgObject->ui32Flags = MSG_OBJ_NO_FLAGS;
    
    //check for error flag and set ui32Flags accordingly
    if( GET_REG_DATA( PIR3, 1<<5 ))         //status of ERRIF bit in PIR3 register 
    {
         psMsgObject->ui32Flags |= MSG_OBJ_ERROR;
         /*********error handling should be implemented here*********/
    }
    
/*******************Check if msg received is in buffer 0**********************/
    
    if ( RXB0CONbits.RXFUL == 1 )  // Check RXB0CON bit RXFUL to see if RX Buffer 0
                                        // has received a message, if so, get the
                                        // associated data from the buffer and save it.
    {
        // check if STANDARD ID RECEIVED and store its value
        if( RXB0SIDLbits.EXID == CAN_RXed_STD_ID )
        {
            psMsgObject->ui32MsgID = ( RXB0SIDH<<3 ) | ( RXB0SIDL>>5 );
        }
        // else EXTENDED ID RECEIVED, store its value
        else
        {
            psMsgObject->ui32MsgID = ( ( ((uint32_t)RXB0SIDH)<<21 ) | ( (GET_REG_DATA( ((uint32_t)RXB0SIDL), 0x00E0))<<18 ) | ( (GET_REG_DATA(((uint32_t)RXB0SIDL), 0x0003))<<16 ) | ( (uint32_t)RXB0EIDH<<8 ) | (uint32_t)( RXB0EIDL ) ) ;
        }

        //store data length of received msg
        psMsgObject->ui32MsgLen =  RXB0DLC;
        
        //check if msg is a REMOTE FRAME and set flag accordingly:
        if( GET_REG_DATA( RXB0DLC, 1<<6 ) )     // get status of RXRTR bit
        {
            psMsgObject->ui32Flags |= MSG_OBJ_RX_REMOTE_FRAME;
        }
        //else msg is a DATA FRAME
        else
        {
        //store data through looping over the data registers for buffer 0
            int i;
            volatile uint8_t * rxBuffer_ptr = &RXB0D0;
            volatile uint8_t * data_ptr = psMsgObject->pui8MsgData;
            for( i=0; i<(psMsgObject->ui32MsgLen); i++ )
            {
                //contents of address of the data are filled with the data received in the CAN Controller registers one by one
                *(data_ptr) = *(rxBuffer_ptr);
                data_ptr++;
                rxBuffer_ptr++;    
            }
        }
        
        SET_REG_DATA( RXB0CON, 0x80, 0 );      // Reset buffer-0-full bit to show "empty" to allow receiving of other msgs
        RXMsgFlag = 0x01;                      // Set message flag to 1 to indicate Msg received and stored
    }
    
/*******************Check if msg received is in buffer 1**********************/
    
    else if ( RXB1CONbits.RXFUL == 1 )      // Check RXB1CON bit RXFUL to see if RX Buffer 1
                                            // has received a message, if so, get the
                                            // associated data from the buffer and save it.
    {
        
        // check if STANDARD ID RECEIVED and store its value
        if( RXB1SIDLbits.EXID == CAN_RXed_STD_ID )
        {
            psMsgObject->ui32MsgID = ( RXB1SIDH<<3 ) | ( RXB1SIDL>>5 );
        }
        
        // check if EXTENDED ID RECEIVED and store its value
        else
        {
            psMsgObject->ui32MsgID = ( ((uint32_t)RXB1SIDH<<21) | ((GET_REG_DATA( (uint32_t)RXB1SIDL, 0xE0))<<18) | ((GET_REG_DATA( (uint32_t)RXB1SIDL, 0x03))<<16) | ((uint32_t)RXB1EIDH<<8) | (uint32_t)(RXB1EIDL) ) ;
        }

        psMsgObject->ui32MsgLen =  RXB1DLC;
        
        //check if msg is a REMOTE FRAME and set flag accordingly:
        if( GET_REG_DATA( RXB1DLC, 1<<6 ) )     // get status of RXRTR bit
        {
            psMsgObject->ui32Flags |= MSG_OBJ_RX_REMOTE_FRAME;
        }
        //else msg is a DATA FRAME
        else
        {
            //store data through looping over the data registers for buffer 1 
            int i;
            volatile uint8_t * rxBuffer_ptr = &RXB1D0; 
            volatile uint8_t * data_ptr = psMsgObject->pui8MsgData;
            for(i=0; i< ( psMsgObject->ui32MsgLen ); i++ )
            {
                //contents of address of data given is filled with the received Msg
                *(data_ptr) = *(rxBuffer_ptr);
                data_ptr ++;
                rxBuffer_ptr ++;    
            }
        }
        
        SET_REG_DATA( RXB1CON, 0x80, 0 );  //Reset buffer-1-full bit to show "empty" to allow receiving of other msgs
        RXMsgFlag = 0x01;                  // Set message flag to 1
    }
    
    /*************************************************************************/
    /*after message has been received and successfully stored clear the flag
     and return TRUE. if no message was received return false.*/
    
    if  (RXMsgFlag == 0x01)     // Test message flag.
                                // if message flag is a 1...
    {
        RXMsgFlag = 0x00;       // Clear the message flag
        //clear interrupt flags if requested to do so by user
        if(bClrPendingInt)
        {
            SET_REG_DATA( PIR3, 1<<1, 0);    // Clear the Buffer-0 interrupt bit RXB1IF (if used)
            SET_REG_DATA( PIR3, 1<<0, 0);    // Clear the Buffer-1 interrupt bit RXB0IF (if used)
        }
        else{};
        
        return TRUE;            // Return a "true" condition to the code that
                                // called the ECAN_Receive function.
    }
    else
    {
        return FALSE;           // Otherwise, return a false condition.
    }    
}


/******************************************************************************
 *  This function sends any data stored in a tCANMsgObject 
 ******************************************************************************/

void CANMessageSend(tCANMsgObject *psMsgObject, uint8_t FrameType)
{
    
    // Check to see which transmit buffer is free and available.
    // There are three TX buffers 0, 1 & 2. When TXREQ bit in register TXbnCON  
    // is set it indicates a msg in the buffer waiting to be sent. The buffer to 
    // be used must have its TXREQ = 0. If no TXREQ = 1 the function should wait  
    // for a specific amount of time then issue a timeout error if transmission
    // fails to send msg in the specified amount of time
    
    volatile uint8_t * TXB_BASE_ADDRESS = &TXB0CON;
    
    // start with buffer 0 --> 2 this loop will go forever until an empty buffer is found
    
    while( (GET_REG_DATA(*(TXB_BASE_ADDRESS), 1<<3)) == 1 )  // while buffer 'n' is full i.e TXREQ = 1
    {
        // increment base address by 0x10 to go to next buffer address base
        if( TXB_BASE_ADDRESS <= &TXB1CON)
        {
            TXB_BASE_ADDRESS += 0x10;
        }
        else            // if address reaches the last buffer go back to the first
        {
            TXB_BASE_ADDRESS = &TXB0CON;    // reset the address base to start again from buffer zero
        }
    }

    
    //setup GENERIC temp CAN register pointers to change their address according
    //to the transmission buffer that is available
        
    volatile uint8_t * TXBCON_PTR       = TXB_BASE_ADDRESS;
    volatile uint8_t * TXBSIDH_PTR      = TXB_BASE_ADDRESS + 1;
    volatile uint8_t * TXBSIDL_PTR      = TXB_BASE_ADDRESS + 2;
    volatile uint8_t * TXBEIDH_PTR      = TXB_BASE_ADDRESS + 3;
    volatile uint8_t * TXBEIDL_PTR      = TXB_BASE_ADDRESS + 4;
    volatile uint8_t * TXBDLC_PTR       = TXB_BASE_ADDRESS + 5;
    volatile uint8_t * TXBD0_PTR        = TXB_BASE_ADDRESS + 6;
    
    //check if msg ID is standard or extended from its size
    
    if( (psMsgObject->ui32MsgID) <= 2047 )
    {
        // standard msg id loading
        SET_REG_DATA(*(TXBSIDL_PTR), 0x08, 0x00);  // set EXIDE = 0;

        SET_REG_DATA(*(TXBSIDH_PTR), 0xFF, ((psMsgObject->ui32MsgID))>>3);
        SET_REG_DATA(*(TXBSIDL_PTR), 0xE0, ((psMsgObject->ui32MsgID))<<5);
    }
    else
    {
        // extended msg id loading
        SET_REG_DATA(*(TXBSIDL_PTR), 0x08, 0xff);  // set EXIDE = 1;
        
        SET_REG_DATA(*(TXBEIDL_PTR), 0xFF, ((psMsgObject->ui32MsgID)));
        SET_REG_DATA(*(TXBEIDH_PTR), 0xFF, ((psMsgObject->ui32MsgID))>>8);
        SET_REG_DATA(*(TXBSIDL_PTR), 0x03, ((psMsgObject->ui32MsgID))>>16);
        SET_REG_DATA(*(TXBSIDL_PTR), 0xE0, ((psMsgObject->ui32MsgID))>>18);
        SET_REG_DATA(*(TXBSIDH_PTR), 0xFF, ((psMsgObject->ui32MsgID))>>21);
    }
    
    //psMsgObject->ui32Flags |= FrameType;
    
    // if DATA FRAME
    if( FrameType == CAN_DATA_FRAME )
    {
        // Load Tx data and DLC registers with values stored in tCANMsgObject
        SET_REG_DATA( *(TXBDLC_PTR), 0x0F, psMsgObject->ui32MsgLen );
        // clear rtr
        SET_REG_DATA( *(TXBDLC_PTR), 1<<6, 0x00 );
        
        int i;
        volatile uint8_t * data_reg_ptr = TXBD0_PTR;
        volatile uint8_t * data_ptr = psMsgObject->pui8MsgData;

        for(i=0; i<(psMsgObject->ui32MsgLen); i++)
        {
            *data_reg_ptr = *(data_ptr);
            data_reg_ptr++;
            data_ptr++;
        }
    }
    else
    {
        SET_REG_DATA( *(TXBDLC_PTR), 1<<6, 0xFF ); // set TXRTR to send remote request, no need to load data registers
    }
    
    //after loading data and settings into registers transmit message by setting TXREQ
    SET_REG_DATA( *TXBCON_PTR, 1<<3, 0xFF);
    
}


void CANIntEnable(uint8_t type)
{
    if( (type & 1<<0) == 1 )
    {
        SET_REG_DATA( PIE3, 1<<0, 0xFF );      // enable rxb0 buffer interrupt
    }
    else{};
    
    if( (type & 1<<1) == 2 )
    {
        SET_REG_DATA( PIE3, 1<<1, 0xFF);       // enable rxb1 buffer interrupt
    }
    else{};
    
    if( (type & 1<<2) == 3 )
    {
        SET_REG_DATA( PIE3, 1<<2, 0xFF);       // enable tx0 buffer interrupt
    }
    else{};
    
    if( (type & 1<<3) == 4 )
    {
        SET_REG_DATA( PIE3, 1<<3, 0xFF);       // enable tx1 buffer interrupt
    }
    else{};
    
    if( (type & 1<<4) == 5 )
    {
        SET_REG_DATA( PIE3, 1<<4, 0xFF);       // enable tx2 buffer interrupt
    }
    else{};
    
    if( (type & 1<<5) == 6 )
    {
        SET_REG_DATA( PIE3, 1<<5, 0xFF);       // enable error interrupt
    }
    else{};
    
    if( (type & 1<<6) == 7 )
    {
        SET_REG_DATA( PIE3, 1<<6, 0xFF);        // enable wake up interrupt
    }
    else{};
    
    if( (type & 1<<7) == 8 )
    {
        SET_REG_DATA( PIE3, 1<<7, 0xFF);        // enable invalid msg interrupt
    }
    else{};
    
}



void CANIntDisable(uint8_t type)
{
    if( (type & 1<<0) == 1 )
    {
        SET_REG_DATA( PIE3, 1<<0, 0x00 );      // enable rxb0 buffer interrupt
    }
    else{};
    
    if( (type & 1<<1) == 2 )
    {
        SET_REG_DATA( PIE3, 1<<1, 0x00);       // enable rxb1 buffer interrupt
    }
    else{};
    
    if( (type & 1<<2) == 3 )
    {
        SET_REG_DATA( PIE3, 1<<2, 0x00);       // enable tx0 buffer interrupt
    }
    else{};
    
    if( (type & 1<<3) == 4 )
    {
        SET_REG_DATA( PIE3, 1<<3, 0x00);       // enable tx1 buffer interrupt
    }
    else{};
    
    if( (type & 1<<4) == 5 )
    {
        SET_REG_DATA( PIE3, 1<<4, 0x00);       // enable tx2 buffer interrupt
    }
    else{};
    
    if( (type & 1<<5) == 6 )
    {
        SET_REG_DATA( PIE3, 1<<5, 0x00);       // enable error interrupt
    }
    else{};
    
    if( (type & 1<<6) == 7 )
    {
        SET_REG_DATA( PIE3, 1<<6, 0x00);        // enable wake up interrupt
    }
    else{};
    
    if( (type & 1<<7) == 8 )
    {
        SET_REG_DATA( PIE3, 1<<7, 0x00);        // enable invalid msg interrupt
    }
    else{};
    
}


void CANIntClear(uint8_t type)
{
    if( (type & 1<<0) == 1 )
    {
        SET_REG_DATA( PIR3, 1<<0, 0 );      // enable rxb0 buffer interrupt
    }
    else{};
    
    if( (type & 1<<1) == 2 )
    {
        SET_REG_DATA( PIR3, 1<<1, 0);       // enable rxb1 buffer interrupt
    }
    else{};
    
    if( (type & 1<<2) == 3 )
    {
        SET_REG_DATA( PIR3, 1<<2, 0);       // enable tx0 buffer interrupt
    }
    else{};
    
    if( (type & 1<<3) == 4 )
    {
        SET_REG_DATA( PIR3, 1<<3, 0);       // enable tx1 buffer interrupt
    }
    else{};
    
    if( (type & 1<<4) == 5 )
    {
        SET_REG_DATA( PIR3, 1<<4, 0);       // enable tx2 buffer interrupt
    }
    else{};
    
    if( (type & 1<<5) == 6 )
    {
        SET_REG_DATA( PIR3, 1<<5, 0);       // enable error interrupt
    }
    else{};
    
    if( (type & 1<<6) == 7 )
    {
        SET_REG_DATA( PIR3, 1<<6, 0);        // enable wake up interrupt
    }
    else{};
    
    if( (type & 1<<7) == 8 )
    {
        SET_REG_DATA( PIR3, 1<<7, 0);        // enable invalid msg interrupt
    }
    else{};
    
}


uint8_t CANIntStatus(void)
{
    uint8_t status = 0;
    
    status |= GET_REG_DATA( PIR3, 1<<0 );       // rxb0 buffer interrupt
    status |= GET_REG_DATA( PIR3, 1<<1 );       // rx1
    status |= GET_REG_DATA( PIR3, 1<<2 );       // tx0
    status |= GET_REG_DATA( PIR3, 1<<3 );       // tx1
    status |= GET_REG_DATA( PIR3, 1<<4 );       // tx2
    status |= GET_REG_DATA( PIR3, 1<<5 );       // err
    status |= GET_REG_DATA( PIR3, 1<<6 );       // wake
    status |= GET_REG_DATA( PIR3, 1<<7 );       // invalid msg int
    
    return status;
    
}

ERROR_STATE_t CANerrState(void)
{
    ERROR_STATE_t rState = ERROR_ACTIVE;
    
    if( COMSTATbits.RXBP == 1 | COMSTATbits.TXBP == 1 )
    {
        rState = ERROR_PASSIVE;
    }
    else if( COMSTATbits.TXBO )
    {
        rState = BUS_OFF;
    }
    else if( COMSTATbits.EWARN )
    {
        rState = WARNING;
    }
    
    return rState;
    
}


        