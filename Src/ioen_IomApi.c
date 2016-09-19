/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_IomApi.c
 *
 * FILE DESCRIPTION:
 *   This module contains externally visible functions for the IOM.
 *
***************************************************************/


#include "ioen_IomLocal.h"
#include "ioen_IomLocalCan.h"



/******************************************************************
 * FUNCTION NAME:
 *   ioen_initializeIom
 *
 * DESCRIPTION:
 *   This function saves the pointers to the buffer in local (static) variable for use in later processing
 *
 * INTERFACE:
 *   Global Data:
 *      ioen_inMsgBuffer
 *      ioen_inParamBuffer
 *      ioen_outMsgBuffer
 *      ioen_outParamBuffer
 *      ioen_inObjectCtrl
 *      ioen_outBoolIsInteger

 *
 *   In: inBoolIsInt                : Input  logic parameters: TRUE = 32 bit integer, FALSE = 8 bit integer
 *   In: outBoolIsInt               : Output logic parameters: TRUE = 32 bit integer, FALSE = 8 bit integer
 
 *   In: CurrentPeerMode            : Current Peer mode
 *   In: inMessageBufferAfdxA429_p  : Pointer to input message buffer for AFDX and A429
 *   In: inParamBuffer_p            : Pointer to input parameter buffer
 *   In: inParamBufferSource_p      : Pointer to input parameter per source buffer
 *   In: outMessageBuffer_p         : Pointer to output message buffer for AFDX
 *   In: outParamBuffer_p           : Pointer to output parameter buffer
 *   In: inObjectCtrl               : Two dimensional array to store the comfirmation data per parameter and source
 *                                    for each input dataset that has multiple sources
 *
 ******************************************************************/
void ioen_initializeIom (
    /* IN     */       Bool_t                       inBoolIsInt,
    /* IN     */       Bool_t                       outBoolIsInt,
    /* IN     */       Byte_t               * const inMessageBufferAfdxA429_p,
    /* IN     */       Byte_t               * const inParamBuffer_p,
    /* IN     */       Byte_t               * const inParamBufferSource_p,
    /* IN     */       Byte_t               * const outMessageBuffer_p,
    /* IN     */ const Byte_t               * const outParamBuffer_p,
    /* IN     */       IomObjectCtrlData_t          (* inObjectCtrl)[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    /* initialize local pointers to global buffers */
    ioen_inMsgBuffer            = inMessageBufferAfdxA429_p;
    ioen_inParamBuffer          = inParamBuffer_p;
    ioen_inParamBufferSrc       = inParamBufferSource_p;
    ioen_outMsgBuffer           = outMessageBuffer_p;
    ioen_outParamBuffer         = outParamBuffer_p;
    ioen_inObjectCtrl           = inObjectCtrl;
    ioen_outBoolIsInteger       = outBoolIsInt;


    ioen_icInitInputConverter(inBoolIsInt);

    if (ioen_outMsgBuffer != NULL_PTR)
    {
        /* Set unused bits to zero */
        ioen_aaInitOutputBuffer (ioen_iomConfig_p, ioen_outMsgBuffer);
    }
    else
    {
        /* No action, as designed              */
        /* No output defined for this instance */
    }


    /* Initialise the confirmation structure for each input signal for AFDX and A429 */
    ioen_aaInitSignalConfirmation (ioen_iomConfig_p, ioen_processPeriodicTimeMs);
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_createPorts
 *
 * DESCRIPTION:
 *   This function creates the AFDX input ports
 *   This function creates the AFDX output ports
 *   This function creates the CAN  input ports, and stores their APEX port ID's in iomCanRtTbl_p
 *   This function creates the CAN  output ports
 *   This function creates the A429 input ports
 *
 * INTERFACE:
 *
 *   Global Data: ioen_iomConfig_p
 *   Global Data: ioen_processPeriodicTimeMs
 *
 *   In:      iom_p           : pointer to IOM configuration
 *   In Out:  iomCanRtTbl_p   : CAN routing table
 *   In:      appPeriodNanosec: Periodic time of process in millisecs
 *
 ******************************************************************/
void ioen_createPorts (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN OUT */       IOMCanRoutingPortTbl_t   * const iomCanRtTbl_p,
    /* IN  */       IOMA429RoutingPortTbl_t  * const iomA429RtTbl_p,
    /* IN     */ const UInt32_t                         appPeriodNanosec
)
{
    ioen_processPeriodicTimeMs  = (Float32_t)appPeriodNanosec / 1000000.0;
    ioen_iomConfig_p            = iom_p;
    
    ioen_createRxPortsAfdx (iom_p, ioen_processPeriodicTimeMs);

    ioen_createTxPortsAfdx (iom_p, ioen_processPeriodicTimeMs);

    if (iomCanRtTbl_p != NULL_PTR)
    {
        /* Create the CAN ports */
        ioen_createPortsCan (iom_p, iomCanRtTbl_p, ioen_processPeriodicTimeMs);
    }
    else
    {
        /* No action as designed              */
        /* No CAN in this instance of the IOM */
    }

    /* Create the A429 ports, if configured */
    if (iomA429RtTbl_p != NULL_PTR)
    {
    ioen_createPortsA429 (iom_p, iomA429RtTbl_p,ioen_processPeriodicTimeMs);
    }
    
    /* Initilises data that must be initialised before the process is in Normal Operation mode */
    /* (takes too long and would produce a deadline missed)                                    */
    ioen_ssInitSourceSelection (ioen_iomConfig_p, ioen_processPeriodicTimeMs);
    
}

