/* 
 * File:   CAN.h
 * Author: Ahmed
 *
 * Created on February 18, 2017, 8:13 PM
 */

#ifndef CAN_H
#define	CAN_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
#define TRUE    1
#define FALSE   0
    
#define    SET_REG_DATA(REG,MASK,DATA)    REG = (REG & ~(MASK)) | (MASK & DATA)	//inserts data into selected register bits
#define    GET_REG_DATA(REG,MASK)         REG & MASK	

// CAN OPERATION MODES BITS 5,6,7 IN CANCON:
#define    CAN_MODE_BITS              0xE0   // Use this as a mask to access opmode  bits
#define    CAN_MODE_NORMAL            0
#define    CAN_MODE_DISABLE           1
#define    CAN_MODE_LOOP              2
#define    CAN_MODE_LISTEN            3
#define    CAN_MODE_CONFIG            4
        
// CAN receive buffers mask registers for ui8CanBfrNum parameter in CANSetMask():
#define    CAN_RX0                    0
#define    CAN_RX1                    1

// ui8IDType parameters for use in function CANSetMask():
#define    CAN_RX_STD_MSG             0x01   
#define    CAN_RX_XTD_MSG             0x10
#define    CAN_RX_ALL_MSG             0x11
#define    CAN_RX_ALL_VALID_MSG       0x00

// Double buffer enable for CANSetMask():
#define    CAN_DOUBLE_BUFFER_EN       0x01
#define    CAN_DOUBLE_BUFFER_DIS      0X00
    
// Extended identifier filter enable BIT
#define    CAN_XTD_ONLY_FILTER        0x01
#define    CAN_STD_ONLY_FILTER        0x00

// Use for ui8CanFltrNum parameter in function CANSetFilter()
#define    CAN_FILTER_B0_F0           0 
#define    CAN_FILTER_B0_F1           1
#define    CAN_FILTER_B1_F0           2
#define    CAN_FILTER_B1_F1           3
#define    CAN_FILTER_B1_F2           4
#define    CAN_FILTER_B1_F3           5
    
// Received ID is XTD or STD flag, use with function CANMessageGet():
#define     CAN_RXed_STD_ID          0x00
#define     CAN_RXed_XTD_ID          0x01

// Flags to be used with ui32flags Rx:
#define     MSG_OBJ_NO_FLAGS         0
#define     MSG_OBJ_RX_REMOTE_FRAME  1<<6
#define     MSG_OBJ_NEW_DATA         1<<0
#define     MSG_OBJ_ERROR            1<<1
    
// Flags to be used with ui32flags for Tx:
// Configure the flag to transfer remote request by OR'ing ui32flags with:
#define     CAN_REMOTE_FRAME         1<<7
#define     CAN_DATA_FRAME           1<<2
    
// CAN interrupt type to be used with CANIntEnable()
// You can combine any number of interrupts together by OR'ing them together
#define     CAN_INT_RX0              1<<0
#define     CAN_INT_RX1              1<<1
#define     CAN_INT_TX0              1<<2
#define     CAN_INT_TX1              1<<3
#define     CAN_INT_TX2              1<<4
#define     CAN_INT_ERR              1<<5
#define     CAN_INT_WAK              1<<6
#define     CAN_INT_IV               1<<7
    
/********************************data types***********************************/

typedef enum{
    
    ERROR_ACTIVE,
    ERROR_PASSIVE,
    BUS_OFF,
    WARNING
            
}ERROR_STATE_t;



typedef struct
{
    //
    //! The CAN message identifier used for 11 or 29 bit identifiers.
    //
    uint32_t ui32MsgID;

    //
    //! The message identifier mask used when identifier filtering is enabled.
    //
    uint32_t ui32MsgIDMask;

    //
    //! This value holds various status flags and settings specified by
    //! tCANObjFlags.
    //
    uint32_t ui32Flags;

    //
    //! This value is the number of bytes of data in the message object.
    //
    uint32_t ui32MsgLen;

    //
    //! This is a pointer to the message object's data.
    //
    uint8_t *pui8MsgData;
}
tCANMsgObject;
    
/**********************************functions**********************************/    

void CANSetMode(uint8_t mode);
uint8_t CANGetMode();
void CANInit(void);
void CANSetBaudRate(uint32_t baudrate);
void CANSetMask( uint8_t CAN_RX_REG, tCANMsgObject * , uint8_t CAN_ID_TYPE, bool bDoubleBuffer );
void CANSetFilter( uint8_t CAN_FILTER, tCANMsgObject * );
uint8_t CANMessageGet(tCANMsgObject *psMsgObject, bool bClrPendingInt);
void CANMessageSend(tCANMsgObject *psMsgObject, uint8_t CAN_TX_MSG_FLAGS);
void CANIntDisable(uint8_t type);
void CANIntClear(uint8_t type);
uint8_t CANIntStatus(void);
void CANIntEnable(uint8_t type);
ERROR_STATE_t CANerrState(void);


#ifdef	__cplusplus
}
#endif

#endif	/* CAN_H */

