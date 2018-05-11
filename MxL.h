/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef __MXL_H__
#define __MXL_H__
    
#include "mxl_eagle_apis.h"

typedef enum __MXL_DEMOD_TYPE_E__
{
    MXL_DEMOD_TYPE_ATSC = 0,
    MXL_DEMOD_TYPE_QAM,
    MXL_DEMOD_TYPE_MAX
} MXL_DEMOD_TYPE_E;   
    
MXL_STATUS_E MxL_DeviceInit( uint8_t devId  );
MXL_STATUS_E MxL_ConfigDemod( uint8_t devId, MXL_DEMOD_TYPE_E demodType );
MXL_STATUS_E MxL_GetTunerLockStatus( uint8_t devId );

char* MxL_Debug_StatusToString( void );

#endif /* __MXL_H__ */

/* [] END OF FILE */
