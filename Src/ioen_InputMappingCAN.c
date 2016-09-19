/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_InputMappingCan.c
 *
 * FILE DESCRIPTION:
 *   This module performs all the conversion of input signals to application input parameters for CAN IO.
 *
***************************************************************/

#include "ioen_IomLocalCan.h"



/******************************************************************
 * FUNCTION NAME:
 *   ioen_imcDoOneInputMapping
 *
 * DESCRIPTION:
 *   This function performs the mapping of one input parameter.
 *   If the parameter has no source, then it is an internal parameter, just set the status to valid
 *
 * INTERFACE:
 *   Global Data      :  ioen_inputConverters
 *   Global Data      :  ioen_inParamBufferSrc
 *
 *   In    :  validity     : Validity of input message
 *   In    :  mapConfig_p  : A Config with one Parameter and on signal Config
 *   In    :  msg_p        : pointer to input message buffer
 *
 ******************************************************************/
void ioen_imcDoOneInputMapping (
    /* IN     */ const Validity_t                     validity,
    /* IN     */ const simpleMapConfigCanIn_t * const mapConfig_p,
    /* IN     */ const CANMessage_t           * const msg_p
)
{
    UInt32_t    valIdx;
    Validity_t  newValidity;

    if (mapConfig_p->parconfig.numSources == 0)
    {
        newValidity.spare          = 0;
        newValidity.selectedSource = 0;
        newValidity.value          = IOEN_GLOB_DATA_VALID;
        newValidity.ifValue        = IOEN_VALIDITY_NORMALOP;
        /* Internal parameter, just set status to valid */
        *(UInt32_t *)(ioen_inParamBufferSrc + mapConfig_p->sigconfig.valOffset) = * ((UInt32_t *) &newValidity);
    }
    else
    {

        /* Call input to output converter function */
        newValidity = validity;
        for (valIdx=0; 
             ((valIdx < mapConfig_p->validity.numConditions) AND (newValidity.value == IOEN_GLOB_DATA_VALID));
             valIdx++)
        {
            /* Check input signal validities for this CAN input, if any configured */
            /* eg. a bool in this CAN message "MKB_L_A825_PushButton_Validity_L1"  */
            /*     or a range check                                                */
            newValidity = ioen_validityChecker[mapConfig_p->validity.conditionType[valIdx]](&msg_p->data[0], &mapConfig_p->validity.condition[valIdx]);
        }

        /* Convert the input signal */
        ioen_inputConverters[mapConfig_p->sigconfig.type](newValidity, &mapConfig_p->parconfig, &mapConfig_p->sigconfig, &msg_p->data[0], ioen_inParamBufferSrc);
    }
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_imcProcessCanMessage
 *
 * DESCRIPTION:
 *   This function extracts signals from the message buffer and copy to param buffer according to mappings associated with message
 *
 * INTERFACE:
 *
 *   In Out:  msgctrl_p      : Runtime control data for input message 
 *   In    :  mapConfig_p    : A Config with one Parameter and on signal Config
 *   In    :  msg_p          : pointer to input message buffer
 *
 ******************************************************************/
void ioen_imcProcessCanMessage (
    /* IN OUT */       CANMessageControl_t    * const msgctrl_p,
    /* IN     */ const CanMessageConfig_t     * const msgConfig_p,
    /* IN     */ const CANMessage_t           * const msg_p
)
{
    /*
     * Called by read message with a message config structure and the data of a message
     * Extract signals from message and copy to param buffer according to mappings associated with message
     */
    UInt32_t                      tmp;
    UInt32_t                      fs;
    Byte_t                        cnt;           /* Index of current Mapping being processed. */
    Byte_t                        numMaps;       /* number of mappings for the message        */
    const simpleMapConfigCanIn_t *curMap_p;      /* pointer to current parameter mapping      */

    fs = (msg_p->canId BIT_AND A825_CAN_ID_FS_MASK) BIT_RSHIFT A825_CAN_ID_FS_SHIFT;
    printf("canID %08X, data : %02X %02X %02X %02X %02X %02X %02X %02X \n",msg_p->canId,
    		msg_p->data[0], msg_p->data[1], msg_p->data[2], msg_p->data[3], msg_p->data[4], msg_p->data[5], msg_p->data[6], msg_p->data[7]);
    if (fs == IOEN_CAN_NO)   /* Secondly check the message FS */
    {
        msgctrl_p->msgValidCtrl.validity.current.value   = IOEN_GLOB_DATA_VALID;
        msgctrl_p->msgValidCtrl.validity.current.ifValue = IOEN_VALIDITY_NORMALOP;
    }
    else if (fs == IOEN_CAN_FT)
    {
        msgctrl_p->msgValidCtrl.validity.current.value   = IOEN_GLOB_DATA_VALID;
        msgctrl_p->msgValidCtrl.validity.current.ifValue = IOEN_VALIDITY_TEST;
    }
    else if (fs == IOEN_CAN_NCD)
    {
        msgctrl_p->msgValidCtrl.validity.current.value   = IOEN_GLOB_DATA_INVALID;
        msgctrl_p->msgValidCtrl.validity.current.ifValue = IOEN_VALIDITY_NCD;
    }
    else
    {
        msgctrl_p->msgValidCtrl.validity.current.value   = IOEN_GLOB_DATA_INVALID;
        msgctrl_p->msgValidCtrl.validity.current.ifValue = IOEN_VALIDITY_NODATA;
    }

    /* Copy to output buffer if value is valid */
    if (msgctrl_p->msgValidCtrl.validity.current.value == IOEN_GLOB_DATA_VALID)
    {
        /* Process all parameters to internal buffer */
        curMap_p = (const simpleMapConfigCanIn_t*)(msgConfig_p + 1);
        numMaps  = msgConfig_p->numMappings;
        for (cnt = 0; cnt < numMaps; cnt++)
        {
            ioen_imcDoOneInputMapping  (msgctrl_p->msgValidCtrl.validity.current,
                                        curMap_p,
                                        msg_p);

            /* Copy confirmed status */
            *(UInt32_t *)(ioen_inParamBuffer + curMap_p->parconfig.valOffset) = *(UInt32_t *)(ioen_inParamBufferSrc + curMap_p->sigconfig.valOffset);
            tmp = *(UInt32_t *)(ioen_inParamBuffer + curMap_p->parconfig.valOffset) ;
            printf("Offset: %d, status: 0x%08X, ",curMap_p->parconfig.valOffset, tmp);

            /* Copy confirmed data */
            if (curMap_p->parconfig.parSize == 32)
            {
                /* 32 bits can be copied as a word in one operation for optimisation */
                *(UInt32_t *)(ioen_inParamBuffer + curMap_p->parconfig.parOffset) = *(UInt32_t *)(ioen_inParamBufferSrc + curMap_p->sigconfig.parOffset);
                tmp = *(UInt32_t *)(ioen_inParamBuffer + curMap_p->parconfig.parOffset);
                printf("data: 0x%08X\n",tmp);
            }
            else
            {
                memcpy ((UInt32_t *)(ioen_inParamBuffer    + curMap_p->parconfig.parOffset),
                        (UInt32_t *)(ioen_inParamBufferSrc + curMap_p->sigconfig.parOffset),
                        curMap_p->parconfig.parSize/8);
            }

            curMap_p++;
        }
    }
    else
    {
        /* No action as designed */
        /* Invalid values are not copied to parameter buffer until they are confirmed */
    }

}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_imcInvalidateCanMessage
 *
 * DESCRIPTION:
 *   This function invalidates all mappings associated with message
 *
 * INTERFACE:
 *
 *   In:  msgctrl_p      : Runtime control data for input message 
 *
 ******************************************************************/
void ioen_imcInvalidateCanMessage (
    /* IN OUT */       CANMessageControl_t    * const msgctrl_p
)
{
    /*
     * Called by read message with a message config structure and the data of a message
     * Extract signals from message and copy to param buffer according to mappings associated with message
     */
    Byte_t                        cnt;           /* Index of current Mapping being processed. */
    Byte_t                        numMaps;       /* number of mappings for the message        */
    const simpleMapConfigCanIn_t *curMap_p;      /* pointer to current parameter mapping      */
    CANMessage_t                  canMsg;

    memset (&canMsg, 0, sizeof(canMsg));

    if (msgctrl_p->msgValidCtrl.validity.current.value == IOEN_GLOB_DATA_INVALID)
    {
        /* An invalid was detected */
        msgctrl_p->msgValidCtrl.validity.confirmed.value   = msgctrl_p->msgValidCtrl.validity.current.value;
        msgctrl_p->msgValidCtrl.validity.confirmed.ifValue = msgctrl_p->msgValidCtrl.validity.current.ifValue;
    }
    else
    {
        /* No invalid detected, therefore this message must be lost */
        msgctrl_p->msgValidCtrl.validity.current.value     = IOEN_GLOB_DATA_LOST;
        msgctrl_p->msgValidCtrl.validity.current.ifValue   = IOEN_VALIDITY_UNFRESH;
        msgctrl_p->msgValidCtrl.validity.confirmed.value   = IOEN_GLOB_DATA_LOST;
        msgctrl_p->msgValidCtrl.validity.confirmed.ifValue = IOEN_VALIDITY_UNFRESH;
    }

    /* Process all parameters from first source (any source could be used) to internal buffer */
    numMaps  = msgctrl_p->msgConfigSource1_p->numMappings;
    curMap_p = (const simpleMapConfigCanIn_t*)(msgctrl_p->msgConfigSource1_p + 1);
    for (cnt = 0; cnt < numMaps; cnt++)
    {
        /* Process these mappings to invalidate its parameters */
        ioen_imcDoOneInputMapping  (msgctrl_p->msgValidCtrl.validity.confirmed,
                                    curMap_p,
                                    &canMsg);

        /* Copy confirmed status to parameter buffer */
        *(UInt32_t *)(ioen_inParamBuffer + curMap_p->parconfig.valOffset) = *(UInt32_t *)(ioen_inParamBufferSrc + curMap_p->sigconfig.valOffset);

        if (curMap_p->parconfig.parSize == 32)
        {
            /* 32 bits can be copied as a word in one operation for optimisation */
            *(UInt32_t *)(ioen_inParamBuffer + curMap_p->parconfig.parOffset) = *(UInt32_t *)(ioen_inParamBufferSrc + curMap_p->sigconfig.parOffset);
        }
        else
        {
            memcpy ((UInt32_t *)(ioen_inParamBuffer    + curMap_p->parconfig.parOffset),
                    (UInt32_t *)(ioen_inParamBufferSrc + curMap_p->sigconfig.parOffset),
                    curMap_p->parconfig.parSize/8);
        }

        curMap_p++;

    }
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_imcStartInputCan
 *
 * DESCRIPTION:
 *   This function resets all addition parameters to 0
 *
 * INTERFACE:
 *   Global Data      :  ioen_inParamBufferSrc
 *   Global Data      :  ioen_iomConfig_p
 *
 ******************************************************************/
void ioen_imcStartInputCan (
    /*        */ void
)
{
    UInt32_t            msgcnt;
    CanMessageConfig_t *msg_p;
    UInt32_t            msgidx;
    UInt32_t            mapidx;
    simpleMapConfig_t  *singleMap_p;
    UInt32_t                  portIdx;
    IOMCanRoutingPortTbl_t   *iomCanPort;


    msgcnt = ioen_iomConfig_p->canInputMessageCount;
    msg_p  = CAN_INPUT_MSGINFO_FIRST(ioen_iomConfig_p);

    /* reset all counter to 0, we will add the other values */
    for (msgidx = 0; msgidx < msgcnt; msgidx++)
    {
        singleMap_p = (simpleMapConfig_t *)(msg_p + 1);    /* mappings start directly after message */

        for (mapidx = 0; mapidx < msg_p->numMappings; mapidx++)
        {
            if (singleMap_p->sigconfig.type == IOEN_INPUT_MAPPING_INT8_ADD)
            {
                *((UInt32_t*)(ioen_inParamBufferSrc + singleMap_p->sigconfig.parOffset)) = 0;
				  *((UInt32_t*)(ioen_inParamBuffer    + singleMap_p->parconfig.parOffset)) = 0;

                singleMap_p++;
            }
            else
            {
                /* No action as designed */
            }
        }

        msg_p = CAN_INPUT_MSGINFO_NEXT(msg_p);
    }

    /* Clear previous CAN Routing messages */
    portIdx    = 0;
    iomCanPort = &ioen_mrcConfigRoutingTable_p[0];
    while (iomCanPort->canType != IOEN_CAN_PORT_TYPE_TERMINATOR)
    {
        iomCanPort->nofOutMsg = 0;

        portIdx++;
        iomCanPort = &ioen_mrcConfigRoutingTable_p[portIdx];
    }
}





/******************************************************************
 * FUNCTION NAME:
 *   ioen_imcLookupMessage
 *
 * DESCRIPTION:
 *   This function looks up can ID in list of message configuration records
 *   Perform a linear search, since we have just (7-10) messages configured
 *
 * INTERFACE:
 *   Global Data      :  ioen_iomConfig_p
 *
 *   In:  canId       : CAN ID to read
 *   Out: msgConfig_p : Pointer to configuration of this CAN ID, no match, return NULL
 *   Out: msgCtrl_p   : Pointer to runtime data of this CAN ID, no match, return NULL
 *
 *
 ******************************************************************/
void ioen_imcLookupMessage (
   /* IN     */ const UInt32_t                       canId,
   /*    OUT */ const CanMessageConfig_t          ** msgConfig_p,
   /*    OUT */       CANMessageControl_t         ** msgCtrl_p
)
{
    UInt32_t                  idxCanId;
    UInt32_t                  canIdIgnoreFsRci;

    *msgConfig_p = (const CanMessageConfig_t *) 0;
    *msgCtrl_p   = (CANMessageControl_t *) 0;

    /* mask out  RCI, FS and unused top 3 bits */
    canIdIgnoreFsRci = canId BIT_AND A825_CAN_ID_IGNORE_FS_RCI_MASK;

    /* Get CAN Control runtime data */
    for (idxCanId = 0; idxCanId < ioen_nofCanMessages; idxCanId++)
    {
        if (ioen_mrcCanMessageControl[idxCanId].canIdMasked == canIdIgnoreFsRci)
        {
            *msgCtrl_p = &ioen_mrcCanMessageControl[idxCanId];
            if ((canId BIT_AND A825_CAN_ID_RCI_MASK) == 0)
            {
                /* Source 1 */
                *msgConfig_p = ioen_mrcCanMessageControl[idxCanId].msgConfigSource1_p;
            }
            else
            {
                /* Source 2 */
                *msgConfig_p = ioen_mrcCanMessageControl[idxCanId].msgConfigSource2_p;
            }
            break;
        }
        else
        {
            /* No action as designed */
        }
    }

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_imcCheckMessageFreshness
 *
 * DESCRIPTION:
 *   This function checks the freshness of a CAN message
 *
 * INTERFACE:
 *   Global Data      :  ioen_iomConfig_p
 *   Global Data      :  ioen_mrcCanMessageControl
 *
 *
 ******************************************************************/
void ioen_imcCheckMessageFreshness (
   /* IN     */ void
)
{
    UInt32_t                  idx;
    CANMessageControl_t      *msgctrl_p;


    for (idx = 0; idx < ioen_nofCanMessages; idx++)
    {
        msgctrl_p = &ioen_mrcCanMessageControl[idx];
        if (msgctrl_p->msgValidCtrl.cycle > 0)
        {
            /* Still fresh, decrement cycle counter */
            msgctrl_p->msgValidCtrl.cycle--;

            if (msgctrl_p->msgValidCtrl.cycle == 0)
            {
                /* Message is not fresh, therefore confirmed lost    */
                /* Process this message to invalidate its parameters */
                ioen_imcInvalidateCanMessage (msgctrl_p);
            }
            else
            {
                /* No action, as designed */
                /* Message is still fresh */
            }
        }
        else
        {
            /* No action, as defined   */
            /* Message is unfresh      */
        }
                    
    }

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_imcNewMessage
 *
 * DESCRIPTION:
 *   This function filters out CAN Messages that are not different to its previous message.
 *   This is only performed on Pilot input message because the refresh rate is 10 to 20 ms.
 * 
 *   The CPs will always send unchanged messages with all data bytes == zero to the ARINC825,  
 *   when the pilot does not operate the CPs, but some of these messages are unnecessary to be 
 *   sent to host application. So the ARINC825 should set up a filter. 
 *   In general (except the specific case where the data of BARO Unit Selection in DCP is never 0, always 1 or 2), 
 *   the ARINC825 will discard the message when all data bytes of the message are 0,
 *   and the message data is identical to the previous message. Message filtering should 
 *   be restricted to the “Pilot Input” messages from the CPs.
 *
 *
 * INTERFACE:
 *
 *   In:  msgctrl_p      : Runtime control data for input message 
 *   In:  msg_p          : pointer to input message buffer
 *
 *   Return: TRUE if the message has changed data from the last message
 *
 ******************************************************************/
Bool_t ioen_imcNewMessage (
    /* IN OUT */       CANMessageControl_t    * const msgctrl_p,
    /* IN     */ const CANMessage_t           * const msg_p
)
{
    Bool_t  newMsg;

    /* Check if data has changed */
    
    if (   (msg_p->canId  != msgctrl_p->prevMsg.canId)
        OR (msg_p->nbytes != msgctrl_p->prevMsg.nbytes)
       )
    {
        /* FS or size Changed */
        newMsg = TRUE;
    }
    else
    {
        /* Remove RCI, FS and common bits (bottom 4 bits) for CAN ID search */
        switch ((msg_p->canId BIT_RSHIFT 4) BIT_AND 0x00000FFF)
        {
            case 0x200:
            case 0x240:
                /* 0x08482004 */
                /* 0x08482005 */
                /* 0x08482404 */
                /* 0x08482405 */
                /* the frame content is:                                     */
                /*   data[0] == 0 ,data[1] == 0 , data[3] == 0 ,data[4] == 0 */
                /*   data[2] == 1 or 2                                       */
                /*   data[5] == Validity, data[6] == HeartBeat               */
                /*   Do not use Validity or HeartBeat for filtering          */

                if (memcmp (&msg_p->data[0], &msgctrl_p->prevMsg.data[0], 5) != 0)
                {
                    /* Different data */
                    newMsg = TRUE;
                }
                else
                {
                    if (ioen_UtilIsZero (&msg_p->data[0], 5) == TRUE)
                    {
                        /* New and Old data is 0, Discard, empty (unchanged data from LRU) */
                        newMsg = FALSE;
                    }
                    else
                    {
                        if (    (msg_p->data[2] == 1 OR  msg_p->data[2] == 2)
                            AND (msg_p->data[0] == 0 AND msg_p->data[1] == 0 AND msg_p->data[3] == 0 AND msg_p->data[4] == 0 )
                           )
                        {
                            /* Discard */
                            newMsg = FALSE;
                        }
                        else
                        {
                            newMsg = TRUE;
                        }

                    }
                }
                break;

            case 0x000:
            case 0x040:
            case 0x100:
            case 0x140:
            case 0x300:
                /* 0x08480004 */
                /* 0x08480005 */
                /* 0x08480404 */
                /* 0x08480405 */
                /* 0x08481004 */
                /* 0x08481005 */
                /* 0x08481404 */
                /* 0x08481405 */
                /* 0x08483004 */
                /* 0x08483005 */
                /* Do not include validity and heartbeat in filter */
                if (   (memcmp (&msg_p->data[0], &msgctrl_p->prevMsg.data[0], msg_p->nbytes - 2) != 0)
                    OR (ioen_UtilIsZero (&msg_p->data[0], msg_p->nbytes - 2) == FALSE)
                   )
                {
                    /* Different data OR data is non zero, eg. a count operation to be added to the output */
                    newMsg = TRUE;
                }
                else
                {
                    newMsg = FALSE;
                }
                break;

            case 0x201:
            case 0x241:
                /* 0x8482014 */
                /* 0x8482015 */
                /* 0x8482414 */
                /* 0x8482415 */
                /* There is no validity and heartbeat in these messages */
                if (   (memcmp (&msg_p->data[0], &msgctrl_p->prevMsg.data[0], msg_p->nbytes) != 0)
                    OR (ioen_UtilIsZero (&msg_p->data[0], msg_p->nbytes) == FALSE)
                   )
                {
                    /* Different data OR data is non zero, eg. a count operation to be added to the output */
                    newMsg = TRUE;
                }
                else
                {
                    newMsg = FALSE;
                }
                break;

            default:
                /* No filtering for other messages */
                newMsg = TRUE;
                break;
        }
    }
    
    return newMsg;
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_imcHandleBus
 *
 * DESCRIPTION:
 *   This function processes data from one bus, it reads all queued messages from the device (up to 32)
 *      For each message
 *         look it up by can id
 *         if bus is the current bus for the message or the message has not been received (on the other bus) for a timeout period,
 *         process it, otherwise discard it.
 *      Note: This mechanism will switch the receiving bus for a message on a permanent or temporary failure. The bus will only be
 *      switched back by after a temporary or permanent failure of the second failure.
 *
 * INTERFACE:
 *   Global Data      :  ioen_mrcCycle_count
 *   Global Data      :  ioen_mrcCanMessageControl
 *   Global Data      :  ioen_mrcConfigRoutingTable_p
 *
 *   In: apexCanBusId : APEX port ID used to read from
 *
 ******************************************************************/
void ioen_imcHandleBus (
   /* IN     */ const SInt32_t   apexCanBusId
)
{
    Bool_t                    newMsg;
    UInt32_t                  nofCanMsg;
    MESSAGE_SIZE_TYPE         len;           /* message length from low level CAN driver, multiple of CANMessage_t */
    RETURN_CODE_TYPE          ret;           /* return value from APEX calls */
    UInt32_t                  portIdx;
    UInt32_t                  msgIdx;
    CANMessage_t             *canMsg_p;      /* pointer to one message */
    CANMessageControl_t      *msgCtrl_p;
    IOMCanRoutingPortTbl_t   *iomCanPort;
    const CanMessageConfig_t *msginfo_p;     /* pointer to processed corresponding message info structure */
    CANMessage_t              inCanMsgBuffer[IOEN_MAX_CAN_MESSAGES_PER_CYCLE];

    msginfo_p   = (const CanMessageConfig_t *) 0;
    msgCtrl_p   = (CANMessageControl_t *) 0;

    /* Read all CAN messages from the given CAN bus */
    RECEIVE_QUEUING_MESSAGE(apexCanBusId, (SYSTEM_TIME_TYPE)0, (MESSAGE_ADDR_TYPE)&inCanMsgBuffer[0], &len, &ret);
    CHECK_CODE("RECEIVE_CAN_MESSAGE", ret);

    if ((len > 0) AND (ret == NO_ERROR))
    {
        nofCanMsg = len / sizeof(CANMessage_t);

        for (msgIdx = 0; msgIdx < nofCanMsg; msgIdx++)
        {
            canMsg_p = &inCanMsgBuffer[msgIdx];

            /* Convert the byte order of the CAN ID, if required */
            canMsg_p->canId = HTON32(canMsg_p->canId);

            /* find the message configuration, by looking up the CAN ID */
            ioen_imcLookupMessage(canMsg_p->canId, &msginfo_p, &msgCtrl_p);

            if ((msginfo_p) AND (msgCtrl_p))
            {
                /* Message is configured */

                if (    /* Message on same bus, use it without changing source selection */
                       (msgCtrl_p->lastbus == apexCanBusId)

                        /* First time, use it, zero is not valid for an APEX port ID (QUEUING_PORT_ID_TYPE) */
                    OR ((msgCtrl_p->lastbus == 0))

                        /* Current message on the selected source has timed out, */
                        /* The message has been received on the other bus,       */
                        /* change source to select message from the other bus    */
                    OR (msgCtrl_p->msgValidCtrl.cycle == 0)
                   )
                {
                    /* Process current CAN bus message if:                                         */
                    /*   1. This is the currently selected CAN bus for this message                */
                    /*   OR                                                                        */
                    /*   2. There is no message on the currently selected CAN bus for longer than  */
                    /*      (n * messages rate), and there is a good message on the other CAN bus. */

                    if (msgCtrl_p->lastbus != apexCanBusId)
                    {
                        /* Change of source selection, process the message */
                        newMsg = TRUE;
                    }
                    else
                    {
                        /* Check if new Message was received */
                        newMsg = ioen_imcNewMessage(msgCtrl_p, canMsg_p);
                    }

                    if (newMsg == TRUE)
                    {
                        /* Save new message for filtering next time */
                        memcpy (&msgCtrl_p->prevMsg, canMsg_p, sizeof(CANMessage_t));

                        /* Validate and route all CAN signals for this message */
                        ioen_imcProcessCanMessage(msgCtrl_p, msginfo_p, canMsg_p);

                        if (msgCtrl_p->msgValidCtrl.validity.current.value == IOEN_GLOB_DATA_VALID)
                        {
                            /* Set this CAN bus as the currently selected bus for this message, and reset the timeout */
                            msgCtrl_p->lastbus            = apexCanBusId;
                            msgCtrl_p->msgValidCtrl.cycle = msgCtrl_p->msgValidCtrl.limitCycleInvalid;  /* Message is fresh, reset the counter */
                        }
                        else
                        {
                            /* No action, as designed                     */
                            /* Message was invalid, do not reset counters */
                        }

                    }
                    else
                    {
                        /* message is filtered, update its freshness */
                        if (msgCtrl_p->msgValidCtrl.validity.current.value == IOEN_GLOB_DATA_VALID)
                        {
                            /* Message is still fresh, reset the counter */
                            msgCtrl_p->msgValidCtrl.cycle = msgCtrl_p->msgValidCtrl.limitCycleInvalid;
                        }
                        else
                        {
                            /* No action as designed */
                            /* Filtered message is still invalid */
                        }

                    }

                }
                else
                {
                    /* else message is discarded */
                }
            }
            else
            {
                /* else message is discarded if not re-routed*/
            }

            /* Check if the message has to be routed to another partition */
            portIdx    = 0;
            iomCanPort = &ioen_mrcConfigRoutingTable_p[0];

            while (iomCanPort->canType != IOEN_CAN_PORT_TYPE_TERMINATOR)
            {
                /* Remove FS from CAN ID before comparision */
                if (    ((canMsg_p->canId BIT_AND A825_CAN_ID_IGNORE_FS_MASK) == (iomCanPort->canId BIT_AND A825_CAN_ID_IGNORE_FS_MASK) )
                    AND (iomCanPort->canType == IOEN_CAN_PORT_TYPE_ROUTING_TX)
                   )
                {
                    if (iomCanPort->nofOutMsg < IOEN_MAX_CAN_MESSAGES_PER_CYCLE)
                    {
                        /* Add to list to send on a Queuing Port */
                        memcpy (&iomCanPort->outMsg[iomCanPort->nofOutMsg], canMsg_p, sizeof(CANMessage_t));
                        iomCanPort->nofOutMsg++;
                    }
                    else
                    {
                        /* No action as designed */
                        /* iomCanPort->nofOutMsg is set to zero at the start of each IOM cycle */
                        /* and incremented only as above */
                    }
                    break;
                }
                else
                {
                    /* No action as designed */
                }
                portIdx++;
                iomCanPort = &ioen_mrcConfigRoutingTable_p[portIdx];
            }
        }

        /* Transmit any CAN Messages configured to be routed to another IOM application */
        portIdx    = 0;
        iomCanPort = &ioen_mrcConfigRoutingTable_p[0];
        while (iomCanPort->canType != IOEN_CAN_PORT_TYPE_TERMINATOR)
        {
            if (iomCanPort->nofOutMsg > 0)
            {
                /* Send Message on a Queuing Port */
                SEND_QUEUING_MESSAGE   (iomCanPort->apexCanBusId, 
                                        (MESSAGE_ADDR_TYPE)&iomCanPort->outMsg[0],
                                        (sizeof(CANMessage_t) * iomCanPort->nofOutMsg),
                                        0,
                                        &ret);
                CHECK_CODE("RECEIVE_QUEUING_MESSAGE", ret);

                iomCanPort->nofOutMsg = 0;
            }
            else
            {
                /* No action as designed */
            }
            portIdx++;
            iomCanPort = &ioen_mrcConfigRoutingTable_p[portIdx];
        }

    }
    else
    {
        /* No action as designed */
        /* No messages received  */
    }

}


