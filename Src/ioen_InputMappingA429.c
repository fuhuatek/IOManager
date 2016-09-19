/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_InputMappingA429.c
 *
 * FILE DESCRIPTION:
 *   This module performs all the conversion of input signals to application input parameters for A429 IO.
 *
***************************************************************/

#include "ioen_IomLocal.h"




/******************************************************************
 * FUNCTION NAME:
 *   ioen_im4GetA429Label
 *
 * DESCRIPTION:
 *   This function returns the Label code of an A429 data word
 *
 * INTERFACE:
 *
 *   In:  a429word       : A429 word to decode
 *
 *   Return:  Label code contained in the A429 word
 *
******************************************************************/
UInt32_t ioen_im4GetA429Label(
   /* IN     */ const UInt32_t   a429word
)
{
    UInt32_t label;

    label = (a429word >> IOEN_A429_LABEL_OFFSET) BIT_AND ((1 << IOEN_A429_LABEL_SIZE) - 1);

    return label;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_im4GetA429Sdi
 *
 * DESCRIPTION:
 *   This function returns the SDI (Source / Destination Identifier) of an A429 data word
 *
 * INTERFACE:
 *
 *   In:  a429word       : A429 word to decode
 *
 *   Return:  SDI value contained in the A429 word
 *
******************************************************************/
UInt32_t ioen_im4GetA429Sdi(
   /* IN     */ const UInt32_t   a429word
)
{
    UInt32_t sdi;

    sdi = (a429word >> IOEN_A429_SDI_OFFSET) BIT_AND ((1 << IOEN_A429_SDI_SIZE) - 1);

    return sdi;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_im4ProcessA429Message
 *
 * DESCRIPTION:
 *   This function processes all A429 data words contained in the message buffer
 *
 * INTERFACE:
 *   Global Data      : ioen_iomConfig_p
 *   Global Data      : ioen_inMsgBuffer
 *   Global Data      : ioen_a429MsgControl
 *
 *   In:  portId         : Port source of message Label data
 *   In:  a429Label      : Raw data of A429 message (NB: only one label)
 *
 ******************************************************************/
void ioen_im4ProcessA429Message(
    /* IN     */ const UInt32_t                    portIdx,
    /* IN     */ const UInt32_t                    a429Label
)
{
    UInt32_t               a429word;
    UInt32_t               labelId;
    UInt32_t               sdi;
    A429PortInfo_t        *portInfo_p;   /* pointer to A429 Port config      */
    A429MessageCtrlData_t *msgCtrl_p;    /* pointer to A429 runtime data     */
    A429MessageData_t     *dataStart_p;  /* pointer to A429 message buffer   */

    /* Get First message in config */
    portInfo_p  = (A429PortInfo_t *)    ((void *)(ioen_iomConfig_p) + ioen_iomConfig_p->a429InputPortStart);
    dataStart_p = (A429MessageData_t *) ((void *)(ioen_inMsgBuffer  + portInfo_p[portIdx].messageOffset));

    /* get the A429 word in correct byte order */
    a429word = NTOH32(a429Label);

    /* Process the word                      */
    /* Get Label code and SDI from A429 data */
    labelId     = ioen_im4GetA429Label (a429word);
    sdi         = ioen_im4GetA429Sdi   (a429word);

    /* Get control data for this label */
    msgCtrl_p = &ioen_a429MsgControl[portIdx].data[labelId][sdi];

    if (msgCtrl_p->msgInfo_p != 0)
    {
        /* Label config valid for this messsage */

        /* Indicate new data and save raw data in correct place in the input message buffer */
        ioen_ConfirmSetNewMsgData (&msgCtrl_p->ctrl);
        dataStart_p[labelId].data[sdi].raw = a429Label;
    }
    else
    {
        /* No action as designed, robustness against an invalid label */
    }
}
