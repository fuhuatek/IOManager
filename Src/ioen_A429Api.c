/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_A429Api.c
 *
 * FILE DESCRIPTION:
 *   This module interfaces to the system to create the A429 input ports and read the A429 input messages.
 *
***************************************************************/

#include <string.h>
#include <stdio.h>
#include <apex/apexLib.h>

#include "ioen_IomLocal.h"


extern IOMA429RoutingPortTbl_t *ioen_429ConfigRoutingTable_p;

/******************************************************************
 * FUNCTION NAME:
 *   ioen_createPortsA429
 *
 * DESCRIPTION:
 *   This function creates A429 Rx ports, and initialises the runtime data structures
 *
 * INTERFACE:
 *   Global Data      :  ioen_a429MsgControl
 *   Global Data      :  ioen_inMsgBuffer
 *
 *   In:      iom_p         : pointer to IOM configuration
 *   In:      appPeriodMs   : Periodic time of process in millisecs
 *
 ******************************************************************/
void ioen_createPortsA429 (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN  */       IOMA429RoutingPortTbl_t   * const iomA429RtTbl_p,
    /* IN     */ const Float32_t                        appPeriodMs

)
{
 /*   UInt32_t                   portIdx;*/
    UInt32_t                   tableIdx;
    UInt32_t                   labelIdx;
    RETURN_CODE_TYPE           ret;
/*    Char_t                    *portname;*/
    A429PortInfo_t            *portInfo_p;       /* pointer to port info in config             */
    A429MessageInfo_t         *msgInfo_p;        /* pointer to message (label) info in config  */
    IOMA429RoutingPortTbl_t   *iomA429Port = &iomA429RtTbl_p[0];
    /* Initialise runtime data */
    /* Reset runtime data      */
    memset (&ioen_a429MsgControl, 0, sizeof(ioen_a429MsgControl));
    ioen_429ConfigRoutingTable_p = iomA429RtTbl_p;
    /* Get First Port in config */
    portInfo_p = (A429PortInfo_t *)((void *)(iom_p) + iom_p->a429InputPortStart);
    tableIdx=0;
    /* create all A429 ports for this partition */
    while (iomA429Port->Type != IOEN_CAN_PORT_TYPE_TERMINATOR)
    {
    	switch (iomA429Port->Type)
    	{
    	case IOEN_CAN_PORT_TYPE_RX_A :
    	                /* CAN bus input for bus A for this partition */
    	                CREATE_QUEUING_PORT(
    	                	iomA429Port->portName,
    	                    4,                    /* message length */
    	                    iomA429Port->queueLength,
    	                    DESTINATION,
    	                    FIFO,
    	                    &ioen_a429MsgControl[iomA429Port->portindex].apexPortId,
    	                    &ret
    	                );
                       
                        printf("%s create %d,apexportid%d,transportid%d\n",iomA429Port->portName,ret,ioen_a429MsgControl[iomA429Port->portindex].apexPortId,ioen_429ConfigRoutingTable_p[0].transport_portindex);
                        
    	                CHECK_CODE("CREATE_QUEUING_PORT", ret);
    	                break;
    	 case IOEN_CAN_PORT_TYPE_ROUTING_TX:
    		 CREATE_QUEUING_PORT(
    				              iomA429Port->portName,
    		    	              4,     /* message length */
    		    	              iomA429Port->queueLength,
    		    	              SOURCE,
    		    	              FIFO,
    		    	              &iomA429Port->apexA429portId,
    		    	              &ret
    		    	                );
    		                      
    		                        
    		                       printf("%s create ret:%d,apexportid:%d\n",iomA429Port->portName,ret,iomA429Port->apexA429portId);
    		                        
    		    	               
    		    	                CHECK_CODE("CREATE_QUEUING_PORT", ret);
    		    	                break;
    	 default:
    	                 /* No action, as designed */
    	             break;
    	}
    	tableIdx++;
    	iomA429Port= &iomA429RtTbl_p[tableIdx];
    	
    }
    
  
    /* Get First message in config */
    msgInfo_p = (A429MessageInfo_t *)((void *)(iom_p) + iom_p->a429InputMessageStart);

    /* create lookup table for quick access to configuration */
    for (labelIdx = 0; (labelIdx < iom_p->a429InputMessageCount); labelIdx++)
    {
        ioen_a429MsgControl[msgInfo_p->port].data[msgInfo_p->code][msgInfo_p->sdi].msgInfo_p = msgInfo_p;

        /* Initialise freshness counters */
        ioen_ConfirmInitMsg (msgInfo_p->validTime,
                             msgInfo_p->invalidTime,
                             appPeriodMs,
                             &ioen_a429MsgControl[msgInfo_p->port].data[msgInfo_p->code][msgInfo_p->sdi].ctrl);

        /* Increment pointer to next label */
        msgInfo_p = (void *)msgInfo_p + sizeof(A429MessageInfo_t);
    }

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_updateMessageFreshness
 *
 * DESCRIPTION:
 *   This function updates the freshness of all labels
 *
 * INTERFACE:
 *   Global Data      :  ioen_iomConfig_p
 *   Global Data      :  ioen_inMsgBuffer
 *   Global Data      :  ioen_a429MsgControl
 *
 *
 ******************************************************************/
void ioen_updateMessageFreshness (
    /*        */ void
)
{
    UInt32_t               labelIdx;
    A429MessageInfo_t     *msgInfo_p;       /* pointer to A429 Label config   */
    A429PortInfo_t        *portInfo_p;      /* pointer to A429 port config    */
    A429MessageCtrlData_t *msgCtrl_p;       /* pointer to A429 runtime data   */
    A429MessageData_t     *dataStart_p;     /* pointer to A429 message buffer */

    /* Get First message in config */
    msgInfo_p     = (A429MessageInfo_t *) ((void *)(ioen_iomConfig_p) + ioen_iomConfig_p->a429InputMessageStart);
    portInfo_p    = (A429PortInfo_t *)    ((void *)(ioen_iomConfig_p) + ioen_iomConfig_p->a429InputPortStart);
    dataStart_p   = (A429MessageData_t *) ((void *)(ioen_inMsgBuffer + portInfo_p->messageOffset));

    /* Update freshness of all configured A429 labels */
    for (labelIdx = 0; (labelIdx < ioen_iomConfig_p->a429InputMessageCount); labelIdx++)
    {
        msgCtrl_p = &ioen_a429MsgControl[msgInfo_p->port].data[msgInfo_p->code][msgInfo_p->sdi];

        if (msgCtrl_p->msgInfo_p != 0)
        {
            /* Label config valid */
            /* Check freshness */
            ioen_ConfirmMsgFreshness (&msgCtrl_p->ctrl, &dataStart_p[msgInfo_p->code].data[msgInfo_p->sdi].freshness);
        }
        else
        {
            /* No action as designed, robustness against an invalid label */
        }

        /* Increment pointer to next label */
        msgInfo_p = (void *)msgInfo_p + sizeof (A429MessageInfo_t);
    }

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_readMessagesA429
 *
 * DESCRIPTION:
 *   This function reads messages from IO and process the data
 *
 * INTERFACE:
 *   Global Data      :  ioen_inMsgBuffer
 *   Global Data      :  ioen_iomConfig_p
 *   Global Data      :  ioen_a429MsgControl
 *
 ******************************************************************/
void ioen_readMessagesA429 (
    /*        */ void
)
{
    UInt32_t             a429Label;
    UInt32_t             idx;
    UInt32_t             portIdx;
    RETURN_CODE_TYPE     ret;
    MESSAGE_SIZE_TYPE    len;

    for (portIdx = 0; (portIdx < ioen_iomConfig_p->a429InputPortCount); portIdx++)
    {
        for (idx = 0; (idx < IOEN_A429_READS_PER_PORT_PER_CYCLE); idx++)
        {
            /* Read a A429 word received on this port (channel) */
            RECEIVE_QUEUING_MESSAGE (ioen_a429MsgControl[portIdx].apexPortId,
                                     (SYSTEM_TIME_TYPE)0,
                                     (MESSAGE_ADDR_TYPE) &a429Label,
                                     &len,
                                     &ret
                                    );

            if (    (len == 4)
                AND ((ret == NO_ERROR) OR (ret == INVALID_CONFIG))
               )
            {
                /* A429 word received, process A429 word                                             */
                /* NB: INVALID_CONFIG means the input queue is full, but we still received a message */
                ioen_im4ProcessA429Message (portIdx, a429Label);
                if(ioen_429ConfigRoutingTable_p[portIdx].transport_portindex != -1)
                {
                	SEND_QUEUING_MESSAGE(ioen_429ConfigRoutingTable_p[ioen_429ConfigRoutingTable_p[portIdx].transport_portindex].apexA429portId, (MESSAGE_ADDR_TYPE) &a429Label, 4, 0, &ret);
                 	CHECK_CODE("SEND_QUEUING_MESSAGE", ret);
                }
            }
            else
            {
                /* No action as designed */
            }
            
        }

    }

    /* Update freshness of all configured A429 messages (labels) */
    ioen_updateMessageFreshness ();

}



