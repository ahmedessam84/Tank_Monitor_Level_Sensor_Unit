/* 
 * File:   comm_can.h
 * Author: Ahmed
 *
 * Created on March 3, 2017, 7:20 PM
 */

#ifndef COM_CAN_H
#define	COM_CAN_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    
#define NODE_NR     0x01    // assigned by a dip switch in the future
#define NODE_TYPE   0x02    // for master = 0x01
						    // for liquid level sensor  = 0x02
							// for pressure sensor = 0x03
                            // CAN ID = (NODE_TYPE<<8) | (NODE_ID)
    
typedef struct
{
	uint8_t node_nr;							// node number
	uint8_t node_type;						// type of sensor: liquid level, pressure...etc 
	uint8_t node_data_size;				
	uint8_t * node_data_ptr;
}Node_t;    

//tCANMsgObject RTRMessage;
void ComInit(void);
/*
void CanCom_Init(void);
void TransmitData(void);
void ReceiveData( Node_t * node )
*/
#ifdef	__cplusplus
}
#endif

#endif	/* COMM_CAN_H */

