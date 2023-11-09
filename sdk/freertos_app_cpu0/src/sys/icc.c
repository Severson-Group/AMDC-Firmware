/*
 * icc.c
 *
 *  Created on: 7 nov. 2023
 *      Author: pnowa
 */

#include "icc.h"

// KEEP THIS FILE IN SYNC IN BOTH CPU0 AND CPU1

/*
void icc_init(void){
//	MessageBufferHandle_t handle = xMessageBufferCreateStatic(size_t xBufferSizeBytes,
//															  uint8_t *pucMessageBufferStorageArea,
//															  StaticMessageBuffer_t *pxStaticMessageBuffer );

MessageBufferHandle_t handle_0_1 = xMessageBufferCreateStatic(16,
														  (uint8_t) (OCM_BASE_ADDR+1024),
														  CPU0_to_CPU1_MessageBuffer);

MessageBufferHandle_t handle_1_0 = xMessageBufferCreateStatic(16,
																(uint8_t) (OCM_BASE_ADDR+1056),
																CPU1_to_CPU0_MessageBuffer);
}
*/

// Used to dimension the array used to hold the messages.  The available space
// will actually be one less than this, so 999.
#define STORAGE_SIZE_BYTES 1000

// Defines the memory that will actually hold the messages within the message
// buffer.
static uint8_t ucBufferStorage[ STORAGE_SIZE_BYTES ];

// The variable used to hold the message buffer structure.
StaticMessageBuffer_t xMessageBufferStruct;

void MyFunction( void )
{
MessageBufferHandle_t xMessageBuffer = xMessageBufferCreateStatic( sizeof( ucBufferStorage ),
                                                 ucBufferStorage,
                                                 &xMessageBufferStruct );

    // As neither the pucMessageBufferStorageArea or pxStaticMessageBuffer
    // parameters were NULL, xMessageBuffer will not be NULL, and can be used to
    // reference the created message buffer in other message buffer API calls.

    // Other code that uses the message buffer can go here.
}



