/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_OutputMappingCan.c
 *
 * FILE DESCRIPTION:
 *   This module performs all the conversion of application output parameters to output signals for the CAN bus.
 *
***************************************************************/

#include "ioen_IomLocalCan.h"




/******************************************************************
 * FUNCTION NAME:
 *   ioen_omcProcessOutputParametersCan
 *
 * DESCRIPTION:
 *   This function processes all mappings (parameters) of one CAN output message
 *
 * INTERFACE:
 *
 *   In:  msgCfg_p       : pointer to current CAN Output message config data
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_omcProcessOutputParametersCan (
    /* IN     */ const CanMessageConfig_t     * const msgCfg_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t                 curMap;
    simpleMapConfigCanOut_t *singleCfg_p;

    /* Otherwise, prepare the first mapping config */
    singleCfg_p = (simpleMapConfigCanOut_t *)(msgCfg_p + 1);

    /* Process all parameters */
    for (curMap=0; curMap < msgCfg_p->numMappings; curMap++)
    {
        /* Apply the corresponding conversion from parameter to signal */
        ioen_outputConverters[singleCfg_p->sigconfig.type](&(singleCfg_p->parconfig), &(singleCfg_p->sigconfig), outParamBuffer, outMsgBuffer);

        if (singleCfg_p->parconfig.numSources == 2)
        {
            /* CAN can have a special function, which contains an extra output validity status parameter */
            ioen_outputConverters[singleCfg_p->sigconfigValidity.type](&(singleCfg_p->parconfig), &(singleCfg_p->sigconfigValidity), outParamBuffer, outMsgBuffer);
        }
        else
        {
            /* No action as designed */
        }

        singleCfg_p = (void *)singleCfg_p + sizeof(simpleMapConfigCanOut_t);
    }

}
