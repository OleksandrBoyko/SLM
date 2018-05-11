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


#include <stdarg.h>

#include "debug.h"
#include <UART.h>
#include <UART_SPI_UART.h>
#include <BLE_Stack.h>
#include <BLE_eventHandler.h>

#define BUFF_LENGTH (128)

#ifdef DEBUG_CONSOLE
void DebugConsole_Init( void )
{
    UART_Start();

    UART_UartPutCRLF(0xc); // Reset console
}
#endif

#ifdef DEBUG_CONSOLE
void PRINTF(const char* fmt, ...)
{
    char buffer[BUFF_LENGTH] = {'\0'};
    va_list ap;
    
    va_start(ap,fmt);
    vsnprintf( buffer, BUFF_LENGTH-1, fmt, ap );
    va_end(ap);
        
    UART_UartPutString(buffer);
}
#endif


#ifdef DEBUG_CONSOLE
char* StringifyBleEvent( uint32 event )
{
    char* ret = "";

    switch( event )
    {
        case CYBLE_EVT_HOST_INVALID:
            ret = "CYBLE_EVT_HOST_INVALID";
            break;

        case CYBLE_EVT_STACK_ON:
            ret = "CYBLE_EVT_STACK_ON";
            break;

        case CYBLE_EVT_TIMEOUT:
            ret = "CYBLE_EVT_TIMEOUT";
            break;

        case CYBLE_EVT_HARDWARE_ERROR:
            ret = "CYBLE_EVT_HARDWARE_ERROR";
            break;

        case CYBLE_EVT_HCI_STATUS:
            ret = "CYBLE_EVT_HCI_STATUS";
            break;

        case CYBLE_EVT_STACK_BUSY_STATUS:
            ret = "CYBLE_EVT_STACK_BUSY_STATUS";
            break;

        case CYBLE_EVT_MEMORY_REQUEST:
            ret = "CYBLE_EVT_MEMORY_REQUEST";
            break;

        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            ret = "CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT";
            break;

        case CYBLE_EVT_GAP_AUTH_REQ:
            ret = "CYBLE_EVT_GAP_AUTH_REQ";
            break;

        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            ret = "CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST";
            break;

        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            ret = "CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST";
            break;

        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            ret = "CYBLE_EVT_GAP_AUTH_COMPLETE";
            break;

        case CYBLE_EVT_GAP_AUTH_FAILED:
            ret = "CYBLE_EVT_GAP_AUTH_FAILED";
            break;

        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            ret = "CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP";
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            ret = "CYBLE_EVT_GAP_DEVICE_CONNECTED";
            break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            ret = "CYBLE_EVT_GAP_DEVICE_DISCONNECTED";
            break;

        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            ret = "CYBLE_EVT_GAP_ENCRYPT_CHANGE";
            break;

        case CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE:
            ret = "CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE";
            break;

        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            ret = "CYBLE_EVT_GAPC_SCAN_START_STOP";
            break;

        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
            ret = "CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT";
            break;

        case CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST:
            ret = "CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST";
            break;

        case CYBLE_EVT_GAP_KEYPRESS_NOTIFICATION:
            ret = "CYBLE_EVT_GAP_KEYPRESS_NOTIFICATION";
            break;

        case CYBLE_EVT_GAP_OOB_GENERATED_NOTIFICATION:
            ret = "CYBLE_EVT_GAP_OOB_GENERATED_NOTIFICATION";
            break;

        case CYBLE_EVT_GAP_DATA_LENGTH_CHANGE:
            ret = "CYBLE_EVT_GAP_DATA_LENGTH_CHANGE";
            break;

        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            ret = "CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE";
            break;

        case CYBLE_EVT_GAPC_DIRECT_ADV_REPORT:
            ret = "CYBLE_EVT_GAPC_DIRECT_ADV_REPORT";
            break;

        case CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO:
            ret = "CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO";
            break;

        case CYBLE_EVT_GAP_CONN_ESTB:
            ret = "CYBLE_EVT_GAP_CONN_ESTB";
            break;

        case CYBLE_EVT_GAP_SCAN_REQ_RECVD:
            ret = "CYBLE_EVT_GAP_SCAN_REQ_RECVD";
            break;

        case CYBLE_EVT_GAP_AUTH_REQ_REPLY_ERR:
            ret = "CYBLE_EVT_GAP_AUTH_REQ_REPLY_ERR";
            break;

        case CYBLE_EVT_GAP_SMP_LOC_P256_KEYS_GEN_AND_SET_COMPLETE:
            ret = "CYBLE_EVT_GAP_SMP_LOC_P256_KEYS_GEN_AND_SET_COMPLETE";
            break;

        case CYBLE_EVT_GATTC_ERROR_RSP:
            ret = "CYBLE_EVT_GATTC_ERROR_RSP";
            break;

        case CYBLE_EVT_GATT_CONNECT_IND:
            ret = "CYBLE_EVT_GATT_CONNECT_IND";
            break;

        case CYBLE_EVT_GATT_DISCONNECT_IND:
            ret = "CYBLE_EVT_GATT_DISCONNECT_IND";
            break;

        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            ret = "CYBLE_EVT_GATTS_XCNHG_MTU_REQ";
            break;

        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            ret = "CYBLE_EVT_GATTC_XCHNG_MTU_RSP";
            break;

        case CYBLE_EVT_GATTC_READ_BY_GROUP_TYPE_RSP:
            ret = "CYBLE_EVT_GATTC_READ_BY_GROUP_TYPE_RSP";
            break;

        case CYBLE_EVT_GATTC_READ_BY_TYPE_RSP:
            ret = "CYBLE_EVT_GATTC_READ_BY_TYPE_RSP";
            break;

        case CYBLE_EVT_GATTC_FIND_INFO_RSP:
            ret = "CYBLE_EVT_GATTC_FIND_INFO_RSP";
            break;

        case CYBLE_EVT_GATTC_FIND_BY_TYPE_VALUE_RSP:
            ret = "CYBLE_EVT_GATTC_FIND_BY_TYPE_VALUE_RSP";
            break;

        case CYBLE_EVT_GATTC_READ_RSP:
            ret = "CYBLE_EVT_GATTC_READ_RSP";
            break;

        case CYBLE_EVT_GATTC_READ_BLOB_RSP:
            ret = "CYBLE_EVT_GATTC_READ_BLOB_RSP";
            break;

        case CYBLE_EVT_GATTC_READ_MULTI_RSP:
            ret = "CYBLE_EVT_GATTC_READ_MULTI_RSP";
            break;

        case CYBLE_EVT_GATTS_WRITE_REQ:
            ret = "CYBLE_EVT_GATTS_WRITE_REQ";
            break;

        case CYBLE_EVT_GATTC_WRITE_RSP:
            ret = "CYBLE_EVT_GATTC_WRITE_RSP";
            break;

        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
            ret = "CYBLE_EVT_GATTS_WRITE_CMD_REQ";
            break;

        case CYBLE_EVT_GATTS_PREP_WRITE_REQ:
            ret = "CYBLE_EVT_GATTS_PREP_WRITE_REQ";
            break;

        case CYBLE_EVT_GATTS_EXEC_WRITE_REQ:
            ret = "CYBLE_EVT_GATTS_EXEC_WRITE_REQ";
            break;

        case CYBLE_EVT_GATTC_EXEC_WRITE_RSP:
            ret = "CYBLE_EVT_GATTC_EXEC_WRITE_RSP";
            break;

        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            ret = "CYBLE_EVT_GATTC_HANDLE_VALUE_NTF";
            break;

        case CYBLE_EVT_GATTC_HANDLE_VALUE_IND:
            ret = "CYBLE_EVT_GATTC_HANDLE_VALUE_IND";
            break;

        case CYBLE_EVT_GATTS_HANDLE_VALUE_CNF:
            ret = "CYBLE_EVT_GATTS_HANDLE_VALUE_CNF";
            break;

        case CYBLE_EVT_GATTS_DATA_SIGNED_CMD_REQ:
            ret = "CYBLE_EVT_GATTS_DATA_SIGNED_CMD_REQ";
            break;

        case CYBLE_EVT_GATTC_STOP_CMD_COMPLETE:
            ret = "CYBLE_EVT_GATTC_STOP_CMD_COMPLETE";
            break;

        case CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            ret = "CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ";
            break;

        case CYBLE_EVT_GATTC_LONG_PROCEDURE_END:
            ret = "CYBLE_EVT_GATTC_LONG_PROCEDURE_END";
            break;

        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ:
            ret = "CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ";
            break;

        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP:
            ret = "CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP";
            break;

        case CYBLE_EVT_L2CAP_COMMAND_REJ:
            ret = "CYBLE_EVT_L2CAP_COMMAND_REJ";
            break;

        case CYBLE_EVT_L2CAP_CBFC_CONN_IND:
            ret = "CYBLE_EVT_L2CAP_CBFC_CONN_IND";
            break;

        case CYBLE_EVT_L2CAP_CBFC_CONN_CNF:
            ret = "CYBLE_EVT_L2CAP_CBFC_CONN_CNF";
            break;

        case CYBLE_EVT_L2CAP_CBFC_DISCONN_IND:
            ret = "CYBLE_EVT_L2CAP_CBFC_DISCONN_IND";
            break;

        case CYBLE_EVT_L2CAP_CBFC_DISCONN_CNF:
            ret = "CYBLE_EVT_L2CAP_CBFC_DISCONN_CNF";
            break;

        case CYBLE_EVT_L2CAP_CBFC_DATA_READ:
            ret = "CYBLE_EVT_L2CAP_CBFC_DATA_READ";
            break;

        case CYBLE_EVT_L2CAP_CBFC_RX_CREDIT_IND:
            ret = "CYBLE_EVT_L2CAP_CBFC_RX_CREDIT_IND";
            break;

        case CYBLE_EVT_L2CAP_CBFC_TX_CREDIT_IND:
            ret = "CYBLE_EVT_L2CAP_CBFC_TX_CREDIT_IND";
            break;

        case CYBLE_EVT_L2CAP_CBFC_DATA_WRITE_IND:
            ret = "CYBLE_EVT_L2CAP_CBFC_DATA_WRITE_IND";
            break;

        case CYBLE_EVT_PENDING_FLASH_WRITE:
            ret = "CYBLE_EVT_PENDING_FLASH_WRITE";
            break;

        case CYBLE_EVT_LE_PING_AUTH_TIMEOUT:
            ret = "CYBLE_EVT_LE_PING_AUTH_TIMEOUT";
            break;

        case CYBLE_EVT_HCI_PKT:
            ret = "CYBLE_EVT_HCI_PKT";
            break;

        case CYBLE_EVT_FLASH_CORRUPT:
            ret = "CYBLE_EVT_FLASH_CORRUPT";
            break;

        case CYBLE_EVT_MAX:
            ret = "CYBLE_EVT_MAX";
            break;

        case CYBLE_AUTHENTICATION_REQUEST_REPLY:
            ret = "CYBLE_AUTHENTICATION_REQUEST_REPLY";
            break;

        case CYBLE_PASSKEY_ENTRY_REQUEST_REPLY:
            ret = "CYBLE_PASSKEY_ENTRY_REQUEST_REPLY";
            break;

        case CYBLE_LONG_TERM_KEY_REQUEST_REPLY:
            ret = "CYBLE_LONG_TERM_KEY_REQUEST_REPLY";
            break;

        case CYBLE_KEY_EXCHANGE_INFO_REQUEST_REPLY:
            ret = "CYBLE_KEY_EXCHANGE_INFO_REQUEST_REPLY";
            break;

        case CYBLE_EVT_GATTS_INDICATION_ENABLED:
            ret = "CYBLE_EVT_GATTS_INDICATION_ENABLED";
            break;

        case CYBLE_EVT_GATTS_INDICATION_DISABLED:
            ret = "CYBLE_EVT_GATTS_INDICATION_DISABLED";
            break;

        case CYBLE_EVT_GATTC_INDICATION:
            ret = "CYBLE_EVT_GATTC_INDICATION";
            break;

        case CYBLE_EVT_GATTC_SRVC_DISCOVERY_FAILED:
            ret = "CYBLE_EVT_GATTC_SRVC_DISCOVERY_FAILED";
            break;

        case CYBLE_EVT_GATTC_INCL_DISCOVERY_FAILED:
            ret = "CYBLE_EVT_GATTC_INCL_DISCOVERY_FAILED";
            break;

        case CYBLE_EVT_GATTC_CHAR_DISCOVERY_FAILED:
            ret = "CYBLE_EVT_GATTC_CHAR_DISCOVERY_FAILED";
            break;

        case CYBLE_EVT_GATTC_DESCR_DISCOVERY_FAILED:
            ret = "CYBLE_EVT_GATTC_DESCR_DISCOVERY_FAILED";
            break;

        default:
            ret = "Unhandled event";
            break;
    }

    return ret;
}
#endif /* DEBUG_CONSOLE */

/* [] END OF FILE */
