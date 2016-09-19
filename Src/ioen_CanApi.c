/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_CanApi.c
 *
 * FILE DESCRIPTION:
 *   This module interfaces to the system to create the CAN input ports and read the CAN input messages.
 *
***************************************************************/

#include <string.h>
#include <stdio.h>
#include <apex/apexLib.h>

#include "ioen_IomLocalCan.h"



/******************************************************************
 * FUNCTION NAME:
 *   ioen_initCanControl
 *
 * DESCRIPTION:
 *   This function initialises the CAN object control structure to allow freshness of message
 *
 * INTERFACE:
 *   Global Data      :  ioen_iomConfig_p
 *
 *   In:      appPeriodMs   : Periodic time of process in millisecs
 *
 ******************************************************************/
void ioen_initCanControl (
    /* IN     */ const Float32_t                   appPeriodMs
)
{
    UInt32_t                  newEntry;
    UInt32_t                  idx;
    UInt32_t                  canIdMasked;
    UInt32_t                  idxCanId;
    const CanMessageConfig_t *msgInfo_p;

    ioen_nofCanMessages = 0;
    msgInfo_p           = CAN_INPUT_MSGINFO_FIRST(ioen_iomConfig_p);

    for (idx = 0; idx < ioen_iomConfig_p->canInputMessageCount; idx++)
    {
        /* Get CAN ID without Functional Status (FS) and Redundancy Channel Identifier (RCI) */
        canIdMasked = msgInfo_p->canId BIT_AND A825_CAN_ID_IGNORE_FS_RCI_MASK;

        /* Search for an existing entry with a different RCI */
        newEntry = TRUE;
        for (idxCanId = 0; ((idxCanId < ioen_nofCanMessages) AND (newEntry == TRUE)); idxCanId++)
        {
            if (ioen_mrcCanMessageControl[idxCanId].canIdMasked == canIdMasked)
            {
                /* An entry allready exists for this CAN ID, with a different source (RCI) */
                /* Do not add a duplicate entry */
                newEntry = FALSE;

                /* Just fill out the config pointer */
                if ((msgInfo_p->canId BIT_AND A825_CAN_ID_RCI_MASK) == 0)
                {
                    /* Source 1 */
                    ioen_mrcCanMessageControl[idxCanId].msgConfigSource1_p = msgInfo_p;
                }
                else
                {
                    /* Source 2 */
                    ioen_mrcCanMessageControl[idxCanId].msgConfigSource2_p = msgInfo_p;
                }
            }
            else
            {
                /* No action as designed */
            }
        }

        if (newEntry == TRUE)
        {
            ioen_mrcCanMessageControl[ioen_nofCanMessages].canIdMasked = canIdMasked;
            /* Just fill out the config pointer */
            if ((msgInfo_p->canId BIT_AND A825_CAN_ID_RCI_MASK) == 0)
            {
                /* Source 1 */
                ioen_mrcCanMessageControl[ioen_nofCanMessages].msgConfigSource1_p = msgInfo_p;
            }
            else
            {
                /* Source 2 */
                ioen_mrcCanMessageControl[ioen_nofCanMessages].msgConfigSource2_p = msgInfo_p;
            }

            /* Initialise Message freshness counters, includes FS validity */
            ioen_ConfirmInitMsg (msgInfo_p->freshTime,
                                 msgInfo_p->unfreshTime,
                                 appPeriodMs,
                                 &ioen_mrcCanMessageControl[ioen_nofCanMessages].msgValidCtrl);

            ioen_nofCanMessages++;

        }
        else
        {
            /* No action as designed                 */
            /* Already an entry for this CAN message */
        }

        /* Always Increment for worst case execution */
        msgInfo_p = CAN_INPUT_MSGINFO_NEXT(msgInfo_p);
    }

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_createPortsCan
 *
 * DESCRIPTION:
 *   This function creates CAN Rx, CAN Tx, CAN routing ports, and initialises the other data structures
 *
 * INTERFACE:
 *   Global Data      :  ioen_apexCanBusIdRxA
 *   Global Data      :  ioen_apexCanBusIdRxB
 *   Global Data      :  ioen_apexCanBusIdTxA
 *   Global Data      :  ioen_apexCanBusIdTxB
 *   Global Data      :  ioen_mrcConfigRoutingTable_p
 *   Global Data      :  ioen_mrcCanMessageControl
 *   Global Data      :  ioen_outputPortsInfoCan
 *
 *   In:      iom_p         : pointer to IOM configuration
 *   In Out:  iomCanRtTbl_p : CAN routing table
 *   In:      appPeriodMs   : Periodic time of process in millisecs
 *
 ******************************************************************/
void ioen_createPortsCan (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN OUT */       IOMCanRoutingPortTbl_t   * const iomCanRtTbl_p,
    /* IN     */ const Float32_t                        appPeriodMs

)
{
    UInt32_t                   msgIdx;
    UInt32_t                   nbPeriod;
    UInt32_t                   portIdx;
    RETURN_CODE_TYPE           ret;
    IOMCanRoutingPortTbl_t    *iomCanPort;
    const CanMessageConfig_t  *msgCfg_p;

    /* Initialise global variables */
    ioen_mrcConfigRoutingTable_p = iomCanRtTbl_p;

    memset (&ioen_mrcCanMessageControl, 0, sizeof(ioen_mrcCanMessageControl));
    memset (&ioen_outputPortsInfoCan,   0, sizeof(ioen_outputPortsInfoCan));

    ioen_initCanControl(appPeriodMs);

    /* create all CAN ports for this partition */
    portIdx    = 0;
    iomCanPort = &iomCanRtTbl_p[0];

    while (iomCanPort->canType != IOEN_CAN_PORT_TYPE_TERMINATOR)
    {
        switch (iomCanPort->canType)
        {
            case IOEN_CAN_PORT_TYPE_RX_A :
                /* CAN bus input for bus A for this partition */
                CREATE_QUEUING_PORT(
                    iomCanPort->portName,
                    sizeof(CANMessage_t) * IOEN_MAX_CAN_MESSAGES_PER_CYCLE,     /* max message length */
                    iomCanPort->queueLength,
                    DESTINATION,
                    FIFO,
                    &ioen_apexCanBusIdRxA,
                    &ret
                );

                CHECK_CODE("CREATE_QUEUING_PORT", ret);
                break;

            case IOEN_CAN_PORT_TYPE_RX_B :
                /* CAN bus input for bus B for this partition */
                CREATE_QUEUING_PORT(
                    iomCanPort->portName,
                    sizeof(CANMessage_t) * IOEN_MAX_CAN_MESSAGES_PER_CYCLE,     /* max message length */
                    iomCanPort->queueLength,
                    DESTINATION,
                    FIFO,
                    &ioen_apexCanBusIdRxB,
                    &ret
                );

                CHECK_CODE("CREATE_QUEUING_PORT", ret);
                break;

            case IOEN_CAN_PORT_TYPE_TX_A   :
                /* CAN bus output for bus A for this partition */
                CREATE_QUEUING_PORT(
                    iomCanPort->portName,
                    sizeof(CANMessage_t) * IOEN_MAX_CAN_MESSAGES_PER_CYCLE,     /* max message length */
                    iomCanPort->queueLength,
                    SOURCE,
                    FIFO,
                    &ioen_apexCanBusIdTxA,
                    &ret
                );

                CHECK_CODE("CREATE_QUEUING_PORT", ret);
                break;

            case IOEN_CAN_PORT_TYPE_TX_B   :
                /* CAN bus output for bus B for this partition */
                CREATE_QUEUING_PORT(
                    iomCanPort->portName,
                    sizeof(CANMessage_t) * IOEN_MAX_CAN_MESSAGES_PER_CYCLE,     /* max message length */
                    iomCanPort->queueLength,
                    SOURCE,
                    FIFO,
                    &ioen_apexCanBusIdTxB,
                    &ret
                );

                CHECK_CODE("CREATE_QUEUING_PORT", ret);
                break;

            case IOEN_CAN_PORT_TYPE_ROUTING_TX:
                /* Port to route a received CAN message to a slave partition */
                /* Check if the port has already been created, NB: duplicate entries with different CAN ID's in the routing table are allowed */
                GET_QUEUING_PORT_ID (iomCanPort->portName, &iomCanPort->apexCanBusId, &ret);

                if (ret == INVALID_CONFIG)
                {
                    /* Port has not yet been created, create it */
                    CREATE_QUEUING_PORT(
                        iomCanPort->portName,     /* port name      */
                        sizeof(CANMessage_t) * IOEN_MAX_CAN_MESSAGES_PER_CYCLE,     /* max message length */
                        iomCanPort->queueLength,
                        SOURCE,
                        FIFO,
                        &iomCanPort->apexCanBusId,
                        &ret
                    );

                    CHECK_CODE("CREATE_QUEUING_PORT", ret);
                }
                else
                {
                    /* No action as designed */
                }
                break;

            default:
                /* No action, as designed */
            break;
        }

        portIdx++;
        iomCanPort = &iomCanRtTbl_p[portIdx];
    }


    /* Initialise Output CAN Message info */
    msgCfg_p = (CanMessageConfig_t *)((void *) iom_p + iom_p->canOutputMessageStart);

    for (msgIdx=0; (msgIdx < iom_p->canOutputMessageCount); msgIdx++)
    {
        nbPeriod = CEILING_POS ((Float32_t)msgCfg_p->freshTime / appPeriodMs); /* rounding up */
        if (nbPeriod != 0)
        {
            ioen_outputPortsInfoCan.port[msgIdx].countdownInit = nbPeriod;
        }
        else
        {
            /* Message rate is lower than the App Period, send the message every cycle */
            ioen_outputPortsInfoCan.port[msgIdx].countdownInit = 1;
        }

        ioen_outputPortsInfoCan.port[msgIdx].countdownValue = (SInt32_t) ioen_outputPortsInfoCan.port[msgIdx].countdownInit;

        /* Next message config is after current config and its simple mappings */
        msgCfg_p = (const CanMessageConfig_t *)((const Byte_t *)msgCfg_p + msgCfg_p->size);

    }

}





/******************************************************************
 * FUNCTION NAME:
 *   ioen_writeMessagesCan
 *
 * DESCRIPTION:
 *   This function transmits the A825 APEX ports
 *
 * INTERFACE:
 *   Global Data:   ioen_outputPortsInfoCan
 *   Global Data:   ioen_iomConfig_p
 *   Global Data:   ioen_outParamBuffer
 *
 ******************************************************************/
void ioen_writeMessagesCan (
       /*        */       void
)
{
    UInt32_t                   idx;                /* index of write message  */
    const CanMessageConfig_t  *msgCfg_p;
    RETURN_CODE_TYPE           ret;                /* return code             */
    CANMessage_t               canOutputMessage;   /* buffer for one CAN message, no global message buffer like AFDX */
    const IOMConfigHeader_t   *iom_p;

    if (ioen_outParamBuffer != NULL_PTR)
    {
        /* Get pointer to IOM Configuration */
        iom_p = ioen_iomConfig_p;

        msgCfg_p = (CanMessageConfig_t *)((void *) iom_p + iom_p->canOutputMessageStart);

        for (idx = 0; idx < iom_p->canOutputMessageCount; idx++)
        {
            if (ioen_outputPortsInfoCan.port[idx].countdownValue > 1)
            {
                /* If the port countdown value has not reached 0, it is too early to send the message, */
                /* decrement the countdown and move to the next message                                */
                ioen_outputPortsInfoCan.port[idx].countdownValue--;
            }
            else
            {
                /* It's time to send the message, reinit the corresponding countdown */
                ioen_outputPortsInfoCan.port[idx].countdownValue = (SInt32_t)ioen_outputPortsInfoCan.port[idx].countdownInit;

                /* Setup the output message CAN ID */
                canOutputMessage.canId = msgCfg_p->canId;

                /* Setup the output message Data */
                ioen_omcProcessOutputParametersCan (msgCfg_p, ioen_outParamBuffer, &canOutputMessage.data[0]);

                /* Send the output message on a Queuing Port to the CAN bus A, NB: A message is lways transmitted on both physical CAN busses */
                SEND_QUEUING_MESSAGE(ioen_apexCanBusIdTxA, (void *) &canOutputMessage, msgCfg_p->messageLength, 0, &ret);
                CHECK_CODE("SEND_QUEUING_MESSAGE", ret);

                /* Send the output message on a Queuing Port to the CAN bus A, NB: A message is lways transmitted on both physical CAN busses */
                SEND_QUEUING_MESSAGE(ioen_apexCanBusIdTxB, (void *) &canOutputMessage, msgCfg_p->messageLength, 0, &ret);
                CHECK_CODE("SEND_QUEUING_MESSAGE", ret);
            }

            /* Next message config is after current config and its simple mappings */
            msgCfg_p = (const CanMessageConfig_t *)((const Byte_t *)msgCfg_p + msgCfg_p->size);
        }
    }
    else
    {
        /* No action as designed, no Output parameter buffer defined */
    }

}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_readMessagesCan
 *
 * DESCRIPTION:
 *   This function reads messages from IO and process the data
 *
 * INTERFACE:
 *
 *   Global Data      :  ioen_apexCanBusIdRxA
 *   Global Data      :  ioen_apexCanBusIdRxB
 *
 ******************************************************************/
void ioen_readMessagesCan (
    /*        */ void
)
{
    /* resets all addition parameters to 0, and message routing lists to zero */
    ioen_imcStartInputCan ();

    ioen_imcHandleBus (ioen_apexCanBusIdRxA);
    ioen_imcHandleBus (ioen_apexCanBusIdRxB);

    /* Check freshness of messages not received */
    ioen_imcCheckMessageFreshness ();

}




