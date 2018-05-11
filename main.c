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
#include "project.h"
#include "debug.h"
#include "MxL.h"

#define FALSE (uint8)(0)
#define TRUE  (uint8)(!FALSE)
#define NOTIFY_MASK (0x01u)

//#define MXL_INTEGRATION

typedef struct _connection_handle
{
    CYBLE_CONN_HANDLE_T handle;
    uint8 connected;
} CONNECTION_HANDLE;

static CONNECTION_HANDLE btConHandle;
static uint8_t notifyBattery = FALSE;
static uint8_t notifySignalStrength = FALSE;


static void ResetIsrHandler( void );
static void TimeoutIsr_Handler( void );
static void BLE_Stack_Handler( uint32 eventCode, void *eventParam );
static uint8 GetUint16Limits( uint16_t limitHandle, uint16_t *max, uint16_t *min );
static uint8 GetData2Bytes( CYBLE_GATT_DB_ATTR_HANDLE_T attribute, uint16_t *data );
static uint8 GetData1Byte( CYBLE_GATT_DB_ATTR_HANDLE_T attribute, uint8_t *data );
static uint8 SetData2Bytes( CYBLE_GATT_DB_ATTR_HANDLE_T attribute, uint16_t data );
static uint8 SetData1Byte( CYBLE_GATT_DB_ATTR_HANDLE_T attribute, uint8_t notify, uint8_t data );

#if 0
void ConfigureI2CMaster( uint32 slaveAddr )
{
    (void)slaveAddr;
    I2CM_Start();
}
#endif


int main(void)
{
#ifdef MXL_INTEGRATION
    uint8_t result = FALSE;
    uint8_t deviceId = 0;
    uint32_t frequency = 0;
    uint8_t slaveAddr = 0;
    uint8_t tunerType = 0xFF;
    uint8_t demodBw = 0xFF;
#endif /* MXL_INTEGRATION */

    CyGlobalIntEnable; /* Enable global interrupts. */


#ifdef DEBUG_CONSOLE
    DebugConsole_Init();
#endif


#ifdef MXL_INTEGRATION
    result = GetData2Bytes( CYBLE_TUNERCONFIGURATION_FREQUENCY_CHAR_HANDLE, &frequency );
    result = GetData1Byte( CYBLE_TUNERCONFIGURATION_SLAVEADDR_CHAR_HANDLE, &slaveAddr );
    result = GetData1Byte( CYBLE_TUNERCONFIGURATION_TUNERTYPE_CHAR_HANDLE, &tunerType );
    result = GetData1Byte( CYBLE_TUNERCONFIGURATION_DEMODULATORBW_CHAR_HANDLE, &demodBw );
    (void)MxL_DeviceInit( deviceId );
    (void)MxL_ConfigDemod( deviceId, MXL_DEMOD_TYPE_ATSC );
    (void)MxL_GetTunerLockStatus( deviceId );
#endif /* MXL_INTEGRATION */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    CyBle_Start( BLE_Stack_Handler );

    SW2Isr_StartEx( ResetIsrHandler );

    /* Make initialisation of Interruption */
    TimeoutIsr_StartEx( TimeoutIsr_Handler );

    for(;;)
    {
        /* Place your application code here. */
        CyBle_ProcessEvents();
    }
}


static void BLE_Stack_Handler( uint32 eventCode, void *eventParam )
{
    uint8 result = TRUE;
    CYBLE_GATTS_ERR_PARAM_T errCode;
    CYBLE_API_RESULT_T ble_result = CYBLE_ERROR_OK;

    DBG("Event: %s(0x%x)", StringifyBleEvent(eventCode), eventCode);

    switch( eventCode )
    {
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        {
            InterruptTimer_Stop();
            btConHandle.connected = FALSE;
            PWM_WritePeriod(255);
        }
            /* Passing by */
        case CYBLE_EVT_STACK_ON:
        {
            CyBle_GappStartAdvertisement( CYBLE_ADVERTISING_FAST );
            PWM_Start();
            break;
        }
        case CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE:
        {
            break;
        }

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
        {
            uint16_t updateTimeout = 0;

            DBG("Timeout period: %d", InterruptTimer_ReadPeriod());
            GetData2Bytes( CYBLE_TUNERCONFIGURATION_UPDATETIMEOUT_CHAR_HANDLE, &updateTimeout );
            InterruptTimer_Start();
            InterruptTimer_WritePeriod( updateTimeout );
            DBG("Timeout period: %d", InterruptTimer_ReadPeriod());

            break;
        }

        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
        {
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {
                /* Fast and slow advertising period complete, go to low power
                 * mode (Hibernate mode) and wait for external
                 * user event to wake up device again */
                DBG("Hibernate\r\n");
            #if (DEBUG_UART_ENABLED == ENABLED)
                //while((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) != 0);
            #endif /* (DEBUG_UART_ENABLED == ENABLED) */
                Pin_SW2_ClearInterrupt();
                SW2Isr_ClearPending();
                SW2Isr_Start();
                Blue_Write( 0 ); // Turn off LED
                CySysPmHibernate();
            }
            break;
        }
#if 1
        case CYBLE_EVT_GATT_CONNECT_IND:
        {
            if( FALSE == btConHandle.connected )
            {
                btConHandle.handle = *(CYBLE_CONN_HANDLE_T*)eventParam;
                btConHandle.connected = TRUE;
                PWM_WritePeriod(4999);
            }
            else
            {
                /* Do we support multi client connection? */
                ERR("Another connection - do we support it?");
            }
            break;
        }

        case CYBLE_EVT_GATTS_WRITE_REQ:
        {
            CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReq = NULL;
            CYBLE_GATT_DB_ATTR_HANDLE_T attr;

            wrReq = (CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam;

            if ( NULL == wrReq )
            {
                ERR("Invalid param");
                result = FALSE;
            }

            if( TRUE == result )
            {
                attr = wrReq->handleValPair.attrHandle;

                if( 0 == memcmp( (const void*)&(wrReq->connHandle), (const void*)&(btConHandle.handle), sizeof(btConHandle.handle) ))
                {
                    if( CYBLE_TUNERCONFIGURATION_FREQUENCY_CHAR_HANDLE == attr )
                    {
                        uint16 min = 0;
                        uint16 max = 0;
                        uint16 freq = wrReq->handleValPair.value.val[1]<<8|wrReq->handleValPair.value.val[0];

                        result = GetUint16Limits( CYBLE_TUNERCONFIGURATION_FREQUENCY_VALID_RANGE_DESC_HANDLE, &max, &min );
                        if( TRUE == result )
                        {
                            if(( freq >= min ) && ( freq <= max ))
                            {
                                CyBle_GattsWriteAttributeValue( &wrReq->handleValPair, 0, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
                                DBG("Frequency: %d", freq);
                            }
                            else
                            {
                                ERR("Invalid params to setup: %d, min: %d max: %d", freq, min, max);
                                result = FALSE;
                            }
                        }
                    }
                    else if ( CYBLE_TUNERCONFIGURATION_SLAVEADDR_CHAR_HANDLE == attr )
                    {
                        CyBle_GattsWriteAttributeValue( &wrReq->handleValPair, 0, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
                        DBG("SlaveAddress: 0x%x", wrReq->handleValPair.value.val[0]);
                    }
                    else if ( CYBLE_TUNERCONFIGURATION_TUNERTYPE_CHAR_HANDLE == attr )
                    {
                        CyBle_GattsWriteAttributeValue( &wrReq->handleValPair, 0, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
                        DBG("Tuner type: %d", wrReq->handleValPair.value.val[0]);
                    }
                    else if ( CYBLE_TUNERCONFIGURATION_DEMODULATORBW_CHAR_HANDLE == attr )
                    {
                        CyBle_GattsWriteAttributeValue( &wrReq->handleValPair, 0, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
                        DBG("Demodulator BW: %d", wrReq->handleValPair.value.val[0]);
                    }
                    else if ( CYBLE_TUNERCONFIGURATION_UPDATETIMEOUT_CHAR_HANDLE == attr )
                    {
                        DBG("Update timeout: %d ms", wrReq->handleValPair.value.val[1]<<8|wrReq->handleValPair.value.val[0]);
                        InterruptTimer_WriteCounter(0);
                        InterruptTimer_WritePeriod( wrReq->handleValPair.value.val[1]<<8|wrReq->handleValPair.value.val[0] );
                        InterruptTimer_Stop();
                        InterruptTimer_Start();
                        
                        DBG("Timeout period: %d", InterruptTimer_ReadPeriod());
                    }
                    else if (( CYBLE_TUNERINFORMATION_LOCK_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == attr ) 
                            || ( CYBLE_TUNERINFORMATION_SIGNALSTRENGTH_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == attr )
                            || ( cyBle_bass[CYBLE_BAS_BATTERY_LEVEL_CCCD].cccdHandle == attr ))
                    {
                        /* Allow to client change any settings in CCCD (Client Configuration Characteristics Descriptor) */
                        if( cyBle_bass[CYBLE_BAS_BATTERY_LEVEL_CCCD].cccdHandle == attr )
                        {
                            if( wrReq->handleValPair.value.val[0] & NOTIFY_MASK )
                            {
                                notifyBattery = TRUE;
                            }
                            else
                            {
                                notifyBattery = FALSE;
                            }
                        }

                        if( CYBLE_TUNERINFORMATION_SIGNALSTRENGTH_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == attr )
                        {
                            if( wrReq->handleValPair.value.val[0] & NOTIFY_MASK )
                            {
                                notifySignalStrength = TRUE;
                            }
                            else
                            {
                                notifySignalStrength = FALSE;
                            }
                        }
                        CyBle_GattsWriteAttributeValue( &wrReq->handleValPair, 0, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
                    }
                    else
                    {
                        ERR("Unknown attribute handle: 0x%x", wrReq->handleValPair.attrHandle);
                    }
                }
                else
                {
                    ERR("Invalid connection handle");
                }
            }

            if( FALSE == result )
            {
                errCode.attrHandle = wrReq->handleValPair.attrHandle;
                errCode.opcode = (uint8) CYBLE_GATT_WRITE_REQ;
                errCode.errorCode = CYBLE_GATT_ERR_OUT_OF_RANGE;
                ble_result = CyBle_GattsErrorRsp( btConHandle.handle, &errCode );
                if( CYBLE_ERROR_OK != ble_result )
                {
                    ERR("Failed to send error responce");
                }
                else
                {
                    DBG("Success to send error responce");
                }
            }
            else
            {
                CyBle_GattsWriteRsp( btConHandle.handle );
            }

            break;
        }

        case CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
        {
            CYBLE_GATTS_CHAR_VAL_READ_REQ_T *readReq = NULL;
            
            readReq = (CYBLE_GATTS_CHAR_VAL_READ_REQ_T *)eventParam;
            if( NULL == readReq )
            {
                ERR("Invalid parameter");
                break;
            }
            
            DBG("GATT error code: 0x%x",readReq->gattErrorCode);
            break;
        }
#endif /* 1 */

        case CYBLE_EVT_GAP_AUTH_FAILED:
        {
            CYBLE_GAP_AUTH_FAILED_REASON_T gapAuthFailure;
            
            gapAuthFailure = *(CYBLE_GAP_AUTH_FAILED_REASON_T*)eventParam;
            ERR("GAP authentication failure: 0x%x", gapAuthFailure);
            break;
        }

        case CYBLE_EVT_GAP_AUTH_REQ:
        {
            ble_result = CyBle_GappAuthReqReply( btConHandle.handle.bdHandle, (CYBLE_GAP_AUTH_INFO_T*)eventParam );
            DBG("retVal: %d", ble_result);
            break;
        }

        case CYBLE_EVT_TIMEOUT:
        {
            DBG("Timeout reason: %d, BLE state: %d", *(CYBLE_TO_REASON_CODE_T*)eventParam, CyBle_GetState());
            break;
        }

        default:
            break;
    }
}


static CY_ISR( ResetIsrHandler )
{
    ERR("Reboot now");
    CyBle_Stop();

    CyDelay(500);
    CySoftwareReset();

    Pin_SW2_ClearInterrupt();
}


static CY_ISR( TimeoutIsr_Handler )
{
    uint8_t signalStrength = 0;
    uint8_t batteryLevel = 0;
    
    DBG("TimeoutIsr fired");

    TimeoutIsr_ClearPending();
    InterruptTimer_Stop();
    InterruptTimer_Start();
    
    if( TRUE == btConHandle.connected )
    {
        GetData1Byte( CYBLE_TUNERINFORMATION_SIGNALSTRENGTH_CHAR_HANDLE, &signalStrength );
        signalStrength++;
        SetData1Byte( CYBLE_TUNERINFORMATION_SIGNALSTRENGTH_CHAR_HANDLE, notifySignalStrength, signalStrength );

        GetData1Byte( cyBle_bass[0].batteryLevelHandle, &batteryLevel );
        batteryLevel++;
        if( 100 < batteryLevel )
            batteryLevel = 0;
        SetData1Byte( cyBle_bass[0].batteryLevelHandle, notifyBattery, batteryLevel );
    }
}


static uint8 GetUint16Limits( uint16_t limitHandle, uint16_t *max, uint16_t *min )
{
    uint8 result = TRUE;
    const uint8 limitsLen = 2;
    uint16 loop = limitsLen * 2 - 1;
    uint16 value[limitsLen*2];
    CYBLE_GATT_HANDLE_VALUE_PAIR_T handleValPair;
    CYBLE_GATT_ERR_CODE_T bleErr = CYBLE_GATT_ERR_NONE;

    if(( NULL == max ) || ( NULL == min ))
    {
        return FALSE;
    }

    do
    {
        handleValPair.attrHandle = limitHandle;
        handleValPair.value.len = limitsLen * 2; // Multiply on 2 because returned values are ORed values: MIN | MAX
        handleValPair.value.val = (uint8_t*)value;
        bleErr = CyBle_GattsReadAttributeValue( &handleValPair, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
        if( CYBLE_GATT_ERR_NONE != bleErr )
        {
            ERR("Failed to read value from DB: %d", bleErr );
            result = FALSE;
            break;
        }
        
        for( ; loop <= limitsLen * 2; loop-- )
        {
            if( loop >= limitsLen )
            {
                *max <<= 8;
                *max |= handleValPair.value.val[loop];
            }
            else
            {
                *min <<= 8;
                *min |= handleValPair.value.val[loop];
            }
        }
    } while( 0 );

    return result;
}


static uint8 GetData2Bytes( CYBLE_GATT_DB_ATTR_HANDLE_T attribute, uint16_t *data )
{
    uint8 result = FALSE;
    const uint8 dataLen = 2;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T handleValPair;
    CYBLE_GATT_ERR_CODE_T bleErr = CYBLE_GATT_ERR_NONE;

    if( NULL == data )
    {
        ERR("Invalid input param(s): %s", "data");
        return FALSE;
    }

    handleValPair.attrHandle = attribute;
    handleValPair.value.len = dataLen;
    handleValPair.value.val = (uint8_t*)data;
    bleErr = CyBle_GattsReadAttributeValue( &handleValPair, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
    if( CYBLE_GATT_ERR_NONE != bleErr )
    {
        ERR("Failed to read value from DB: %d", bleErr );
        result = FALSE;
    }

    return result;
}


static uint8 GetData1Byte( CYBLE_GATT_DB_ATTR_HANDLE_T attribute, uint8_t *data )
{
    uint8 result = FALSE;
    const uint8 dataLen = 1;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T handleValPair;
    CYBLE_GATT_ERR_CODE_T bleErr = CYBLE_GATT_ERR_NONE;

    if( NULL == data )
    {
        ERR("Invalid input param(s): %s", "data");
        return FALSE;
    }

    handleValPair.attrHandle = attribute;
    handleValPair.value.len = dataLen;
    handleValPair.value.val = data;
    bleErr = CyBle_GattsReadAttributeValue( &handleValPair, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
    if( CYBLE_GATT_ERR_NONE != bleErr )
    {
        ERR("Failed to read value from DB: %d", bleErr );
        result = FALSE;
    }

    return result;
}


static uint8 SetData2Bytes( CYBLE_GATT_DB_ATTR_HANDLE_T attribute, uint16_t data )
{
    uint8 result = TRUE;
    const uint8 dataLen = 2;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T handleValPair;
    CYBLE_GATT_ERR_CODE_T bleErr = CYBLE_GATT_ERR_NONE;

    handleValPair.attrHandle = attribute;
    handleValPair.value.len = dataLen;
    handleValPair.value.val = (uint8_t*)&data;
    bleErr = CyBle_GattsWriteAttributeValue( &handleValPair, 0, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
    if( CYBLE_GATT_ERR_NONE != bleErr )
    {
        ERR("Failed to write value to DB: %d", bleErr );
        result = FALSE;
    }

    return result;
}


static uint8 SetData1Byte( CYBLE_GATT_DB_ATTR_HANDLE_T attribute, uint8_t notify, uint8_t data )
{
    uint8 result = TRUE;
    const uint8 dataLen = 1;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T handleValPair;
    CYBLE_GATT_ERR_CODE_T bleErr = CYBLE_GATT_ERR_NONE;

    handleValPair.attrHandle = attribute;
    handleValPair.value.len = dataLen;
    handleValPair.value.val = &data;

    bleErr = CyBle_GattsWriteAttributeValue( &handleValPair, 0, &btConHandle.handle, CYBLE_GATT_DB_LOCALLY_INITIATED );
    if( CYBLE_GATT_ERR_NONE != bleErr )
    {
        ERR("Failed to write value to DB: %d", bleErr );
        result = FALSE;
    }
    else
    {
        if( TRUE == notify )
        {
            CyBle_GattsNotification( btConHandle.handle, &handleValPair );
        }
    }

    return result;
}
/* [] END OF FILE */
