/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_SourceSelection.c
 *
 * FILE DESCRIPTION:
 *   This module contains functions to select the input message source for a logic parameter.
 *
***************************************************************/

#include "ioen_IomLocal.h"
#include "ioen_Platform.h"





/******************************************************************
 * FUNCTION NAME:
 *   ioen_ssCheckSourceValidity
 *
 * DESCRIPTION:
 *   This function reads the validity configured for this dataset, and returns the worst validity
 *
 * INTERFACE:
 *   Global Data      :  ioen_validityChecker
 *
 *   In:  inMsgBuffer_p       : pointer to message buffer with validity and data
 *   In:  validityConfig_p    : validity configured for this dataset
 *
 *   Return : worst validity found
 *
 ******************************************************************/
Validity_t ioen_ssCheckSourceValidity (
    /* IN     */ const Byte_t                   * const inMsgBuffer_p,
    /* IN     */ const ValidityConfig_t         * const validityConfig_p
)
{
    UInt32_t       valIdx;
    Validity_t     validityFinal;
    Validity_t     validityCondition;

    /* Initialized Condition Config pointer */

    validityFinal.spare          = 0;
    validityFinal.selectedSource = 0;
    validityFinal.value          = IOEN_GLOB_DATA_INIT_VALUE;
    validityFinal.ifValue        = IOEN_VALIDITY_NODATA;

    for (valIdx=0; (valIdx < validityConfig_p->numConditions); valIdx++)
    {
        /* Call the corresponding condition function given in the configuration */
        validityCondition = ioen_validityChecker[validityConfig_p->conditionType[valIdx]](inMsgBuffer_p, &validityConfig_p->condition[valIdx]);

        /* Calculate worst case validity */
        switch (validityCondition.value)
        {
            case IOEN_GLOB_DATA_VALID:
                if (validityFinal.value == IOEN_GLOB_DATA_INIT_VALUE)
                {
                    /* Not yet initialised, intialise it now */
                    validityFinal = validityCondition;
                }
				else if(validityFinal.ifValue == IOEN_VALIDITY_NORMALOP && validityCondition.ifValue == IOEN_VALIDITY_TEST)
				{
				     validityFinal = validityCondition;
				}
                else
                {
                    /* No action, as designed */
                }
                break;

            case IOEN_GLOB_DATA_LOST:
                /* final validity already set */
                validityFinal = validityCondition;
                break;

            default:
                /* Save error condition */
                if (validityFinal.value != IOEN_GLOB_DATA_LOST)
                {
                    /* LOST has priority, overwrite with invalid only if lost was not detected */
                    validityFinal = validityCondition;
                }
                else
                {
                    /* No action as deigned */
                }
                break;

        }
    }

    /* return final result */
    return validityFinal;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ssGetSourceValidity
 *
 * DESCRIPTION:
 *   This function
 *   1. Calculates and stores the validities for this selection set.
 *   2. Calculates the health score for this selection set.
 *
 * INTERFACE:
 *   Global Data      :  ioen_selectionSetlist
 *
 *   In :  validityLogic_p : pointer to Validity configuration
 *   In :  numSources      : number of sources in selection set
 *   In Out:  ctrl_p       : Confirmation control object
 *
 ******************************************************************/
void ioen_ssGetSourceValidity (
    /* IN     */ const ValidityConfig_t         * const validityLogic_p,
    /* IN     */ const UInt32_t                         numSources,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    UInt32_t   curSource;

    /* Calculate validities for each source */
    for (curSource=0; (curSource < numSources); curSource++)
    {
        /* Save validity for later rounting of the data.  */
        ctrl[curSource].validity.current = ioen_ssCheckSourceValidity(ioen_inMsgBuffer, &validityLogic_p[curSource]);

        /* Check all validities for the health score source.  */
        if (ctrl[curSource].validity.current.value == IOEN_GLOB_DATA_VALID)
        {
            /* If the source is valid, update the health score for the current Set/Source */
            ioen_selectionSetlist[validityLogic_p->sourceSet].healthScore[curSource]++;
        }
        else
        {
            /* No action as designed                            */
            /* Do not increase health score for invalid objects */
        }
    }

}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ssCheckLicParameter
 *
 * DESCRIPTION:
 *   This function checks the if LIC_PARAMETER is valid, and returns the new source
 *
 * INTERFACE:
 *   Global Data      :  ioen_inParamBuffer
 *   Global Data      :  ioen_outBoolIsInteger
 *
 *   In     :  curSource           : the current selected source
 *   In     :  iomLicParamConfig_p : pointer to LIC_PARAMETER configuration
 *   In     :  selectedSource      : the current selected source
 *   In Out :  nextSource_p        : the new selected source
 *
 *   Return : Validity of LIC Parameter
 *
 ******************************************************************/
Validity_t ioen_ssCheckLicParameter (
    /* IN     */ const UInt32_t                     curSource,
    /* IN     */ const LicParamConfig_t     * const iomLicParamConfig_p,
    /* IN     */ const UInt32_t                     selectedSource,
    /* IN OUT */       UInt32_t             * const nextSource_p
)
{
    UInt32_t   data;
    Validity_t validity;

    /* Check LIC Parameter value */

    /* Get validity of source selection parameter, NB: already calculated this cycle */
    validity = *(Validity_t *)(ioen_inParamBuffer + iomLicParamConfig_p->valOffset);

    /* If the source is valid, update the selected Source */
    if (   (validity.ifValue == IOEN_VALIDITY_NORMALOP)
        OR (validity.ifValue == IOEN_VALIDITY_TEST)
       )
    {
        /* LIC_PARAMETER is valid, and a source has not yet been selected */
        validity.value = IOEN_GLOB_DATA_VALID;

        /* Check mode of expected value */
        if (iomLicParamConfig_p->valueMode == IOEN_SOURCE_PARAM_VALUE_EXACT)
        {
            /* Check expected value of LIC_PARAMETER */
            /* compare 32 bit value */


            /* Obtain the 32-bits word containing the value */
            if (     (iomLicParamConfig_p->parType == IOEN_INPUT_MAPPING_A664_BOOLEAN32)
                 AND (ioen_outBoolIsInteger == FALSE)
               )
            {
                /* Get 8 bit BOOLEAN */
                data = *((Byte_t*)(ioen_inParamBuffer + iomLicParamConfig_p->parOffset));
            }
            else
            {
                /* Get 32 bit value */
                data = *((UInt32_t*)(ioen_inParamBuffer + iomLicParamConfig_p->parOffset));
            }

            if (    (data == iomLicParamConfig_p->valueExp)
                AND ((*nextSource_p == IOEN_NO_VALID_SOURCE_FOUND) OR (curSource == selectedSource))
               )
            {
                /* value matches expected value */
                /* | 1 | VALID    |   1   | VALID     |   0   || CHA      | */
                /* | 2 | VALID    |   1   | VALID     |   1   || Previous | */
                /* | 3 | VALID    |   1   | INVALID   |   X   || CHA      | */
                /* | 5 | VALID    |   0   | VALID     |   1   || CHB      | */
                /* | 8 | INVALID  |   X   | VALID     |   1   || CHB      | */

                /* Set the first found source (highest priority), or keep the currently selected source */
                *nextSource_p = curSource;
            }
            else
            {
                /* No action as designed */
            }
        }
        else
        {
            /* ANY mode selected, do not check the value */

            if (   (*nextSource_p == IOEN_NO_VALID_SOURCE_FOUND)
                OR (curSource == selectedSource)
               )
            {
                /* Set the lowest found source (highest priority), or keep the currently selected source */
                *nextSource_p = curSource;
            }
            else
            {
                /* No action as designed */
            }

        }
    }
    else
    {
        validity.value = IOEN_GLOB_DATA_INVALID;
    }

    return validity;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ssPerformHealthScoreSelection
 *
 * DESCRIPTION:
 *   This function selects a source based on the health score value
 *
 * INTERFACE:
 *   Global Data      :  ioen_selectionSetlist
 *
 *   In     :  curSet              : the current selected source
 *   In     :  iomSetConfig_p      : the current / new selected source
 *
 ******************************************************************/
void ioen_ssPerformHealthScoreSelection (
    /* IN     */ const UInt32_t                        curSet,
    /* IN     */ const SelectionSetConfig_t    * const iomSetConfig_p
)
{
    UInt32_t               curSource;
    UInt32_t               nextSource;
    UInt32_t               highestHealthScore;
    SelectionSetInfo_t    *setData_p;

    /* Only process the SOURCE_HEALTH_SCORE here */

    highestHealthScore  = 0;
    nextSource          = 0;
    setData_p           = &ioen_selectionSetlist[curSet]; /* shortcut to set dynamic data */

    /* Confirm current lock intervals for all sources in this selection set */
    ioen_ConfirmObjectValidity (iomSetConfig_p->nofSources, setData_p->lock);

    /* Loop for all sources in a set, to find the source with the highest priority and highest score */
    for (curSource=0; curSource < iomSetConfig_p->nofSources; curSource++)
    {
        /* Find source with the highest score */
        if (setData_p->healthScore[curSource] > highestHealthScore)
        {
            /* save first (highest priority) highest score, with its lock expired */

            highestHealthScore = setData_p->healthScore[curSource];

            if (curSource > setData_p->selectedSource)
            {
                /* Switching to a lower priority source */
                nextSource = curSource;

                /* Start lock period on none selected source */
                setData_p->lock[setData_p->selectedSource].validity.last.value    = IOEN_GLOB_DATA_INVALID;
                setData_p->lock[setData_p->selectedSource].validity.current.value = IOEN_GLOB_DATA_VALID;
            }
            else if (curSource < setData_p->selectedSource)
            {
                /* Trying to switching to a higher priority source */

                if (setData_p->lock[curSource].validity.confirmed.value == IOEN_GLOB_DATA_VALID)
                {
                    /* Switching to a confirmed higher priority source */
                    nextSource = curSource;
                }
                else
                {
                    /* No action, as designed */
                    /* New source has not finished its lock period */
                }
            }
            else
            {
                /* Same source selected  */
                nextSource = curSource;
            }
        }
        else
        {
            /* Start lock period on none selected source */
            setData_p->lock[curSource].validity.last.value    = IOEN_GLOB_DATA_INVALID;
            setData_p->lock[curSource].validity.current.value = IOEN_GLOB_DATA_VALID;
        }

        /* Clear highest score for next cycle */
        setData_p->healthScore[curSource] = 0;
    }

    /* Select the new source */
    setData_p->selectedSource = nextSource;

    /* Turn off lock period on selected source */
    setData_p->lock[setData_p->selectedSource].validity.last.value      = IOEN_GLOB_DATA_VALID;
    setData_p->lock[setData_p->selectedSource].validity.current.value   = IOEN_GLOB_DATA_VALID;
    setData_p->lock[setData_p->selectedSource].validity.confirmed.value = IOEN_GLOB_DATA_VALID;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ssPerformObjectValidSelection
 *
 * DESCRIPTION:
 *   This function selects a source based on the Object valid value
 *
 * INTERFACE:
 *   Global Data      :  ioen_selectionSetlist
 *
 *   In     :  curSet              : the current selected source
 *   In     :  iomSetConfig_p      : the current / new selected source
 *
 ******************************************************************/
void ioen_ssPerformObjectValidSelection (
    /* IN     */ const UInt32_t                        curSet,
    /* IN     */ const SelectionSetConfig_t    * const iomSetConfig_p
)
{
    UInt32_t               curSource;
    SelectionSetInfo_t    *setData_p;

    /* Only process the SOURCE_OBJECT_VALID here */

    setData_p                  = &ioen_selectionSetlist[curSet]; /* shortcut to set dynamic data*/
    setData_p->selectedSource  = IOEN_NO_VALID_SOURCE_FOUND;

    /* Loop for all sources in a set, to select a source */
    for (curSource=0; curSource < iomSetConfig_p->nofSources; curSource++)
    {
        /* Find the first valid source, that has its lock interval expired */
        if (    (setData_p->ctrl[curSource].validity.confirmed.value == IOEN_GLOB_DATA_VALID)
            AND (setData_p->lock[curSource].validity.confirmed.value == IOEN_GLOB_DATA_VALID)
            AND (setData_p->selectedSource == IOEN_NO_VALID_SOURCE_FOUND)
           )
        {
            /* save first (highest priority) highest score */
            setData_p->selectedSource = curSource;
        }
        else
        {
            /* No action as designed */
        }
    }

    /* Check if no valid source was found */
    if (setData_p->selectedSource == IOEN_NO_VALID_SOURCE_FOUND)
    {
        /* No source found, use first (highest priority) */
        setData_p->selectedSource = 0;
    }
    else
    {
        /* No action as designed */
    }
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ssPerformLicSelection
 *
 * DESCRIPTION:
 *      For given selection set the following is performed:
 *        1. Performs LIC_PARAMETER selection
 *
 * INTERFACE:
 *   Global Data      :  ioen_selectionSetlist
 *
 *   In     :  iom_p               : pointer to the IOM configuration
 *   In     :  curSet              : the current set
 *   In     :  iomSetConfig_p      : the current / new selected source
 *
 ******************************************************************/
void ioen_ssPerformLicSelection (
    /* IN     */ const IOMConfigHeader_t       * const iom_p,
    /* IN     */ const UInt32_t                        curSet,
    /* IN     */ const SelectionSetConfig_t    * const iomSetConfig_p
)
{
    UInt32_t                        curSource;          /* Current source being processed                                       */
    UInt32_t                        nofInvalidSources;  /* Highest health score found for LIC PARAMETER                         */
    UInt32_t                        firstInvalidSource; /* Source to be used if one LIC PARAMETER is valid, but incorrect value */
    Validity_t                      validity;
    const LicParamConfig_t         *iomLicParamConfig_p;

    /* -------------------------------------------------------  */
    /*     LIC_PARAMETER selection to be performed              */
    /*                                                          */
    /* -------------------------------------------------------  */
    /* |   |    CHA Control   |     CHB Control   || Channel  | */
    /* |   | Validity | Value |  Validity | Value || Selected | */
    /* -------------------------------------------------------| */
    /* | 1 | VALID    |   1   | VALID     |   0   || CHA      | */
    /* | 2 | VALID    |   1   | VALID     |   1   || Previous | */
    /* | 3 | VALID    |   1   | INVALID   |   X   || CHA      | */
    /* | 4 | VALID    |   0   | VALID     |   0   || Previous | */
    /* | 5 | VALID    |   0   | VALID     |   1   || CHB      | */
    /* | 6 | VALID    |   0   | INVALID   |   X   || CHB      | */
    /* | 7 | INVALID  |   X   | VALID     |   0   || CHA      | */
    /* | 8 | INVALID  |   X   | VALID     |   1   || CHB      | */
    /* | 9 | INVALID  |   X   | INVALID   |   X   || Previous | */
    /* -------------------------------------------------------| */
    /*                                                          */
    /*     Note 1=TRUE, 0=FALSE, Previous=last selected channel */
    /*                                                          */
    /*     Initialisation for last selected channel = CHA       */
    /* -------------------------------------------------------  */

    /* Invalidate previous selection */
    ioen_selectionSetlist[curSet].nextSource = IOEN_NO_VALID_SOURCE_FOUND;

    iomLicParamConfig_p  = (const LicParamConfig_t *)  ((const void *)(iom_p) + iomSetConfig_p->sourceOffset);
    nofInvalidSources    = 0;
    firstInvalidSource   = IOEN_NO_VALID_SOURCE_FOUND;

    /* Loop for all sources in a set */
    for (curSource=0; curSource < iomSetConfig_p->nofSources; curSource++)
    {
        /* Check LIC Parameter validity and value, updates in/out parameter if LIC is valid                */
        /* NB: LIC Parameter validity and value already calculated because it is a single source parameter */
        validity = ioen_ssCheckLicParameter (curSource, 
                                             iomLicParamConfig_p, 
                                             ioen_selectionSetlist[curSet].selectedSource,
                                             &ioen_selectionSetlist[curSet].nextSource);

        if (validity.value != IOEN_GLOB_DATA_VALID)
        {
            /* Invalid source detected */
            nofInvalidSources++;

            if (firstInvalidSource == IOEN_NO_VALID_SOURCE_FOUND)
            {
                /* Save first (highest priority) invalid source,                     */
                /* for use when an other source is valid, but its value is incorrect */
                firstInvalidSource = curSource;
            }
            else
            {
                /* No action as designed */
                /* Both source invalid   */
            }
        }
        else
        {
            /* No action as designed */
        }

        /* Increment pointer to next source */
        iomLicParamConfig_p ++;

    }

    /* If No source found with a valid LIC parameter */
    if (ioen_selectionSetlist[curSet].nextSource == IOEN_NO_VALID_SOURCE_FOUND)
    {
        if (   (nofInvalidSources == 0)
            OR (nofInvalidSources == iomSetConfig_p->nofSources)
           )
        {
            /* No selection found, and both sources have same state */

            if (ioen_selectionSetlist[curSet].selectedSource >= iomSetConfig_p->nofSources)
            {
                /* Robustness for out of range */
                ioen_selectionSetlist[curSet].selectedSource = 0;
            }
            else
            {
                /* No action as designed     */
                /* keep last selected source */ 
                /* ioen_selectionSetlist[curSet].selectedSource already contains correct value */
                /* | 4 | VALID    |   0   | VALID     |   0   || Previous | */
                /* | 9 | INVALID  |   X   | INVALID   |   X   || Previous | */
            }
        }
        else
        {
            /* At least one source is valid with incorrect value, use first invalid source */
            /* | 6 | VALID    |   0   | INVALID   |   X   || CHB      | */
            /* | 7 | INVALID  |   X   | VALID     |   0   || CHA      | */
            ioen_selectionSetlist[curSet].selectedSource = firstInvalidSource;
        }
    }
    else
    {
        /* If last source is still valid, keep it, otherwise change to new selection */
        /* | 1 | VALID    |   1   | VALID     |   0   || CHA      | */
        /* | 2 | VALID    |   1   | VALID     |   1   || Previous | */
        /* | 3 | VALID    |   1   | INVALID   |   X   || CHA      | */
        /* | 5 | VALID    |   0   | VALID     |   1   || CHB      | */
        /* | 8 | INVALID  |   X   | VALID     |   1   || CHB      | */
        ioen_selectionSetlist[curSet].selectedSource = ioen_selectionSetlist[curSet].nextSource;
    }

}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ssPerformSourceSelection
 *
 * DESCRIPTION:
 *      For all selection sets the following is performed:
 *        1. Performs LIC_PARAMETER selection
 *        2. Performs SOURCE_HEALTH_SCORE selection
 *        3. Performs OBJECT_VALID selection
 *
 * INTERFACE:
 *   Global Data      :  ioen_selectionSetlist
 *
 *   In     :  iom_p               : pointer to the IOM configuration
 *
 ******************************************************************/
void ioen_ssPerformSourceSelection (
    /* IN     */ const IOMConfigHeader_t    * const iom_p
)
{
    UInt32_t                        curSet;             /* Current Set being processed                                          */
    const SelectionSetlistConfig_t *iomSetlistConfig_p;
    const SelectionSetConfig_t     *iomSetConfig_p;

    /* In order to have a constant execution time, all sources are evaluated (in order of priority) */

    /* Get pointers to start of source information in the configuration */
    iomSetlistConfig_p      = (const SelectionSetlistConfig_t *)      ((const void *)(iom_p) + iom_p->selectionSetStart);
    iomSetConfig_p          = (const SelectionSetConfig_t *)          ((const void *)(iom_p) + iomSetlistConfig_p->setListOffset);

    /* Loop for all sets */
    for (curSet=0;  curSet < iomSetlistConfig_p->nofSets; curSet++)
    {
        if (iomSetConfig_p->criteria == IOEN_SOURCE_LIC_PARAMETER)
        {
            /* LIC_PARAMETER selection performed   */
            ioen_ssPerformLicSelection (iom_p, curSet, iomSetConfig_p);
        }
        else if (iomSetConfig_p->criteria == IOEN_OBJECT_VALID)
        {
            /* SOURCE_OBJECT_VALID performed */
            ioen_ssPerformObjectValidSelection (curSet, iomSetConfig_p);
        }
        else if (iomSetConfig_p->criteria == IOEN_SOURCE_HEALTH_SCORE)
        {
            /* SOURCE_HEALTH_SCORE performed */
            ioen_ssPerformHealthScoreSelection (curSet, iomSetConfig_p);
        }
        else
        {
            /* No action as designed */
            /* IOEN_SOURCE_ONE, No source selection for parameters with only one source */
        }

        /* Increment pointer to next set */
        iomSetConfig_p = (const SelectionSetConfig_t *) ((const void *)(iomSetConfig_p) + iomSetConfig_p->setConfigSize);

    }

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ssCalcSourceSelectionObjects
 *
 * DESCRIPTION:
 *      For all selection sets the following is performed:
 *        1. Performs OBJECT_VALID validity calculation
 *
 * INTERFACE:
 *   Global Data      :  ioen_selectionSetlist
 *   Global Data      :  ioen_inMsgBuffer
 *
 *   In     :  iom_p               : pointer to the IOM configuration
 *
 ******************************************************************/
void ioen_ssCalcSourceSelectionObjects (
    /* IN     */ const IOMConfigHeader_t    * const iom_p
)
{
    UInt32_t                        curSet;
    UInt32_t                        curSource;
    SelectionSetInfo_t             *setData_p;
    const SelectionSetlistConfig_t *iomSetlistConfig_p;
    const SelectionSetConfig_t     *iomSetConfig_p;
    const ValidityConfig_t         *validityLogic_p;

    /* In order to have a constant execution time, all sources are evaluated (in order of priority) */

    /* Get pointers to start of source information in the configuration */
    iomSetlistConfig_p      = (const SelectionSetlistConfig_t *)      ((const void *)(iom_p) + iom_p->selectionSetStart);
    iomSetConfig_p          = (const SelectionSetConfig_t *)          ((const void *)(iom_p) + iomSetlistConfig_p->setListOffset);

    /* Loop for all sets */
    for (curSet=0;  curSet < iomSetlistConfig_p->nofSets; curSet++)
    {
        if (iomSetConfig_p->criteria == IOEN_OBJECT_VALID)
        {
            /* Calculate extra set validity of this selection sets criteria */

            setData_p = &ioen_selectionSetlist[curSet]; /* shortcut to set dynamic data */

            /* first logic starts after Selection set header */
            validityLogic_p = (const ValidityConfig_t *)  ((const void *)(iom_p) + iomSetConfig_p->sourceOffset);

            /* Loop for all sources in a set */
            for (curSource=0; curSource < iomSetConfig_p->nofSources; curSource++)
            {
                /* Check all validities for the Object */
                setData_p->ctrl[curSource].validity.current = ioen_ssCheckSourceValidity(ioen_inMsgBuffer, &validityLogic_p[curSource]);
                setData_p->lock[curSource].validity.current = setData_p->ctrl[curSource].validity.current;
            }

            /* Confirm current validities for all sources in this selection set */
            ioen_ConfirmObjectValidity (iomSetConfig_p->nofSources, setData_p->ctrl);

            /* Confirm current lock intervals for all sources in this selection set */
            ioen_ConfirmObjectValidity (iomSetConfig_p->nofSources, setData_p->lock);

        }
        else
        {
            /* No action as designed                                     */
            /* There are only extra validity parameters for OBJECT_VALID */
        }

        /* Increment pointer to next set */
        iomSetConfig_p = (const SelectionSetConfig_t *) ((const void *)(iomSetConfig_p) + iomSetConfig_p->setConfigSize);

    }

}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_ssInitSourceSelection
 *
 * DESCRIPTION:
 *      Initialises global data for all selection sets
 *
 * INTERFACE:
 *   Global Data      :  ioen_selectionSetlist
 *
 *   In:  iom_p       : pointer to IOM Configuration
 *   In:  appPeriodMs : Periodic time of process in millisecs
 *
 ******************************************************************/
void ioen_ssInitSourceSelection (
    /* IN     */ const IOMConfigHeader_t   * const iom_p,
    /* IN     */ const Float32_t                   appPeriodMs
)
{
    UInt32_t                        curSet;
    const SelectionSetlistConfig_t *iomSetlistConfig_p;
    const SelectionSetConfig_t     *iomSetConfig_p;
    const ValidityConfig_t         *validityLogic_p;

    /* Clear dynamic data and Set default source selections to first entry (0=highest priority) */
    memset ((void *) &ioen_selectionSetlist, 0x00, sizeof(ioen_selectionSetlist));

    /* Initialise confirmation times for all selection sets, that are OBJECT_VALID */
    
    /* Get pointers to start of source information in the configuration */
    iomSetlistConfig_p      = (const SelectionSetlistConfig_t *)      ((const void *)(iom_p) + iom_p->selectionSetStart);
    iomSetConfig_p          = (const SelectionSetConfig_t *)          ((const void *)(iom_p) + iomSetlistConfig_p->setListOffset);

    /* Loop for all sets */
    for (curSet=0;  curSet < iomSetlistConfig_p->nofSets; curSet++)
    {
        if (iomSetConfig_p->criteria == IOEN_OBJECT_VALID)
        {
            /* Initialise the validity confirmation structure                */
            /* NB: There are only extra validity parameters for OBJECT_VALID */

            /* first logic starts after Selection set header */
            validityLogic_p = (const ValidityConfig_t *)  ((const void *)(iom_p) + iomSetConfig_p->sourceOffset);

            ioen_ConfirmInitParamValidity (iom_p,
                                           appPeriodMs,
                                           iomSetConfig_p->nofSources,
                                           validityLogic_p,
                                           ioen_selectionSetlist[curSet].ctrl);

            /* Initialise the Lock interval confirmation structure */
            ioen_ConfirmInitParam (iom_p,
                                   appPeriodMs,
                                   iomSetConfig_p->sourceHealthValue,
                                   iomSetConfig_p->sourceHealthValue,
                                   iomSetConfig_p->nofSources,
                                   ioen_selectionSetlist[curSet].lock);
        }
        else if (iomSetConfig_p->criteria != IOEN_SOURCE_ONE)
        {
            /* Initialise the Lock interval confirmation structure to confirmed */
            ioen_ConfirmInitParamConfirmed
                                  (iom_p,
                                   appPeriodMs,
                                   iomSetConfig_p->sourceHealthValue,
                                   iomSetConfig_p->sourceHealthValue,
                                   iomSetConfig_p->nofSources,
                                   ioen_selectionSetlist[curSet].lock);
        }

        /* Increment pointer to next set */
        iomSetConfig_p = (const SelectionSetConfig_t *) ((const void *)(iomSetConfig_p) + iomSetConfig_p->setConfigSize);

    }


}

