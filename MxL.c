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

#include <string.h>
#include "debug.h"
#include "MxL.h"
#include "mxl_eagle_oem_debug.h"


typedef enum __MXL_ERROR_STAGE_E__
{
    MXL_ERROR_STAGE_NONE = 0
    ,MXL_ERROR_STAGE_DEVICE_INIT_A1_CFG_DRV_INIT
    ,MXL_ERROR_STAGE_DEVICE_INIT_A2_CFG_DEV_RESET
    ,MXL_ERROR_STAGE_DEVICE_INIT_A3_CFG_POWER_REGULATORS
    ,MXL_ERROR_STAGE_DEVICE_INIT_A4_CFG_DEV_XTAL
    ,MXL_ERROR_STAGE_DEVICE_INIT_A5_CFG_DEV_LOOPTHROUGHT
    ,MXL_ERROR_STAGE_DEVICE_INIT_A6_CFG_DEV_FW_DOWNLOAD
    ,MXL_ERROR_STAGE_DEVICE_INIT_A7_CFG_DEV_DEMOD_TYPE
    ,MXL_ERROR_STAGE_DEVICE_INIT_A8_CFG_DEV_POWER_MODE
    ,MXL_ERROR_STAGE_DEVICE_INIT_A9_CFG_DEV_MPEG_OUT_PARAMS
    ,MXL_ERROR_STAGE_DEVICE_INIT_A10_CFG_DEV_INTRRUPT_MASK
    ,MXL_ERROR_STAGE_DEVICE_INIT_A11_DEV_GPIO_DIRECTION
    ,MXL_ERROR_STAGE_DEVICE_INIT_A12_CFG_DEV_GPO_LEVEL
    ,MXL_ERROR_STAGE_DEVICE_INIT_A13_CFG_DEV_IO_MUX
    ,MXL_ERROR_STAGE_DEVICE_INIT_B1_CFG_TUNER_CHANNEL_TUNE
    ,MXL_ERROR_STAGE_DEVICE_INIT_C1_CGF_ATSC_DEMOD_INIT
    ,MXL_ERROR_STAGE_DEVICE_INIT_C2_CFG_ATSC_DEMOD_ACQUIRE_CARRIER
    ,MXL_ERROR_STAGE_DEVICE_INIT_D1_CGF_QAM_DEMOD_INIT
    ,MXL_ERROR_STAGE_DEVICE_INIT_D2_CFG_QAM_DEMOD_RESTART
    ,MXL_ERROR_STAGE_DEVICE_INIT_E1_CFG_OOB_DEMOD_PARAMS
    ,MXL_ERROR_STAGE_DEVICE_INIT_E2_CFG_OOB_DEMOD_RESTART
    ,MXL_ERROR_STAGE_DEVICE_INIT_MAX
} MXL_ERROR_STAGE_E;


static UINT32 FirmwareUpdateCallback( UINT8 devId, UINT32 callbackType, void* pCallbackPayload );

static MXL_ERROR_STAGE_E g_error = MXL_ERROR_STAGE_DEVICE_INIT_MAX;

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_DeviceInit
*
* AUTHOR        : Oleksandr Boyko
* 
* DATE CREATED  : 
*
* \brief          This API make intialization of MxL device according to specification. Please reffer to:
                    "MxLWare API for MxL248 and MxL691_MxL692 User Guide_043UGR05.pdf" Figure 4: DeviceInit
                    and Figure 5: Tune Channel
* 
* \param[in]      devId       The MxL device id
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxL_DeviceInit( uint8_t devId  )
{
    MXL_STATUS_E retCode = MXL_FAILURE;
    UINT32 slaveAddr = 0x60;    // Should be taken from UI?
    MXL_EAGLE_TUNER_CHANNEL_PARAMS_T channelTuneConfig;
    UINT32 frequencyHz = 581000000;
    UINT8 bandWidth = MXL_EAGLE_TUNER_BW_7MHz;
    MXL_EAGLE_TUNER_LOCK_STATUS_T tunerStatus;
    MXL_EAGLE_DEV_XTAL_T xtalSettings;
    UINT8 *fwBuffer = NULL;
    UINT8 fwBufLen = 0;
    MXL_EAGLE_DEMOD_TYPE_E demodType = MXL_EAGLE_DEMOD_TYPE_ATSC; // Default for now
    MXL_EAGLE_POWER_MODE_E powerMode = MXL_EAGLE_POWER_MODE_ACTIVE; // Default for now
    
    
    memset((void*)&channelTuneConfig, 0, sizeof( channelTuneConfig ));
    memset((void*)&tunerStatus, 0, sizeof( tunerStatus ));
    memset((void*)&xtalSettings, 0, sizeof( xtalSettings ));
    
    do
    {
        retCode = MxLWare_EAGLE_API_CfgDrvInit( devId, &slaveAddr, MXL_EAGLE_DEVICE_691);
        if( MXL_SUCCESS != retCode )
        {
            g_error = MXL_ERROR_STAGE_DEVICE_INIT_A1_CFG_DRV_INIT;
            ERR("Failed to config drivers for device %d (%d)", devId, retCode);
            break;
        }
        
        retCode = MxLWare_EAGLE_API_CfgDevReset( devId );
        if( MXL_SUCCESS != retCode )
        {
            g_error = MXL_ERROR_STAGE_DEVICE_INIT_A2_CFG_DEV_RESET;
            ERR("Failed to config drivers for device %d (%d)", devId, retCode);
            break;
        }
        
        retCode = MxLWare_EAGLE_API_CfgDevPowerRegulators( devId, MXL_EAGLE_POWER_SUPPLY_SOURCE_DUAL );
        if( MXL_SUCCESS != retCode )
        {
            g_error = MXL_ERROR_STAGE_DEVICE_INIT_A3_CFG_POWER_REGULATORS;
            ERR("Failed to config drivers for device %d (%d)", devId, retCode);
            break;
        }
        
        xtalSettings.xtalCap = 26;
        xtalSettings.clkOutEnable = MXL_OFF;
        xtalSettings.clkOutDivEnable = MXL_OFF;
        xtalSettings.xtalCalibrationEnable = MXL_OFF;
        xtalSettings.xtalSharingEnable = MXL_OFF;
        retCode = MxLWare_EAGLE_API_CfgDevXtal( devId, &xtalSettings );
        if( MXL_SUCCESS != retCode )
        {
            g_error = MXL_ERROR_STAGE_DEVICE_INIT_A4_CFG_DEV_XTAL;
            ERR("Failed to config drivers for device %d (%d)", devId, retCode);
            break;
        }
        
        retCode = MxLWare_EAGLE_API_CfgDevLoopthrough( devId, MXL_OFF );
        if( MXL_SUCCESS != retCode )
        {
            g_error = MXL_ERROR_STAGE_DEVICE_INIT_A5_CFG_DEV_LOOPTHROUGHT;
            ERR("Failed to config drivers for device %d (%d)", devId, retCode);
            break;
        }
        
        retCode = MxLWare_EAGLE_API_CfgDevFwDownload( devId, fwBuffer, fwBufLen, FirmwareUpdateCallback );
        if( MXL_SUCCESS != retCode )
        {
            g_error = MXL_ERROR_STAGE_DEVICE_INIT_A6_CFG_DEV_FW_DOWNLOAD;
            ERR("Failed to config drivers for device %d (%d)", devId, retCode);
            break;
        }
        
        retCode = MxLWare_EAGLE_API_CfgDevDemodType( devId, demodType );
        if( MXL_SUCCESS != retCode )
        {
            g_error = MXL_ERROR_STAGE_DEVICE_INIT_A7_CFG_DEV_DEMOD_TYPE;
            ERR("Failed to config drivers for device %d (%d)", devId, retCode);
            break;
        }
        
        retCode = MxLWare_EAGLE_API_CfgDevPowerMode( devId, powerMode );
        if( MXL_SUCCESS != retCode )
        {
            g_error = MXL_ERROR_STAGE_DEVICE_INIT_A8_CFG_DEV_POWER_MODE;
            ERR("Failed to config drivers for device %d (%d)", devId, retCode);
            break;
        }
        
        /* Steps A9-A13 is optional - so not supported for now */
        
        
        // MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_VIEW,               //!< Normal "view" mode - optimal performance
        // MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_SCAN,               //!< Fast "scan" mode - faster tune time
        channelTuneConfig.tuneMode = MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_VIEW;  
        channelTuneConfig.freqInHz = frequencyHz; /* Should be passed from UI, isn't it? */
        //  MXL_EAGLE_TUNER_BW_6MHz,
        //  MXL_EAGLE_TUNER_BW_7MHz,
        //  MXL_EAGLE_TUNER_BW_8MHz,
        channelTuneConfig.bandWidth = MXL_EAGLE_TUNER_BW_7MHz; /* Should be passed from UI? */

        retCode = MxLWare_EAGLE_API_CfgTunerChannelTune( devId, &channelTuneConfig);
        if( MXL_SUCCESS != retCode )
        {
            g_error = MXL_ERROR_STAGE_DEVICE_INIT_B1_CFG_TUNER_CHANNEL_TUNE;
            ERR("MxLWare_EAGLE_API_CfgTunerChannelTune() failed: %s to config tuner to tune to channel: %dMHz BW: %s", MxL_EAGLE_Oem_Debug_StatusToString( retCode ), frequencyHz/1000000, (MXL_EAGLE_TUNER_BW_6MHz==bandWidth)?"6MHz":((MXL_EAGLE_TUNER_BW_7MHz==bandWidth)?"7MHz":"8MHz"));
            break;
        }
    } while ( 0 );
    
    return retCode;
} /* MxL_DeviceInit() */

    
/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_ConfigDemod
*
* AUTHOR        : Oleksandr Boyko
* 
* DATE CREATED  : 
*
* \brief          This API makes configuration of demodulator, please reffer to:
                    "MxLWare API for MxL248 and MxL691_MxL692 User Guide_043UGR05.pdf" Figure 6: ATSC Demod Configuration
                    and Figure 6: QAM Demod Configuration
* 
* \param[in]      devId       The MxL device id
* \param[in]      demodType   Demodulator type requested to configure
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxL_ConfigDemod( uint8_t devId, MXL_DEMOD_TYPE_E demodType )
{
    MXL_STATUS_E ret = MXL_SUCCESS;
    MXL_EAGLE_QAM_DEMOD_PARAMS_T qamParams;
   
    
    switch( demodType )
    {
        case MXL_DEMOD_TYPE_QAM:
            qamParams.annexType = MXL_EAGLE_QAM_DEMOD_ANNEX_B;
            qamParams.iqFlip = MXL_EAGLE_DEMOD_IQ_AUTO;
            qamParams.qamType = MXL_EAGLE_QAM_DEMOD_AUTO; // Set as default for now
            /* TODO: Require for additional investigations about further params */
            ret = MxLWare_EAGLE_API_CfgQamDemodParams( devId, &qamParams );
            if( MXL_SUCCESS != ret )
            {
                ERR("MxLWare_EAGLE_API_CfgQamDemodParams() failed");
                break;
            }
            
            ret = MxLWare_EAGLE_API_CfgQamDemodRestart( devId );
            if( MXL_SUCCESS != ret )
            {
                ERR("MxLWare_EAGLE_API_CfgQamDemodRestart() failed");
            }
            break;
            
        case MXL_DEMOD_TYPE_ATSC:
            ret = MxLWare_EAGLE_API_CfgAtscDemodInit( devId );
            if( MXL_SUCCESS != ret )
            {
                ERR("MxLWare_EAGLE_API_CfgAtscDemodInit() failed");
                break;
            }
            
            ret = MxLWare_EAGLE_API_CfgAtscDemodAcquireCarrier( devId );
            if( MXL_SUCCESS != ret )
            {
                ERR("MxLWare_EAGLE_API_CfgAtscDemodAcquireCarrier() failed");                
            }
            break;
            
        case MXL_DEMOD_TYPE_MAX:
        default:
            ret = MXL_INVALID_PARAMETER;
            break;
    }
    
    return ret;
} /* MxL_ConfigDemod() */


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_Debug_StatusToString
*
* AUTHOR        : Oleksandr Boyko
* 
* DATE CREATED  : 
*
* \brief          This API returns stringified error state
*
* \returns        Stringified error code
*
*-------------------------------------------------------------------------------------*/
char* MxL_Debug_StatusToString( void )
{
    char* ret = "";
    
    switch ( g_error )
    {
        case MXL_ERROR_STAGE_NONE:
            ret = "MXL_ERROR_STAGE_NONE";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A1_CFG_DRV_INIT:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A1_CFG_DRV_INIT";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A2_CFG_DEV_RESET:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A2_CFG_DEV_RESET";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A3_CFG_POWER_REGULATORS:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A3_CFG_POWER_REGULATORS";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A4_CFG_DEV_XTAL:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A4_CFG_DEV_XTAL";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A5_CFG_DEV_LOOPTHROUGHT:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A5_CFG_DEV_LOOPTHROUGHT";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A6_CFG_DEV_FW_DOWNLOAD:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A6_CFG_DEV_FW_DOWNLOAD";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A7_CFG_DEV_DEMOD_TYPE:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A7_CFG_DEV_DEMOD_TYPE";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A8_CFG_DEV_POWER_MODE:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A8_CFG_DEV_POWER_MODE";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A9_CFG_DEV_MPEG_OUT_PARAMS:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A9_CFG_DEV_MPEG_OUT_PARAMS";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A10_CFG_DEV_INTRRUPT_MASK:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A10_CFG_DEV_INTRRUPT_MASK";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A11_DEV_GPIO_DIRECTION:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A11_DEV_GPIO_DIRECTION";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A12_CFG_DEV_GPO_LEVEL:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A12_CFG_DEV_GPO_LEVEL";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_A13_CFG_DEV_IO_MUX:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_A13_CFG_DEV_IO_MUX";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_B1_CFG_TUNER_CHANNEL_TUNE:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_B1_CFG_TUNER_CHANNEL_TUNE";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_C1_CGF_ATSC_DEMOD_INIT:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_C1_CGF_ATSC_DEMOD_INIT";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_C2_CFG_ATSC_DEMOD_ACQUIRE_CARRIER:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_C2_CFG_ATSC_DEMOD_ACQUIRE_CARRIER";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_D1_CGF_QAM_DEMOD_INIT:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_D1_CGF_QAM_DEMOD_INIT";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_D2_CFG_QAM_DEMOD_RESTART:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_D2_CFG_QAM_DEMOD_RESTART";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_E1_CFG_OOB_DEMOD_PARAMS:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_E1_CFG_OOB_DEMOD_PARAMS";
            break;
        case MXL_ERROR_STAGE_DEVICE_INIT_E2_CFG_OOB_DEMOD_RESTART:
            ret = "MXL_ERROR_STAGE_DEVICE_INIT_E2_CFG_OOB_DEMOD_RESTART";
            break;
        
        case MXL_ERROR_STAGE_DEVICE_INIT_MAX:
        default:
            ret = "Unhandled error type";
            break;
    }
    
    return ret;
} /* MxL_Debug_StatusToString() */


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_GetTunerLockStatus
*
* AUTHOR        : Oleksandr Boyko
*
* DATE CREATED  :
*
* \brief          This API gets lock status of specified tuner
*
* \param[in]      devId       The MxL device id
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxL_GetTunerLockStatus( uint8_t devId )
{
    MXL_STATUS_E ret = MXL_SUCCESS;
    MXL_EAGLE_TUNER_AGC_STATUS_T agcStatus;
    MXL_EAGLE_TUNER_LOCK_STATUS_T lockStatus;

    memset((void*)&agcStatus, 0, sizeof( agcStatus ));
    memset((void*)&lockStatus, 0, sizeof( lockStatus ));

    ret = MxLWare_EAGLE_API_ReqTunerAgcStatus( devId, &agcStatus );
    if( MXL_SUCCESS == ret )
    {
        if( MXL_TRUE == agcStatus.isLocked )
        {
            DBG("Tuner locked. Rx power: %f[dB], AGC gain: %f[dB]", agcStatus.rxPowerDbHundredths/100, agcStatus.rawAgcGain/(2<<6));
        }

        ret = MxLWare_EAGLE_API_ReqTunerLockStatus( devId, &lockStatus );
        if( MXL_SUCCESS == ret )
        {
            DBG("RF synthesizer Lock: %d, Ref synthesizer Lock: %d", lockStatus.isRfPllLocked, lockStatus.isRefPllLocked);
        }
    }

    return ret;
} /* MxL_GetTunerLockStatus() */


/* Local Functions */


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : FirmwareUpdateCallback
*
* AUTHOR        : Oleksandr Boyko
* 
* DATE CREATED  : 
*
* \brief          This API periodically called to report the firmware download progress
*
* \returns        MXL_SUCCESS, MXL_INVALID_PARAMETER
*
*-------------------------------------------------------------------------------------*/
static UINT32 FirmwareUpdateCallback( UINT8 devId, UINT32 callbackType, void* pCallbackPayload )
{
    MXL_EAGLE_FW_DOWNLOAD_CB_PAYLOAD_T *payload = (MXL_EAGLE_FW_DOWNLOAD_CB_PAYLOAD_T*)pCallbackPayload;
    
    if( NULL == payload )
    {
        return MXL_INVALID_PARAMETER;
    }
    
    if( MXL_EAGLE_CB_FW_DOWNLOAD == callbackType )
    {
        DBG("Firmware uploaded: %d bytes, %d%", payload->downloadedLen, (payload->downloadedLen * 100)/payload->totalLen);
    }
    (void)devId;
    
    return MXL_SUCCESS;
} /* FirmwareUpdateCallback() */
/* [] END OF FILE */
