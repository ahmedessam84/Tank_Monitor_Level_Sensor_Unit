
#include <xc.h>
#include "CAN.h"
#include "com_can.h"


   // most significant 3 bits are the type of node
   // least significant 8 bits are the node number
   // master sends the sync period as a 16 bit value in milliseconds


tCANMsgObject rxCANMessage;

// this is the reply msg we will use to reply back with
tCANMsgObject Reply;

void ComInit(void)
{
    CANInit();
    CANSetMode( CAN_MODE_CONFIG );
    
    // The three functions below must be called in configuration mode
    CANSetBaudRate( 125000 );
    CANSetMask( CAN_RX0, &rxCANMessage, CAN_RX_ALL_VALID_MSG, 0 );
    CANSetFilter( CAN_FILTER_B0_F0, &rxCANMessage );

    CANSetMode( CAN_MODE_NORMAL );

    CANIntClear( CAN_INT_RX0 | CAN_INT_RX1 );
    CANIntEnable( CAN_INT_RX0 | CAN_INT_RX1 );
    
    INTCONbits.GIE = 1 ; // Enable global interrupt
    INTCONbits.PEIE = 1; // Enable Peripheral interrupt  
}
/*
void CanCom_Slave_Init(void)
{   
   // receive msgs only from master
   // master id is always set to 001 0000 0001
   // most significant 3 bits are the type of node
   // least significant 8 bits are the node number
   // master sends the sync period as a 16 bit value in milliseconds
   rxCANMessage.ui32MsgID = 0x101;
   rxCANMessage.ui32MsgIDMask = 0x7ff;
   rxCANMessage.pui8MsgData = (uint8_t *)&sync_period;   
}


void ReceiveData( Node_t * node )
{
		CANMessageGet( &rxCANMessage, 0 );
		node->node_nr = (rxCANMessage.ui32MsgID & 0xff); 		   // extact the first 8 bits which correspond to the node id
		node->node_type = ((rxCANMessage.ui32MsgID) & (0x700))>>8; // extract the node type from the msg id
		node->node_data_size = rxCANMessage.ui32MsgLen;
		node->node_data_ptr = rxCANMessage.pui8MsgData;
}

void TransmitData(Node_t * node)
{
   // data frame transmitter ID set to :
   // most significant 3 bits of the the 11 bit ID are set to 001
   // since this is node 1 the remaining 8 bits are set to 0000 0001
   // so the entire ID is set to 001 0000 0001
   Reply.ui32MsgID = ((node->node_type<<8) | (node->node_nr)) ;
   Reply.ui32MsgLen = node->node_data_size;
   Reply.pui8MsgData = (uint8_t *)node->node_data_ptr;

   CANMessageSend( &Reply, CAN_DATA_FRAME );
}
*/






