/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_InputMappingsAfdx.c
 *
 * FILE DESCRIPTION:
 *   This module processes a Dataset for the conversion of input signals to application input parameters.
 *
***************************************************************/


#include "ioen_IomLocal.h"




/******************************************************************
 * FUNCTION NAME:
 *   ioen_imaDoInputMappings
 *
 * DESCRIPTION:
 *   This function reads multiple bytes from Input Message Buffer and copies it into the Parameter Buffer
 *
 * INTERFACE:
 *   Global Data      :  ioen_inputConverters
 *   Global Data      :  ioen_inParamBuffer
 *
 *   In:  parConfig_p     : Parameter Config
 *   In:  numParams       : Number of parameters in the data set
 *   In:  selectedSource  : Selected source in the selection set
 *   In Out:  valid_p     : control object for validities
 *
 ******************************************************************/
void ioen_imaCopyToParamBuffer (
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const UInt16_t                     numParams,
    /* IN     */ const UInt32_t                     selectedSource,
    /* IN OUT */       Valid_t              * const valid_p
)
{
    Validity_t                  validityNew;
    UInt16_t                    curMap;      /* Index of current Mapping being processed. */
    const ParamMappingConfig_t *curParam_p;  /* pointer to current parameter mapping      */
    InputSignalConfig_t        *sigConfig_p; /* Address of the Input Signal Configuration */

    /* First param */
    curParam_p = parConfig_p;

    /* Process all parameters in this dataset */
    /* (NB: params that are associated with the same Functional status) */
    for (curMap = 0; (curMap < numParams); curMap++)
    {
        /* perform the mapping of one input parameter per iteration */

        /* Obtain the address of the first signal configuration */
        sigConfig_p = (InputSignalConfig_t *) ((Byte_t *)curParam_p + sizeof(ParamMappingConfig_t));

        /* Obtain the address of the selected signal configuration */
        sigConfig_p = &sigConfig_p[selectedSource];

        if (valid_p->confirmed.value == IOEN_GLOB_DATA_INIT_VALUE)
        {
            /* Initial value is not confirmed in buffer, use initial value */
            validityNew = valid_p->confirmed;

            /* Set output data to zero */
            memset ((UInt32_t *)(ioen_inParamBuffer + curParam_p->parOffset), 0, curParam_p->parSize/8);

            /* Set current source */
            validityNew.selectedSource = selectedSource + 1;

            /* Copy status to  application buffer */
            *(UInt32_t *)(ioen_inParamBuffer + curParam_p->valOffset) = * ((UInt32_t *) &validityNew);
        }
        else if (valid_p->confirmed.value == valid_p->current.value)
        {
            /* Copy from param source buffer to param buffer */
            /* Only copy a confirmed value, initial value, otherwise previous value is used */

            /* Read confirmed status from buffer */
            validityNew = * (Validity_t *)(ioen_inParamBufferSrc + sigConfig_p->valOffset);

            /* Copy confirmed data */
            memcpy ((UInt32_t *)(ioen_inParamBuffer + curParam_p->parOffset),
                    (UInt32_t *)(ioen_inParamBufferSrc + sigConfig_p->parOffset),
                    curParam_p->parSize/8);

            /* Set current source */
            validityNew.selectedSource = selectedSource + 1;

            /* Copy status to  application buffer */
            *(UInt32_t *)(ioen_inParamBuffer + curParam_p->valOffset) = * ((UInt32_t *) &validityNew);
        }
        else
        {
            /* No action, as designed */
        }

        /* Next param */
        curParam_p = (void *)curParam_p + IOEN_SIZEOF_PARAM_MAPPING_CONFIG(curParam_p);
    }
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_imaDoInputMappings
 *
 * DESCRIPTION:
 *   This function reads multiple bytes from Input Message Buffer and copies it into the Parameter Buffer
 *
 * INTERFACE:
 *   Global Data      :  ioen_inputConverters
 *   Global Data      :  ioen_inParamBuffer
 *
 *   In:  parConfig_p     : Parameter Config
 *   In:  numParams       : Number of parameters in the data set
 *   In Out:  inObjectCtrl: array of confirmation control objects
 *
 ******************************************************************/
void ioen_imaDoInputMappings (
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const UInt16_t                     numParams,
    /* IN OUT */       IomObjectCtrlData_t          objectCtrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    UInt16_t                    curMap;          /* Index of current Mapping being processed.      */
    UInt32_t                    srcIdx;          /* Index of current source processed.             */
    const ParamMappingConfig_t *curParam_p;      /* pointer to current parameter mapping           */
    InputSignalConfig_t        *sigConfig_p;     /* Address of the Input Signal Configuration      */
    InputSignalConfig_t        *sigConfigNext_p; /* Address of the next Input Signal Configuration */

    /* First param */
    curParam_p = parConfig_p;

    /* Process all parameters in this dataset */
    /* (NB: params that are associated with the same Functional status) */
    for (curMap = 0; (curMap < numParams); curMap++)
    {
        /* perform the mapping of one input parameter per iteration */

        /* Obtain the address of the first signal configuration */
        sigConfig_p = (InputSignalConfig_t *) ((Byte_t *)curParam_p + sizeof(ParamMappingConfig_t));

        for (srcIdx = 0; (srcIdx < curParam_p->numSources); srcIdx++)
        {
            /* Obtain the address of the signal configuration */
            sigConfigNext_p = &sigConfig_p[srcIdx];

            /* Perform the actual conversion from input message buffer to internal buffer */
            ioen_inputConverters[sigConfigNext_p->type](objectCtrl[srcIdx].validity.current, curParam_p, sigConfigNext_p, ioen_inMsgBuffer, ioen_inParamBufferSrc);
        }

        /* Next param */
        curParam_p = (void *)curParam_p + IOEN_SIZEOF_PARAM_MAPPING_CONFIG(curParam_p);
    }
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_imaProcessDataset
 *
 * DESCRIPTION:
 *   This function Processes one dataset: Evaluates the logic and then maps all parameters
 *
 * INTERFACE:
 *   Global Data      :  ioen_inParamBuffer
 *   Global Data      :  ioen_selectionSetlist
 *
 *   In    :  dataset_p   : pointer to dataset header
 *   In Out:  inObjectCtrl: array of confirmation control objects
 *
 ******************************************************************/
void ioen_imaProcessDataset (
    /* IN     */ const InputDatasetHeader_t     * const dataset_p,
    /* IN OUT */       IomObjectCtrlData_t              objectCtrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    const ValidityConfig_t         *validityLogic_p;
    const ParamMappingConfig_t     *parConfig_p;
    const InputSignalConfig_t      *sigConfig_p; /* Address of the Input Signal Configuration */

    /* first logic starts after dataset header */
    validityLogic_p = (const ValidityConfig_t *)(dataset_p + 1);

    /* first parameter starts after logicSize bytes */
    parConfig_p = (const ParamMappingConfig_t *)((void *)validityLogic_p + dataset_p->logicSize);

    if (dataset_p->numSources == 0)
    {
        /* Internal parameter, just set status to valid */
        /* Obtain the address of the first signal configuration */
        sigConfig_p = (InputSignalConfig_t *) ((Byte_t *)parConfig_p + sizeof(ParamMappingConfig_t));

        *(UInt32_t *)(ioen_inParamBufferSrc + sigConfig_p->valOffset) = IOEN_VALIDITY_NORMALOP;
    }
    else if (dataset_p->numSources == 1)
    {
        /* Get validity of the only source in the dataset (parameters associated with one FS) */
        ioen_ssGetSourceValidity   (validityLogic_p, dataset_p->numSources, objectCtrl);

        /* process the parameter to the internal buffer for the one source */
        ioen_imaDoInputMappings (parConfig_p, dataset_p->numParams, objectCtrl);

        /* Do confirmation on parameter sources */
        ioen_ConfirmObjectValidity (dataset_p->numSources, objectCtrl);

        /* No source selection, so copy from internal buffer to application */
        ioen_imaCopyToParamBuffer (parConfig_p, dataset_p->numParams, 0, &objectCtrl[0].validity);
    }
    else
    {
        /* No Action, as designed */
        /* Robustnest case for in valid number of sources */
    }

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_imaProcessDatasetSources
 *
 * DESCRIPTION:
 *   This function Processes one dataset: Evaluates the logic only 
 *   and stores the result in the internal parameter buffer
 *
 * INTERFACE:
 *
 *   In:      dataset_p   : pointer to dataset header
 *   In Out:  inObjectCtrl: array of confirmation control objects
 *
 ******************************************************************/
void ioen_imaProcessDatasetSources (
    /* IN     */ const InputDatasetHeader_t     * const dataset_p,
    /* IN OUT */       IomObjectCtrlData_t              objectCtrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    const ValidityConfig_t         *validityLogic_p;
    const ParamMappingConfig_t     *parConfig_p;

    /* first logic starts after dataset header */
    validityLogic_p = (const ValidityConfig_t *)(dataset_p + 1);

    /* first parameter starts after logicSize bytes */
    parConfig_p = (const ParamMappingConfig_t *)((void *)validityLogic_p + dataset_p->logicSize);

    /* Get validities of each source in the dataset (parameters associated with one FS) */
    ioen_ssGetSourceValidity   (validityLogic_p, dataset_p->numSources, objectCtrl);

    /* process the parameter mapping for the sources */
    ioen_imaDoInputMappings(parConfig_p, dataset_p->numParams, objectCtrl);

    /* Do confirmation on parameter sources */
    ioen_ConfirmObjectValidity (dataset_p->numSources, objectCtrl);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_imaProcessDatasetValue
 *
 * DESCRIPTION:
 *   This function Processes one dataset: stores the result, validity is already calculated
 *
 * INTERFACE:
 *   Global Data      :  ioen_inParamBuffer
 *   Global Data      :  ioen_selectionSetlist
 *
 *   In :  iom_p       : pointer to IOM Configuration
 *   In :  dataset_p   : pointer to dataset header
 *   In Out:  inObjectCtrl: array of confirmation control objects
 *
 ******************************************************************/
void ioen_imaProcessDatasetValue (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const InputDatasetHeader_t     * const dataset_p,
    /* IN OUT */       IomObjectCtrlData_t              objectCtrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    const ValidityConfig_t         *validityLogic_p;
    const ParamMappingConfig_t     *parConfig_p;
    UInt32_t                        selectedSource;

    /* first logic starts after dataset header */
    validityLogic_p = (const ValidityConfig_t *)(dataset_p + 1);

    /* first parameter starts after logicSize bytes */
    parConfig_p = (const ParamMappingConfig_t *)((void *)validityLogic_p + dataset_p->logicSize);

    /* Get currently selected source */
    selectedSource = ioen_selectionSetlist[validityLogic_p->sourceSet].selectedSource;

    /* source selection performed, so copy from param source buffer to param buffer */
    ioen_imaCopyToParamBuffer (parConfig_p, dataset_p->numParams, selectedSource, &objectCtrl[selectedSource].validity);
}


