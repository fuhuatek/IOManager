/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_AfdxApi.c
 *
 * FILE DESCRIPTION:
 *   This module interfaces to the system to create the output ports and write the output messages.
 *
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ioen_IomLocal.h"

#ifdef IOM_DEBUG
unsigned int delayTick = 0;
#endif


/******************************************************************
 * FUNCTION NAME:
 *   ioen_aaInitOutputBuffer
 *
 * DESCRIPTION:
 *   This function initialises the AFDX message output buffer
 *   to ensure unused bits are set to zero.
 *
 * INTERFACE:
 *   Global Data:
 *      ioen_outMsgBuffer
 *
 ******************************************************************/
void ioen_aaInitOutputBuffer (
    /* IN     */ const IOMConfigHeader_t   * const iom_p,
    /*    OUT */       Byte_t              * const outMessageBuffer_p
)
{
    UInt32_t            idx;
    AfdxMessageInfo_t  *msginfo_p;
    void               *messageStart_p;     /* pointer to start of message in message buffer */

    /* App period is OK, we can create all the ports and schedule them */
    msginfo_p = AFDX_OUTPUT_MSG_FIRST(iom_p);

    for(idx = 0; idx < iom_p->afdxOutputMessageCount; idx++)
    {
        messageStart_p = outMessageBuffer_p + msginfo_p->messageHdrOffset;
        memset (messageStart_p, 0, msginfo_p->messageLength);  /* Make sure unused data is set to zero */

        msginfo_p++;
    }
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_createTxPortsAfdx
 *
 * DESCRIPTION:
 *   This function creates the A664 APEX Transmit ports
 *
 * INTERFACE:
 *   Global Data      :  ioen_outputPortsInfoAfdx
 *
 *   In:  iom_p       : pointer to IOM configuration
 *   In:  appPeriodMs : Periodic time of process in millisecs
 *
 ******************************************************************/
void ioen_createTxPortsAfdx (
    /* IN     */ const IOMConfigHeader_t   * const iom_p,
    /* IN     */ const Float32_t                   appPeriodMs
)
{
    UInt32_t            idx;
    AfdxMessageInfo_t  *msg_p;
    RETURN_CODE_TYPE    ret;
    UInt32_t            nbPeriod;
    Char_t             *portname;

#ifdef IOM_DEBUG    
	delayTick=sysClkRateGet()/1000;	
#endif
	    
    /* Clear dynamic data */
    memset ((void *) &ioen_outputPortsInfoAfdx, 0x00, sizeof(ioen_outputPortsInfoAfdx));

    /* App period is OK, we can create all the ports and schedule them */
    msg_p = AFDX_OUTPUT_MSG_FIRST(iom_p);

    for(idx = 0; idx < iom_p->afdxOutputMessageCount; idx++)
    {
        /* Port names from string table */
        portname = IOEN_PORT_NAME(iom_p, msg_p);

        /* Initialize the countdown value for scheduler */
        nbPeriod = (UInt32_t)( ((Float32_t)msg_p->refreshPeriod / appPeriodMs) + 0.99); /* plus 0,99 for rounding up */
        if (nbPeriod != 0)
        {
            ioen_outputPortsInfoAfdx.port[idx].countdownInit = nbPeriod;
        }
        else
        {
            /* Message rate is lower than the App Period, send the message every cycle */
            ioen_outputPortsInfoAfdx.port[idx].countdownInit = 1;
        }

        ioen_outputPortsInfoAfdx.port[idx].countdownValue = (SInt32_t)ioen_outputPortsInfoAfdx.port[idx].countdownInit;

        if (msg_p->queueLength == 0)
        {
            /* Sampling Port */
            CREATE_SAMPLING_PORT(
                portname,                /* port name      */
                msg_p->messageLength,    /* message length */
                SOURCE,
                (SYSTEM_TIME_TYPE) (msg_p->refreshPeriod) * MS_TO_NS,
                (SAMPLING_PORT_ID_TYPE*)(&ioen_outputPortsInfoAfdx.port[idx].apexPortId),
                &ret
            );
#ifdef IOM_DEBUG
            printf("CREATE SAMPLING WRITE portname:%s:\n",portname);
            printf("                      msglength: %d\n", msg_p->messageLength);
            printf("                      queuelength: %d\n", msg_p->queueLength);
            printf("                      source: %d\n", SOURCE);
            printf("                      rate: %d\n", msg_p->messageRate);
            printf("                      port ID: %d\n",ioen_outputPortsInfoAfdx.port[idx].apexPortId);
            printf("                      ret: %d\n",ret);
#endif
            CHECK_CODE("CREATE_SAMPLING_PORT", ret);
        }
        else
        {
            /* Queuing Port */
            CREATE_QUEUING_PORT(
                portname,                /* port name      */
                msg_p->messageLength,    /* message length */
                msg_p->queueLength    ,
                SOURCE,
                FIFO,
                (QUEUING_PORT_ID_TYPE*)(&ioen_outputPortsInfoAfdx.port[idx].apexPortId),
                &ret
            );
#ifdef IOM_DEBUG
            printf("CREATE QUEUING WRITE portname:%s:\n",portname);
            printf("                     msglength: %d\n", msg_p->messageLength);
            printf("                     queuelength: %d\n", msg_p->queueLength);
            printf("                     source: %d\n", SOURCE);
            printf("                     fifo: %d\n", FIFO);
            printf("                     port ID: %d\n",ioen_outputPortsInfoAfdx.port[idx].apexPortId);
            printf("                     ret: %d\n",ret);
#endif
            CHECK_CODE("CREATE_QUEUING_PORT", ret);
        }

        msg_p++;
    }
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_aaInitSignalConfirmation
 *
 * DESCRIPTION:
 *   This function initialises the confirmation structure for each input signal
 *
 * INTERFACE:
 *
 *   Global Data      :  ioen_imaMsgValidity
 *
 *   In:  iom_p       : pointer to IOM Configuration
 *   In:  appPeriodMs : Periodic time of process in millisecs
 *
 ******************************************************************/
void ioen_aaInitSignalConfirmation (
    /* IN     */ const IOMConfigHeader_t   * const iom_p,
    /* IN     */ const Float32_t                   appPeriodMs
)
{
    UInt32_t                        idx;        /* index of processed dataset   */
    UInt32_t                        paramIdx;
    InputDatasetHeader_t           *dataset_p;  /* pointer to processed dataset */
    const ValidityConfig_t         *validityLogic_p;
    const ParamMappingConfig_t     *parConfig_p;
    const InputSignalConfig_t      *sigConfig_p; /* Address of the Input Signal Configuration */

    /* Process all datasets that have only one source */
    /* Get first dataset */
    dataset_p = (InputDatasetHeader_t *) ((void *)(iom_p) + iom_p->afdxInputDatasetStart);

    paramIdx = 0;
    for (idx = 0; (idx < iom_p->afdxInputDatasetCount); idx++)
    {

        /* first logic starts after dataset header */
        validityLogic_p = (const ValidityConfig_t *)(dataset_p + 1);

        /* first parameter starts after logicSize bytes */
        parConfig_p = (const ParamMappingConfig_t *)((void *)validityLogic_p + dataset_p->logicSize);

        /* Obtain the address of the first signal configuration */
        sigConfig_p = (const InputSignalConfig_t *) ((Byte_t *)parConfig_p + sizeof(ParamMappingConfig_t));

        /* Initialise the confirmation structure, with confirmation of one, eg. for LIC source selection */
        ioen_ConfirmInitParam (iom_p,
                               appPeriodMs,
                               0,
                               0,
                               parConfig_p->numSources,
                               ioen_inObjectCtrl[paramIdx]);

/*
         Initialise the confirmation structure 
        ioen_ConfirmInitParamSignal   (iom_p,
                                       appPeriodMs,
                                       parConfig_p->numSources,
                                       sigConfig_p,
                                       ioen_inObjectCtrl[paramIdx]);
*/

        /* Increment pointer to next dataset */
        dataset_p = (void *)dataset_p + dataset_p->datasetSize;
        paramIdx++;
    }

    /* Get validities of all datasets that have multiple sources */
    dataset_p = (InputDatasetHeader_t *) ((void *)(iom_p) + iom_p->afdxInputDatasetMultiStart);

    paramIdx = iom_p->afdxInputDatasetCount;
    for (idx = 0; (idx < iom_p->afdxInputDatasetMultiCount); idx++)
    {
        /* first logic starts after dataset header */
        validityLogic_p = (const ValidityConfig_t *)(dataset_p + 1);

        /* first parameter starts after logicSize bytes */
        parConfig_p = (const ParamMappingConfig_t *)((void *)validityLogic_p + dataset_p->logicSize);

        /* Obtain the address of the first signal configuration */
        sigConfig_p = (const InputSignalConfig_t *) ((Byte_t *)parConfig_p + sizeof(ParamMappingConfig_t));

        /* Initialise the confirmation structure */
        ioen_ConfirmInitParamSignal   (iom_p,
                                       appPeriodMs,
                                       parConfig_p->numSources,
                                       sigConfig_p,
                                       ioen_inObjectCtrl[paramIdx]);

        /* Increment pointer to next dataset */
        dataset_p = (void *)dataset_p + dataset_p->datasetSize;
        paramIdx++;
    }

}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_createRxPortsAfdx
 *
 * DESCRIPTION:
 *   This function creates the AFDX input ports
 *
 * INTERFACE:
 *
 *   Global Data      :  ioen_afdxMsgCtrl
 *   Global Data      :  ioen_imaMsgValidity
 *
 *   In:  iom_p       : pointer to IOM Configuration
 *   In:  appPeriodMs : Periodic time of process in millisecs
 *
 ******************************************************************/
void ioen_createRxPortsAfdx (
    /* IN     */ const IOMConfigHeader_t   * const iom_p,
    /* IN     */ const Float32_t                   appPeriodMs
)
{
    UInt32_t            idx;
    Char_t             *portname;
    AfdxMessageInfo_t  *msg_p;
    RETURN_CODE_TYPE    ret;

    /* Clear dynamic data */
    memset ((void *) &ioen_afdxMsgCtrl, 0x00, sizeof(ioen_afdxMsgCtrl));

    msg_p = AFDX_INPUT_MSG_FIRST(iom_p);

    for(idx = 0; idx < iom_p->afdxInputMessageCount; idx++)
    {
        /* port name from string table */
        portname = IOEN_PORT_NAME(iom_p, msg_p);

        if (msg_p->queueLength == 0)
        {
            /* Sampling */
            CREATE_SAMPLING_PORT(
                portname,                /* port name      */
                msg_p->messageLength,    /* message length */
                DESTINATION,
                (SYSTEM_TIME_TYPE) (msg_p->refreshPeriod) * MS_TO_NS,
                (SAMPLING_PORT_ID_TYPE*)(&ioen_afdxMsgCtrl[idx].apexPortId),
                &ret
            );

#ifdef IOM_DEBUG
            PRINT("CREATE SAMPLING READ portname:%s:\n",portname);
            PRINT("                     msglength: %d\n", msg_p->messageLength);
            PRINT("                     queuelength: %d\n", msg_p->queueLength);
            PRINT("                     destination: %d\n", DESTINATION);
            PRINT("                     rate: %d\n", msg_p->messageRate);
            PRINT("                     port ID: %d\n", ioen_afdxMsgCtrl[idx].apexPortId);
            PRINT("                     ret: %d\n",ret);
#endif
            CHECK_CODE("CREATE_SAMPLING_PORT", ret);
        }
        else
        {
            /* Queuing */
            CREATE_QUEUING_PORT(
                portname,                /* port name      */
                msg_p->messageLength,    /* message length */
                msg_p->queueLength    ,
                DESTINATION,
                FIFO,
                (QUEUING_PORT_ID_TYPE*)(&ioen_afdxMsgCtrl[idx].apexPortId),
                &ret);

#ifdef IOM_DEBUG
            PRINT("CREATE QUEUING READ portname:%s:\n",portname);
            PRINT("                    msglength: %d\n", msg_p->messageLength);
            PRINT("                    queuelength: %d\n", msg_p->queueLength);
            PRINT("                    destination: %d\n", DESTINATION);
            PRINT("                    fifo: %d\n", FIFO);
            PRINT("                    port ID: %d\n",ioen_afdxMsgCtrl[idx].apexPortId);
            PRINT("                    ret: %d\n",ret);
#endif
            CHECK_CODE("CREATE_QUEUING_PORT", ret);
        }

        /* Calculate number of cycles for next out of date message, rounding up */
        ioen_ConfirmInitMsg (msg_p->validTime,
                             msg_p->invalidTime,
                             appPeriodMs,
                             &ioen_afdxMsgCtrl[idx].objCtrl);

        /* Initialise the start cycle, to schedule reads for this message */
        ioen_afdxMsgCtrl[idx].readCycle = msg_p->schedOffset;

        msg_p++;
    }


}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_processOutputAfdx
 *
 * DESCRIPTION:
 *   This function processes one frame of output data:
 *         Traverse all parameters and convert/copy data from parameters to output messages
 *         in the message buffer according to data conversion
 *
 * INTERFACE:
 *
 *   Global Data:   ioen_iomConfig_p
 *
 ******************************************************************/
void ioen_processOutputAfdx (
       /*        */       void
)
{
    UInt32_t                 idx;        /* index of processed dataset    */
    OutputDatasetConfig_t   *dataset_p;  /* pointer to processed dataset  */
    const IOMConfigHeader_t *iom_p;

    if (ioen_outParamBuffer != NULL_PTR)
    {
        /* Get pointer to IOM Configuration */
        iom_p = ioen_iomConfig_p;

        /* AFDX Output   */
        /* First dataset */
        dataset_p = AFDX_OUTPUT_DS_FIRST(iom_p);

        for (idx = 0; idx < iom_p->afdxOutputDatasetCount; idx++)
        {
            ioen_omaProcessOutputDatasetAfdx(dataset_p);

            /* Next dataset */
            dataset_p = (void *)dataset_p + dataset_p->size;
        }
    }
    else
    {
        /* No action as designed, no Output parameter buffer defined */
    }
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_writeMessagesAfdx
 *
 * DESCRIPTION:
 *   This function transmits the A664 APEX ports
 *
 * INTERFACE:
 *   Global Data:   ioen_outputPortsInfoAfdx
 *   Global Data:   ioen_iomConfig_p
 *   Global Data:   ioen_outMsgBuffer
 *   Global Data:   ioen_outParamBuffer
 *
 ******************************************************************/
void ioen_writeMessagesAfdx (
       /*        */       void
)
{
    UInt32_t            idx;                /* index of write message                                    */
    AfdxMessageInfo_t * msginfo_p;          /* pointer to processed corresponding message info structure */
    RETURN_CODE_TYPE    ret;                /* return code                                               */
    void              * messageStart_p;     /* pointer to start of message to send                       */
    const IOMConfigHeader_t * iom_p;

    if (ioen_outParamBuffer != NULL_PTR)
    {
        /* Get pointer to IOM Configuration */
        iom_p = ioen_iomConfig_p;

        msginfo_p = (AfdxMessageInfo_t *)((void *) iom_p + iom_p->afdxOutputMessageStart);

        for (idx = 0; idx < iom_p->afdxOutputMessageCount; idx++)
        {
            if (ioen_outputPortsInfoAfdx.port[idx].countdownValue > 1)
            {
                /* If the port countdown value has not reached 0, it is too early to send the message, */
                /* decrement the countdown and move to the next message                                */
                ioen_outputPortsInfoAfdx.port[idx].countdownValue--;
            }
            else
            {
                /* It's time to send the message, reinit the corresponding countdown */
                ioen_outputPortsInfoAfdx.port[idx].countdownValue = (SInt32_t)ioen_outputPortsInfoAfdx.port[idx].countdownInit;

                /* Compute the message start */
                messageStart_p = ioen_outMsgBuffer + msginfo_p->messageHdrOffset;

                /* Set Freshness counter */
                ioen_UtilSetFc (messageStart_p, msginfo_p, &ioen_outputPortsInfoAfdx.port[idx].fcValue);

                /* Set CRC last, in case the freshness counter is to be included in the CRC */
                ioen_UtilSetCrc (messageStart_p, msginfo_p);

                if (msginfo_p->queueLength == 0)
                {
                    /* Send Message on a Sampling Port */
                    WRITE_SAMPLING_MESSAGE(ioen_outputPortsInfoAfdx.port[idx].apexPortId, (void *) messageStart_p, msginfo_p->messageLength, &ret);
                    CHECK_CODE("WRITE_SAMPLING_MESSAGE", ret);
                }
                else
                {
                    /* Send Message on a Queuing Port */
                    SEND_QUEUING_MESSAGE(ioen_outputPortsInfoAfdx.port[idx].apexPortId, (void *) messageStart_p, msginfo_p->messageLength, 0, &ret);
                    CHECK_CODE("SEND_QUEUING_MESSAGE", ret);
                }
            }

            msginfo_p++;
        }
    }
    else
    {
        /* No action as designed, no Output parameter buffer defined */
    }
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_processInputAfdxA429
 *
 * DESCRIPTION:
 *   This function processes one frame of input data:
 *         Traverse all datasets and convert/copy signal from messages to parameters
 *         in the parameter buffer according to source selection and data conversion
 *
 * INTERFACE:
 *
 *   Global Data:   ioen_iomConfig_p
 *
 ******************************************************************/
void ioen_processInputAfdxA429 (
       /*        */       void
)
{
    UInt32_t                  idx;        /* index of processed dataset   */
    UInt32_t                  paramIdx;
    InputDatasetHeader_t    * dataset_p;  /* pointer to processed dataset */
    const IOMConfigHeader_t * iom_p;

    /* Get pointer to IOM Configuration */
    iom_p = ioen_iomConfig_p;

    /* Process all datasets that have only one source */
    dataset_p = (InputDatasetHeader_t *) ((void *)(iom_p) + iom_p->afdxInputDatasetStart);  /* Get first dataset */

    paramIdx = 0;
    for (idx = 0; (idx < iom_p->afdxInputDatasetCount); idx++)
    {
        /* Process and route to application, all datasets with a single source */
        ioen_imaProcessDataset(dataset_p, ioen_inObjectCtrl[paramIdx]);

        /* Increment pointer to next dataset */
        dataset_p = (void *)dataset_p + dataset_p->datasetSize;
        paramIdx++;
    }

    /* Get validities of all datasets that have multiple sources */
    dataset_p = (InputDatasetHeader_t *) ((void *)(iom_p) + iom_p->afdxInputDatasetMultiStart);

    paramIdx = iom_p->afdxInputDatasetCount;
    for (idx = 0; (idx < iom_p->afdxInputDatasetMultiCount); idx++)
    {
        /* Process and route to source buffer, all datasets wita multiple sources */
        ioen_imaProcessDatasetSources (dataset_p, ioen_inObjectCtrl[paramIdx]);

        /* Increment pointer to next dataset */
        dataset_p = (void *)dataset_p + dataset_p->datasetSize;
        paramIdx++;
    }

    /* Calculate any source selection objects that are only associated with a set (not an output parameter) */
    ioen_ssCalcSourceSelectionObjects (iom_p);

    /* Select a source for each selection set */
    ioen_ssPerformSourceSelection (iom_p);

    /* Route the data for all datasets that have multiple sources */
    dataset_p = (InputDatasetHeader_t *) ((void *)(iom_p) + iom_p->afdxInputDatasetMultiStart);
    paramIdx  = iom_p->afdxInputDatasetCount;

    for (idx = 0; (idx < iom_p->afdxInputDatasetMultiCount); idx++)
    {
        /* Using the selected sources, route to application, all datasets with multiple sources */
        ioen_imaProcessDatasetValue(iom_p, dataset_p, ioen_inObjectCtrl[paramIdx]);

        /* Increment pointer to next dataset */
        dataset_p = (void *)dataset_p + dataset_p->datasetSize;
        paramIdx++;
    }

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_aaNewMessageAfdx
 *
 * DESCRIPTION:
 *   This function checks if the message just read is valid.
 *
 * INTERFACE:
 *
 *   In    :  msginfo_p        : pointer to message config
 *   In    :  messageStart_p   : pointer to message data
 *   In Out:  msgCtrl_p        : pointer to message confirmation control objects
 *
 ******************************************************************/
void ioen_aaNewMessageAfdx (
    /* IN     */ const AfdxMessageInfo_t        * const msginfo_p,
    /* IN     */ const void                     * const messageStart_p,
    /* IN OUT */       IomMessageCtrlData_t     * const msgCtrl_p
)
{
    Bool_t               crcOk;
    Bool_t               fcOk;

    /* Check CRC */
   /* crcOk = ioen_UtilCheckCrc (messageStart_p, msginfo_p);*/

    /* Check Freshness counter */
   /* fcOk = ioen_UtilCheckFc (messageStart_p, msginfo_p, &msgCtrl_p->fcCtrl);*/
    crcOk = TRUE;
    fcOk  = TRUE;
    
    if (    (crcOk == TRUE)
        AND (fcOk  == TRUE)
       )
    {
        /* APEX message received and still valid */
        ioen_ConfirmSetNewMsgData (&msgCtrl_p->objCtrl);
    }
    else
    {
        /* No action as designed */
        /* Invalid CRC or FC     */
    }
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_readMessagesAfdx
 *
 * DESCRIPTION:
 *   This function reads the AFDX input ports as defined in the IOM configuration.
 *
 * INTERFACE:
 *
 *   Global Data      :  ioen_afdxMsgCtrl
 *   Global Data      :  ioen_iomConfig_p
 *   Global Data      :  ioen_inMsgBuffer
 *
 ******************************************************************/
void ioen_readMessagesAfdx (
   /*        */       void
)
{
    UInt32_t             idx;             /* index of read message                                     */
    AfdxMessageInfo_t   *msginfo_p;       /* pointer to processed corresponding message info structure */
    void                *messageStart_p;
    AfdxMessageHeader_t *msgHdr_p;
    MESSAGE_SIZE_TYPE    len;
    RETURN_CODE_TYPE     ret;
    VALIDITY_TYPE        validity;


    msginfo_p = AFDX_INPUT_MSG_FIRST(ioen_iomConfig_p);

    for (idx = 0;
         idx < ioen_iomConfig_p->afdxInputMessageCount;
         idx++)
    {
        msgHdr_p       = (AfdxMessageHeader_t *)(ioen_inMsgBuffer + msginfo_p->messageHdrOffset);
        messageStart_p = ioen_inMsgBuffer + msginfo_p->messageHdrOffset + sizeof(AfdxMessageHeader_t);

        /* Check if message is scheduled to be read this cycle */
	
	  if (msginfo_p->queueLength == 0)
            {
			  
                /* Sampling message configured, use APEX interface to read it */
                READ_SAMPLING_MESSAGE(ioen_afdxMsgCtrl[idx].apexPortId, messageStart_p, &len, &validity, &ret);

                if (    (ret      == NO_ERROR)
                    AND (validity == VALID   )
                   )
                {
                    /* Extra checks if the message is valid */
                    ioen_aaNewMessageAfdx (msginfo_p, messageStart_p, &ioen_afdxMsgCtrl[idx]);
                }
                else
                {
                    /* No message received this cycle */
                    /* No action as designed          */
                }
            }
		else{
		     if (ioen_afdxMsgCtrl[idx].readCycle == 0)
			 {
			  RECEIVE_QUEUING_MESSAGE (ioen_afdxMsgCtrl[idx].apexPortId, (SYSTEM_TIME_TYPE)0, messageStart_p, &len, &ret);

                if (     (len > 0)
                     AND ((ret == NO_ERROR) OR (ret == INVALID_CONFIG))
                   )
                {
                    /* NB: INVALID_CONFIG means a message was received,                   */ 
                    /* but a further message was lost because the queue size is too small */

                    /* Extra checks if the message is valid */
                    ioen_aaNewMessageAfdx (msginfo_p, messageStart_p, &ioen_afdxMsgCtrl[idx]);
                }
                else
                {
                    /* No message received this cycle */
                    /* No action as designed          */
                }
			 }
			else
               {
                 if (ioen_afdxMsgCtrl[idx].readCycle <= msginfo_p->schedRate)
                {
                   /* Not time to read message this cycle, decrement the cycle counter */
                   ioen_afdxMsgCtrl[idx].readCycle--;
                }
                else
               {
                /* Robustness, reset counter */
                   ioen_afdxMsgCtrl[idx].readCycle = msginfo_p->schedRate;
                }
          }
			 
		}
       

        /* Update freshness of message */
        ioen_ConfirmMsgFreshness (&ioen_afdxMsgCtrl[idx].objCtrl, &msgHdr_p->freshness);

        msginfo_p++;
    }
}


