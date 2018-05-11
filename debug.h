/* ========================================
 *
 * Copyright Dish Ukraine LLC, 2018
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#if !defined __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cytypes.h>

/*
To use this module header file please do next:

Place on schematic UART (SCB mode) with name "UART" and configure port 
in addition connect your serial properly: 
- P0[4:5]
- P1[4:5]

*/

    
#define DEBUG_CONSOLE  
#define DEBUG_ENABLED
#define ERROR_ENABLED
    
#ifdef DEBUG_CONSOLE
void DebugConsole_Init( void );
void PRINTF(const char* fmt, ...);
    
char* StringifyBleEvent( uint32 event );
#endif /* DEBUG_CONSOLE */

#if defined DEBUG_CONSOLE && defined DEBUG_ENABLED 
#define DBG(...)  PRINTF("%s:%d ", __FILE__, __LINE__); PRINTF("[DBG] "); PRINTF( __VA_ARGS__ ); PRINTF("\r\n")
#else
#define DBG(...)
#endif /* DEBUG_ENABLED */

#if defined DEBUG_CONSOLE && defined ERROR_ENABLED
#define ERR(...)  PRINTF("%s:%d ", __FILE__, __LINE__); PRINTF("[ERR] "); PRINTF( __VA_ARGS__ ); PRINTF("\r\n")
#else
#define ERR(...)
#endif /* ERROR_ENABLED */

#endif /* __DEBUG_H__ */
/* [] END OF FILE */
