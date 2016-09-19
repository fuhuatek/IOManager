/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_OutputMappingAfdx.c
 *
 * FILE DESCRIPTION:
 *   This module performs all the conversion of application output parameters to output signals.
 *
***************************************************************/

#include "ioen_IomLocal.h"







/******************************************************************
 * FUNCTION NAME:
 *   ioen_omaComputeFunctionalStatus
 *
 * DESCRIPTION:
 *   This function Computes the FS of a parameter aand updates the Dataset FS if needed
 *
 * INTERFACE:
 *
 *   In    :  parValidity   : validity of the parameter
 *   In Out:  dsFs          : dataset Functional Status, i.e. worst FS among all parameters
 *
 ******************************************************************/
static void ioen_omaComputeFunctionalStatus (
    /* IN     */ const UInt32_t             parValidity,
    /* IN OUT */       Byte_t       * const dsFs
)
{
    Byte_t parFs;   /* FS of the parameter */

    /* Determine if the validity of the parameter is worse than the dataset validity */
    switch (parValidity)
    {
        case IOEN_VALIDITY_NORMALOP:
            parFs = IOEN_A664_FS_NO;
            break;

        case IOEN_VALIDITY_TEST:
            parFs = IOEN_A664_FS_FT;
            break;

        case IOEN_VALIDITY_NCD:
        case IOEN_VALIDITY_OUTOFRANGE:
            parFs = IOEN_A664_FS_NCD;
            break;

        case IOEN_VALIDITY_NODATA:
        case IOEN_VALIDITY_UNFRESH:
        case IOEN_VALIDITY_EMPTY:
        default:
            parFs = IOEN_A664_FS_ND;
            break;
    }

    /* Update the Dataset FS if the parameter FS is more critical */
    switch (parFs)
    {
        /* parFS is ND => Worst case, update always */
        case IOEN_A664_FS_ND:
            *dsFs = IOEN_A664_FS_ND;
            break;

        /* parFS is NCD => Update if dsFS is not ND */
        case IOEN_A664_FS_NCD:
            if (*dsFs != IOEN_A664_FS_ND)
            {
                *dsFs = IOEN_A664_FS_NCD;
            }
            else
            {
                /* No action as designed */
            }
            break;

        /* parFS is FT => Update only if dsFS is NO */
        case IOEN_A664_FS_FT:
            if (*dsFs == IOEN_A664_FS_NO)
            {
                *dsFs = IOEN_A664_FS_FT;
            }
            else
            {
                /* No action as designed */
            }
            break;

        /* parFS is NO => Do nothing */
        default:
            break;
    }
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_omaComputeA429SSM
 *
 * DESCRIPTION:
 *   This function Computes the FS of a parameter aand updates the Dataset FS if needed
 *
 * INTERFACE:
 *
 *   In    :  a429Cfg_p     : pointer to A429 Configuration
 *   In    :  dsFs          : Functional Status in A664 format
 *      Out:  a429signal_p  : pointer to A429 word in Output Message Buffer
 *
 ******************************************************************/
static void ioen_omaComputeA429SSM (
    /* IN     */ const OutA429LabelConfig_t   * const a429Cfg_p,
    /* IN     */ const Byte_t                         dsFs,
    /*    OUT */       UInt32_t               * const a429signal_p
)
{
    UInt32_t a429word;

    a429word = NTOH32(*a429signal_p);
    a429word = a429word & 0x9FFFFFFF;
    switch (dsFs)
    {
        case IOEN_A664_FS_ND:
            switch (a429Cfg_p->SSMType)
            {
                case IOEN_OUTPUT_SSM_TYPE_BNR:  /* BNR - Failure Warning */
                    a429word = a429word | IOEN_A429_SSM_BNR_FW;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_BCD:  /* BCD - Failure Warning. Note: BCD SSM cannot encode FW. NCD is used instead. */
                    a429word = a429word | IOEN_A429_SSM_BCD_NCD;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_DIS: /* DSI - Failure Warning. */
                    a429word = a429word | IOEN_A429_SSM_DIS_FW;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_NONE: /* No SSM - Failure Warning. No effect */
                default:
                    break;
            }
            break;

        case IOEN_A664_FS_NCD:
            switch (a429Cfg_p->SSMType)
            {
                case IOEN_OUTPUT_SSM_TYPE_BNR: /* BNR - NCD */
                    a429word = a429word | IOEN_A429_SSM_BNR_NCD;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_BCD: /* BCD - NCD. */
                    a429word = a429word | IOEN_A429_SSM_BCD_NCD;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_DIS: /* DSI - NCD. */
                    a429word = a429word | IOEN_A429_SSM_DIS_NCD;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_NONE: /* No SSM - NCD. No effect */
                default:
                    break;
            }
            break;

        case IOEN_A664_FS_FT:
            switch (a429Cfg_p->SSMType)
            {
                case IOEN_OUTPUT_SSM_TYPE_BNR: /* BNR - FT */
                    a429word = a429word | IOEN_A429_SSM_BNR_FT;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_BCD: /* BCD - FT */
                    a429word = a429word | IOEN_A429_SSM_BCD_FT;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_DIS: /* DSI - FT */
                    a429word = a429word | IOEN_A429_SSM_DIS_FT;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_NONE: /* No SSM - FT. No effect */
                default:
                    break;
            }
            break;

        case IOEN_A664_FS_NO:
            switch (a429Cfg_p->SSMType)
            {
                case IOEN_OUTPUT_SSM_TYPE_BNR: /* BNR - NO */
                    a429word = a429word | IOEN_A429_SSM_BNR_NO;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_BCD: /* BCD - NO. No effect. The SSM contains the sign, which is already set during parameter encoding. */
                    break;

                case IOEN_OUTPUT_SSM_TYPE_DIS: /* DSI - NO */
                    a429word = a429word | IOEN_A429_SSM_DIS_FT;
                    break;

                case IOEN_OUTPUT_SSM_TYPE_NONE: /* No SSM - NO. No effect */
                default:
                    break;
            }
            break;
            
        default:
            /* No action as designed */
            break;
    }
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_omaDoOneOutputMapping
 *
 * DESCRIPTION:
 *   This function Computes the FS of a parameter aand updates the Dataset FS if needed
 *
 * INTERFACE:
 *
 *   In    :  mapCfg_p       : pointer to first mapping config data
 *   In Out:  dsFs           : dataset Functional Status, i.e. worst FS among all parameters
 *
 ******************************************************************/
static void ioen_omaDoOneOutputMapping (
    /* IN     */ const simpleMapConfig_t      * const mapCfg_p,
    /* IN OUT */       Byte_t                 * const dsFs
)
{
    UInt32_t parValidity; /* Validity of the parameter */

    /* Apply the corresponding conversion from parameter to signal */
    ioen_outputConverters[mapCfg_p->sigconfig.type](&(mapCfg_p->parconfig), &(mapCfg_p->sigconfig), ioen_outParamBuffer, ioen_outMsgBuffer);

    /* Obtain the validity of the parameter */
    parValidity = *(UInt32_t *)(ioen_outParamBuffer + mapCfg_p->parconfig.valOffset);

    /* Convert the FS and update the Dataset FS if needed */
    ioen_omaComputeFunctionalStatus(parValidity, dsFs);
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_omaDoOutputMappings
 *
 * DESCRIPTION:
 *   This function processes all mappings (parameters) of one data set
 *
 * INTERFACE:
 *
 *   In    :  mapCfg_p      : pointer to first mapping config data
 *   In    :  numParams     : number of parameters
 *   In Out:  dsFs          : dataset Functional Status, i.e. worst FS among all parameters
 *
 ******************************************************************/
static void ioen_omaDoOutputMappings (
    /* IN     */ const simpleMapConfig_t      * const mapCfg_p,
    /* IN     */ const int                            numParams,
    /*    OUT */       Byte_t                 * const dsFs
)
{
    UInt16_t                     curMap;   /* Index of current Mapping being processed.   */
    const simpleMapConfig_t    * curMap_p; /* pointer to configuration of current mapping */

    curMap_p = mapCfg_p;

    /* Initialize validity to Normal Operation (best case) */
    *dsFs = IOEN_A664_FS_NO;

     /* Process all parameters */
    for (curMap=0; curMap < numParams; curMap++)
    {
        ioen_omaDoOneOutputMapping(curMap_p, dsFs);

        curMap_p = (void *)curMap_p + sizeof(simpleMapConfig_t);
    }
}





/******************************************************************
 * FUNCTION NAME:
 *   ioen_omaProcessOutputDatasetAfdx
 *
 * DESCRIPTION:
 *   This function processes all mappings (parameters) of one data set
 *
 * INTERFACE:
 *
 *   In    :  dataset_p     : pointer to first mapping config data
 *
 ******************************************************************/
void ioen_omaProcessOutputDatasetAfdx (
    /* IN     */ const OutputDatasetConfig_t   * const dataset_p
)
{
    UInt32_t               *a429signal_p;
    simpleMapConfig_t      *mapCfg_p;
    Byte_t                  dsFS;

    a429signal_p  = (UInt32_t*)(ioen_outMsgBuffer + dataset_p->DSOffset);

    if (dataset_p->type == IOEN_OUTPUT_DS_TYPE_EMBEDDED_A429)
    {
        /* If the paramset is an embedded A429 Label, initialize the Label ID and SDI */
        *a429signal_p = HTON32(dataset_p->a429w.labelIdSdi);
    }
    else
    {
        /* No action as designed */
    }

    /* Otherwise, prepare the first mapping config */
    mapCfg_p = (simpleMapConfig_t *)(dataset_p + 1);

    /* Do all the mappings */
    ioen_omaDoOutputMappings(mapCfg_p, dataset_p->numMappings, &dsFS);


    ioen_outMsgBuffer[dataset_p->FSOffset] = dsFS;

    /* If the paramset is an embedded A429 Label, update the SSM */
    if (dataset_p->type == IOEN_OUTPUT_DS_TYPE_EMBEDDED_A429)
    {
        ioen_omaComputeA429SSM(&dataset_p->a429w, dsFS, a429signal_p);
    }
    else
    {
        /* No action as dsigned */
    }
}


