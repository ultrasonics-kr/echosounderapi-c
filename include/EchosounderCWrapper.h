// Copyright (c) EofE Ultrasonics Co., Ltd., 2024
#if !defined(SINGLESONARCWRAPPER_H)
#define SINGLESONARCWRAPPER_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "EchosounderCommands.h"

#define SERIALPORT_TIMEOUT_MS 100U
#define VALUE_TEXT_SIZE 64U

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 ) 

#ifdef DLL_LIB
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#elif defined( __GNUC__ )

#ifdef DLL_LIB
#define DLL_EXPORT __attribute__((visibility("default")))
#else
#define DLL_EXPORT
#endif

#else
#error compiler not supported!

#endif

#ifdef __cplusplus
extern "C" {
#endif

struct echosoundervalue_t
{
    char value_text[VALUE_TEXT_SIZE];
    int value_len;
};

typedef struct echosoundervalue_t EchosounderValue;
typedef struct echosoundervalue_t *pEchosounderValue;
typedef const struct echosoundervalue_t *pcEchosounderValue;

typedef void *pSnrCtx;
typedef void *hEchosounder; 

/**
 * @brief   Initiate connection to single frequency echosounder
 *
 * @note    This function is used for open port and check whether echosounder is connected to it.
 *
 * @param[in]  portpath     path to serial port
 * @param[in]  baudrate     serial port baudrate
 
 * @return                  Valid handle to futher using to manage the echosounder
 * @return                  NULL in case of failure
 */
DLL_EXPORT pSnrCtx SingleEchosounderOpen(const char *portpath, uint32_t baudrate);

/**
 * @brief   Initiate connection to dual frequency echosounder
 *
 * @note    This function is used for open port and check whether echosounder is connected to it.
 *
 * @param[in]  portpath     path to serial port
 * @param[in]  baudrate     serial port baudrate
 *
 * @return                  Valid handle to futher using to manage the echosounder
 * @return                  NULL in case of failure
 */
DLL_EXPORT pSnrCtx DualEchosounderOpen(const char* portpath, uint32_t baudrate);

/**
 * @brief   Finalize connection to the echosounder
 *
 * @note    This function is used for close port, it does not change the running state of the echosounder.
 */
DLL_EXPORT void EchosounderClose(pSnrCtx snrctx);

/**
 * @brief   Read raw data from the echosounder
 *
 * @note    This function should be used when echosounder is in the running state
 *
 * @param[in]  snrctx       Connection handle obtained by (Single|Dual)EchosounderOpen function.
 * @param[in]  buffer       pointer for data buffer
 * @param[in]  baudrate     number of bytes to be read
 *
 * @return                  number of bytes read
 */
DLL_EXPORT size_t EchosounderReadData(pSnrCtx snrctx, uint8_t *buffer, size_t size);

/**
 * @brief   Get value for the given parameter (command)
 *
 * @note    This function can be used in any state on the echosounder
 *
 * @param[in]  snrctx       Connection handle obtained by (Single|Dual)EchosounderOpen function.
 * @param[in]  command      parameter for which command should be obtained
 * @param[out] value        value of given command
 *
 * @return                  0  - value is valid
 * @return                  -1 - value is invalid
 */
DLL_EXPORT int EchosounderGetValue(pSnrCtx snrctx, EchosounderCommandIds_t command, pEchosounderValue value);

/**
 * @brief   Set value for the given parameter (command)
 *
 * @note    This function can be used in any state on the echosounder
 *
 * @param[in]  snrctx       Connection handle obtained by (Single|Dual)EchosounderOpen function.
 * @param[in]  command      parameter for which command should be set
 * @param[out] value        value of given command
 *
 * @return                  0  - value is set successfully
 * @return                  -1 - set value failed
 */
DLL_EXPORT int EchosounderSetValue(pSnrCtx snrctx, EchosounderCommandIds_t command, pcEchosounderValue value);

/**
 * @brief   Convert value read from echosounder to long 
 *
 * @note    Helper function
 *
 * @param[in]  value        Echosounder value to convert
 *
 * @return                  Converted value
 */
DLL_EXPORT long EchosounderValueToLong(pcEchosounderValue value);

/**
 * @brief   Convert value read from echosounder to float
 *
 * @note    Helper function
 *
 * @param[in]  value        Echosounder value to convert
 *
 * @return                  Converted value
 */
DLL_EXPORT float EchosounderValueToFloat(pcEchosounderValue value);

/**
 * @brief   Convert value read from echosounder to null-terminated char*
 *
 * @note    Helper function
 *
 * @param[in]  value        Echosounder value to convert
 *
 * @return                  Converted value
 */
DLL_EXPORT const char *EchosounderValueToText(pcEchosounderValue value);

/**
 * @brief   Check whether echosounder value is valid
 *
 * @note    Helper function
 *
 * @param[in]  value        Echosounder value to convert
 *
 * @return                  true - value is valid
 * @return                  false - value is invalid
 */
DLL_EXPORT bool IsValidEchosounderValue(pcEchosounderValue value);

/**
 * @brief   Convert long number to echosounder value
 *
 * @note    Helper function
 *
 * @param[in]  num          Long number to convert
 * @param[out] value        Converted value
 */
DLL_EXPORT void LongToEchosounderValue(long num, pEchosounderValue value);

/**
 * @brief   Convert float number to echosounder value
 *
 * @note    Helper function
 *
 * @param[in]  num          Long number to convert
 * @param[out] value        Converted value
 */
DLL_EXPORT void FloatToEchosounderValue(float num, pEchosounderValue value);

/**
 * @brief   Start the unit.
 *
 * @note    Echosounder will be in running state after this.
 *
 * @param[in]  snrctx       Connection handle obtained by (Single|Dual)EchosounderOpen function.
 */
DLL_EXPORT void EchosounderStart(pSnrCtx snrctx);

/**
 * @brief   Stop the unit.
 *
 * @note    Echosounder will be in stop state after this.
 *
 * @param[in]  snrctx       Connection handle obtained by (Single|Dual)EchosounderOpen function.
 */
DLL_EXPORT void EchosounderStop(pSnrCtx snrctx);

/**
 * @brief   Checking the running state of the echosounder
 *
 * @note    Not affect the echosounder state
 *
 * @param[in]  snrctx       Connection handle obtained by (Single|Dual)EchosounderOpen function.
 *
 * @return                  true - echosounder in running state
 * @return                  false - echosounder is not in running state
 */
DLL_EXPORT bool EchosounderIsRunning(pSnrCtx snrctx);

/**
 * @brief   Checking whether echosounder is detected on serial port
 *
 * @note    Not affect the echosounder state
 *
 * @param[in]  snrctx       Connection handle obtained by (Single|Dual)EchosounderOpen function.
 *
 * @return                  true - echosounder is detected
 * @return                  false - echosounder is not detected
 */
DLL_EXPORT bool EchosounderIsDetected(pSnrCtx snrctx);

/**
 * @brief   Detect echosounder on serial port
 *
 * @note    Echosounder will be in stop state after this
 *
 * @param[in]  snrctx       Connection handle obtained by (Single|Dual)EchosounderOpen function.
 *
 * @return                  true - echosounder is detected
 * @return                  false - echosounder is not detected
 */
DLL_EXPORT bool EchosounderDetect(pSnrCtx snrctx);

/**
 * @brief   Set current time to the echosounder (UTC). It use current time from host PC.
 *
 * @note    It can be neccessary for sync NMEA output with GPS output
 *
 * @param[in]  snrctx       Connection handle obtained by (Single|Dual)EchosounderOpen function.
 */
DLL_EXPORT void EchosounderSetCurrentTime(pSnrCtx snrctx);

#ifdef __cplusplus
}
#endif

#endif // !SINGLESONARCWRAPPER_H
