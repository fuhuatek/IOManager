/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_ValidityCheck.c
 *
 * FILE DESCRIPTION:
 *   This module contains the source selection condition checks.
 *
***************************************************************/


#include "ioen_IomLocal.h"


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ConfirmInitMsg
 *
 * DESCRIPTION:
 *   This function initialises an object control structure for confirmation of values.
 *
 * INTERFACE:
 *
 *   In:  freshTime   : Max duration, before a message is marked as fresh in ms
 *   In:  unfreshTime : Max duration, before a message is marked as unfresh in ms
 *   In:  appPeriodMs : Periodic time of process in millisecs
 *   In Out:  ctrl_p  : Confirmation control object
 *
 ******************************************************************/
void ioen_ConfirmInitMsg (
    /* IN     */ const UInt32_t                      freshTime,
    /* IN     */ const UInt32_t                      unfreshTime,
    /* IN     */ const Float32_t                     appPeriodMs,
    /* IN OUT */       IomObjectCtrlData_t   * const ctrl_p
)
{
    ctrl_p->newData            = FALSE;
    ctrl_p->cycle              = 1; /* Initial value of one will force an initial unfresh transition */
    ctrl_p->limitCycleValid    = (UInt32_t)( ((Float32_t)freshTime   / appPeriodMs) + 0.99); /* plus 0,99 for rounding up */
    ctrl_p->limitCycleInvalid  = (UInt32_t)( ((Float32_t)unfreshTime / appPeriodMs) + 0.99); /* plus 0,99 for rounding up */
    ctrl_p->limitCycleValid++;    /* plus 1 because the decrement is before checking */
    ctrl_p->limitCycleInvalid++;  /* plus 1 because the decrement is before checking */
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ConfirmInitParam
 *
 * DESCRIPTION:
 *   This function initialises object control structures for confirmation of parameters.
 *
 * INTERFACE:
 *
 *   In:  iom_p       : pointer to IOM Configuration
 *   In:  appPeriodMs : Periodic time of process in millisecs
 *   In:  validTime   : Max duration, before a message is marked as fresh in ms
 *   In:  invalidTime : Max duration, before a message is marked as unfresh in ms
 *   In:  nofsources  : Number of sources to initialise
 *   In Out:  ctrl_p  : Confirmation control object
 *
 ******************************************************************/
void ioen_ConfirmInitParam (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         validTime,
    /* IN     */ const UInt32_t                         invalidTime,
    /* IN     */ const UInt32_t                         nofsources,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    UInt32_t            idx;
    Float32_t           validTimeFloat;
    Float32_t           invalidTimeFloat;

    /* Clear dynamic data */
    memset ((void *) ctrl, 0x00, sizeof(IomObjectCtrlData_t) * IOEN_MAX_SOURCES_PER_INPUT);

    validTimeFloat   = (Float32_t)validTime;
    invalidTimeFloat = (Float32_t)invalidTime;

    for (idx=0; (idx < nofsources); idx++)
    {
        ctrl[idx].limitCycleValid            = (UInt32_t)( (validTimeFloat   / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleInvalid          = (UInt32_t)( (invalidTimeFloat / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleValid++;    /* plus 1 because the decrement is before checking */
        ctrl[idx].limitCycleInvalid++;  /* plus 1 because the decrement is before checking */
        ctrl[idx].newData                    = FALSE;
        ctrl[idx].cycle                      = 1; /* Initial value of one will force an initial unfresh transition */
        ctrl[idx].validity.confirmed.value   = IOEN_GLOB_DATA_INIT_VALUE;
        ctrl[idx].validity.confirmed.ifValue = IOEN_VALIDITY_NODATA;
        ctrl[idx].validity.last.value        = IOEN_GLOB_DATA_VALID;    /* If first data is valid, then it is confirmed straight away */
        ctrl[idx].validity.last.ifValue      = IOEN_VALIDITY_NORMALOP;
        ctrl[idx].validity.current.value     = IOEN_GLOB_DATA_LOST;
        ctrl[idx].validity.current.ifValue   = IOEN_VALIDITY_UNFRESH;
    }
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ConfirmInitParamConfirmed
 *
 * DESCRIPTION:
 *   This function initialises object control structures for confirmation of parameters to confirmed.
 *
 * INTERFACE:
 *
 *   In:  iom_p       : pointer to IOM Configuration
 *   In:  appPeriodMs : Periodic time of process in millisecs
 *   In:  validTime   : Max duration, before a message is marked as fresh in ms
 *   In:  invalidTime : Max duration, before a message is marked as unfresh in ms
 *   In:  nofsources  : Number of sources to initialise
 *   In Out:  ctrl_p  : Confirmation control object
 *
 ******************************************************************/
void ioen_ConfirmInitParamConfirmed (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         validTime,
    /* IN     */ const UInt32_t                         invalidTime,
    /* IN     */ const UInt32_t                         nofsources,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    UInt32_t            idx;
    Float32_t           validTimeFloat;
    Float32_t           invalidTimeFloat;

    /* Clear dynamic data */
    memset ((void *) ctrl, 0x00, sizeof(IomObjectCtrlData_t) * IOEN_MAX_SOURCES_PER_INPUT);

    validTimeFloat   = (Float32_t)validTime;
    invalidTimeFloat = (Float32_t)invalidTime;

    for (idx=0; (idx < nofsources); idx++)
    {
        ctrl[idx].limitCycleValid            = (UInt32_t)( (validTimeFloat   / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleInvalid          = (UInt32_t)( (invalidTimeFloat / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleValid++;    /* plus 1 because the decrement is before checking */
        ctrl[idx].limitCycleInvalid++;  /* plus 1 because the decrement is before checking */
        ctrl[idx].newData                    = FALSE;
        ctrl[idx].cycle                      = 1; /* Initial value of one will force an initial unfresh transition */
        ctrl[idx].validity.confirmed.value   = IOEN_GLOB_DATA_VALID;
        ctrl[idx].validity.confirmed.ifValue = IOEN_VALIDITY_NORMALOP;
        ctrl[idx].validity.last.value        = IOEN_GLOB_DATA_VALID;
        ctrl[idx].validity.last.ifValue      = IOEN_VALIDITY_NORMALOP;
        ctrl[idx].validity.current.value     = IOEN_GLOB_DATA_VALID;
        ctrl[idx].validity.current.ifValue   = IOEN_VALIDITY_NORMALOP;
    }
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ConfirmInitParamCan
 *
 * DESCRIPTION:
 *   This function initialises object control structures for confirmation of parameters.
 *
 * INTERFACE:
 *
 *   In:  iom_p       : pointer to IOM Configuration
 *   In:  appPeriodMs : Periodic time of process in millisecs
 *   In:  validTime   : Max duration, before a message is marked as fresh in ms
 *   In:  invalidTime : Max duration, before a message is marked as unfresh in ms
 *   In:  nofsources  : Number of sources to initialise
 *   In Out:  ctrl_p  : Confirmation control object
 *
 ******************************************************************/
void ioen_ConfirmInitParamCan (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         validTime,
    /* IN     */ const UInt32_t                         invalidTime,
    /* IN     */ const UInt32_t                         nofParams,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_CAN_PARAM_PER_MESSAGE]
)
{
    UInt32_t            idx;
    Float32_t           validTimeFloat;
    Float32_t           invalidTimeFloat;

    /* Clear dynamic data */
    memset ((void *) ctrl, 0x00, sizeof(IomObjectCtrlData_t) * IOEN_MAX_SOURCES_PER_INPUT);

    validTimeFloat   = (Float32_t)validTime;
    invalidTimeFloat = (Float32_t)invalidTime;

    for (idx=0; (idx < nofParams); idx++)
    {
        ctrl[idx].limitCycleValid            = (UInt32_t)( (validTimeFloat   / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleInvalid          = (UInt32_t)( (invalidTimeFloat / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleValid++;    /* plus 1 because the decrement is before checking */
        ctrl[idx].limitCycleInvalid++;  /* plus 1 because the decrement is before checking */
        ctrl[idx].newData                    = FALSE;
        ctrl[idx].cycle                      = 1; /* Initial value of one will force an initial unfresh transition */
        ctrl[idx].validity.confirmed.value   = IOEN_GLOB_DATA_INIT_VALUE;
        ctrl[idx].validity.confirmed.ifValue = IOEN_VALIDITY_NODATA;
        ctrl[idx].validity.last.value        = IOEN_GLOB_DATA_VALID;    /* If first data is valid, then it is confirmed straight away */
        ctrl[idx].validity.last.ifValue      = IOEN_VALIDITY_NORMALOP;
        ctrl[idx].validity.current.value     = IOEN_GLOB_DATA_LOST;
        ctrl[idx].validity.current.ifValue   = IOEN_VALIDITY_UNFRESH;
    }
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ConfirmInitParamSignal
 *
 * DESCRIPTION:
 *   This function initialises object control structures for confirmation of parameters.
 *
 * INTERFACE:
 *
 *   In:  iom_p       : pointer to IOM Configuration
 *   In:  appPeriodMs : Periodic time of process in millisecs
 *   In:  nofsources  : Number of sources to initialise
 *   In:  sigConfig_p : Config for message refresh rates
 *   In Out:  ctrl_p  : Confirmation control object
 *
 ******************************************************************/
void ioen_ConfirmInitParamSignal (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         nofsources,
    /* IN     */ const InputSignalConfig_t      * const sigConfig_p,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    UInt32_t            idx;
    Float32_t           validTimeFloat;
    Float32_t           invalidTimeFloat;
    AfdxMessageInfo_t  *msgAfdx_p;
    A429MessageInfo_t  *msgA429_p;

    /* Clear dynamic data */
    memset ((void *) ctrl, 0x00, sizeof(IomObjectCtrlData_t) * IOEN_MAX_SOURCES_PER_INPUT);

    /* Get First message in config */
    msgAfdx_p = (AfdxMessageInfo_t *)((void *)(iom_p) + iom_p->afdxInputMessageStart);
    msgA429_p = (A429MessageInfo_t *)((void *)(iom_p) + iom_p->a429InputMessageStart);

    for (idx=0; (idx < nofsources); idx++)
    {
        /* Get corresponding fresh and unfresh times */
        if (sigConfig_p[idx].transport == IOEN_TRANSPORT_A429)
        {
            validTimeFloat   = (Float32_t)msgA429_p[sigConfig_p[idx].msgIdx].validTime;
            invalidTimeFloat = (Float32_t)msgA429_p[sigConfig_p[idx].msgIdx].invalidTime;
        }
        else
        {
            validTimeFloat   = (Float32_t)msgAfdx_p[sigConfig_p[idx].msgIdx].validTime;
            invalidTimeFloat = (Float32_t)msgAfdx_p[sigConfig_p[idx].msgIdx].invalidTime;
        }

        ctrl[idx].limitCycleValid            = (UInt32_t)( (validTimeFloat   / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleInvalid          = (UInt32_t)( (invalidTimeFloat / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleValid++;    /* plus 1 because the decrement is before checking */
        ctrl[idx].limitCycleInvalid++;  /* plus 1 because the decrement is before checking */
        ctrl[idx].newData                    = FALSE;
        ctrl[idx].cycle                      = 1; /* Initial value of one will force an initial unfresh transition */
        ctrl[idx].validity.confirmed.value   = IOEN_GLOB_DATA_INIT_VALUE;
        ctrl[idx].validity.confirmed.ifValue = IOEN_VALIDITY_NODATA;
        ctrl[idx].validity.last.value        = IOEN_GLOB_DATA_VALID;    /* If first data is valid, then it is confirmed straight away */
        ctrl[idx].validity.last.ifValue      = IOEN_VALIDITY_NORMALOP;
        ctrl[idx].validity.current.value     = IOEN_GLOB_DATA_LOST;
        ctrl[idx].validity.current.ifValue   = IOEN_VALIDITY_UNFRESH;
    }
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ConfirmInitParamValidity
 *
 * DESCRIPTION:
 *   This function initialises object control structures for confirmation of parameters.
 *
 * INTERFACE:
 *
 *   In:  iom_p           : pointer to IOM Configuration
 *   In:  appPeriodMs     : Periodic time of process in millisecs
 *   In:  nofsources      : Number of sources to initialise
 *   In:  validityLogic_p : Config for message refresh rates
 *   In Out:  ctrl_p      : Confirmation control object
 *
 ******************************************************************/
void ioen_ConfirmInitParamValidity (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         nofsources,
    /* IN     */ const ValidityConfig_t         * const validityLogic_p,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    UInt32_t                        idx;
    Float32_t                       validTimeFloat;
    Float32_t                       invalidTimeFloat;
    AfdxMessageInfo_t              *msgAfdx_p;
    A429MessageInfo_t              *msgA429_p;
    const ValidityConfig_t         *currValidityLogic_p;

    /* Clear dynamic data */
    memset ((void *) ctrl, 0x00, sizeof(IomObjectCtrlData_t) * IOEN_MAX_SOURCES_PER_INPUT);

    /* Get First message in config */
    msgAfdx_p = (AfdxMessageInfo_t *)((void *)(iom_p) + iom_p->afdxInputMessageStart);
    msgA429_p = (A429MessageInfo_t *)((void *)(iom_p) + iom_p->a429InputMessageStart);
    
    currValidityLogic_p = validityLogic_p;

    for (idx=0; (idx < nofsources); idx++)
    {
        /* Get corresponding fresh and unfresh times */
        if (currValidityLogic_p->condition[0].transport == IOEN_TRANSPORT_A429)
        {
            validTimeFloat   = (Float32_t)msgA429_p[currValidityLogic_p->condition[0].msgIdx].validTime;
            invalidTimeFloat = (Float32_t)msgA429_p[currValidityLogic_p->condition[0].msgIdx].invalidTime;
        }
        else
        {
            validTimeFloat   = (Float32_t)msgAfdx_p[currValidityLogic_p->condition[0].msgIdx].validTime;
            invalidTimeFloat = (Float32_t)msgAfdx_p[currValidityLogic_p->condition[0].msgIdx].invalidTime;
        }

        ctrl[idx].limitCycleValid            = (UInt32_t)( (validTimeFloat   / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleInvalid          = (UInt32_t)( (invalidTimeFloat / appPeriodMs) + 0.99);  /* plus 0,99 for rounding up */
        ctrl[idx].limitCycleValid++;    /* plus 1 because the decrement is before checking */
        ctrl[idx].limitCycleInvalid++;  /* plus 1 because the decrement is before checking */
        ctrl[idx].newData                    = FALSE;
        ctrl[idx].cycle                      = 1; /* Initial value of one will force an initial unfresh transition */
        ctrl[idx].validity.confirmed.value   = IOEN_GLOB_DATA_INIT_VALUE;
        ctrl[idx].validity.confirmed.ifValue = IOEN_VALIDITY_NODATA;
        ctrl[idx].validity.last.value        = IOEN_GLOB_DATA_VALID;    /* If first data is valid, then it is confirmed straight away */
        ctrl[idx].validity.last.ifValue      = IOEN_VALIDITY_NORMALOP;
        ctrl[idx].validity.current.value     = IOEN_GLOB_DATA_LOST;
        ctrl[idx].validity.current.ifValue   = IOEN_VALIDITY_UNFRESH;

        /* Get next logic */
        currValidityLogic_p = ((const void *)currValidityLogic_p + sizeof(ValidityConfig_t));
    }
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ConfirmSetNewMsgData
 *
 * DESCRIPTION:
 *   This function signals that new data has been received for confirmation of values.
 *
 * INTERFACE:
 *
 *   In Out:  ctrl_p  : Confirmation control object
 *
 ******************************************************************/
void ioen_ConfirmSetNewMsgData (
    /* IN OUT */       IomObjectCtrlData_t   * const ctrl_p
)
{
    ctrl_p->newData  = TRUE;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ConfirmMsgFreshness
 *
 * DESCRIPTION:
 *   This function sets confirmed valid or invalid values.
 *
 * INTERFACE:
 *
 *   In Out:  ctrl_p  : Confirmation control object
 *   In Out:  value_p : Value to update
 *
 ******************************************************************/
void ioen_ConfirmMsgFreshness (
    /* IN OUT */       IomObjectCtrlData_t   * const ctrl_p,
    /* IN OUT */       UInt32_t              * const value_p
)
{
    /* End of Fresh cycle, check validity of message */
    if (ctrl_p->newData == TRUE)
    {
        *value_p = IOEN_MSG_FRESH;  /* Confirmed valid message  */

        /* Reset cycle */
        ctrl_p->cycle   = ctrl_p->limitCycleInvalid;
        ctrl_p->newData = FALSE;
    }
    else
    {
        if (ctrl_p->cycle > 0)
        {
            /* Decrement cycle counter */
            ctrl_p->cycle--;

            if (ctrl_p->cycle == 0)
            {
                *value_p = IOEN_MSG_UNFRESH;  /* Confirmed unfresh message  */
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
 *   ioen_ConfirmObjectValidity
 *
 * DESCRIPTION:
 *   This function sets confirmed valid or invalid validities.
 *
 * INTERFACE:
 *
 *   In    :  nofSources : number of validity entries
 *   In Out:  ctrl_p     : Confirmation control object
 *
 ******************************************************************/
void ioen_ConfirmObjectValidity (
    /* IN     */ const UInt32_t                         nofSources,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
)
{
    UInt32_t             idx;
    Valid_t             *validity_p;
    IomObjectCtrlData_t *objCtrl_p;

    for (idx=0; (idx < nofSources); idx++)
    {
        /* Shortcuts */
        validity_p = &ctrl[idx].validity;
        objCtrl_p  = &ctrl[idx];

        if (validity_p->current.value == validity_p->last.value)
        {
            /* No change in validity, Check if end of cycle */
            if (objCtrl_p->cycle > 0)
            {
                /* No change in validity, decrement counter */
                objCtrl_p->cycle--;

                if (objCtrl_p->cycle == 0)
                {
                    /* Confirmed */
                    validity_p->confirmed = validity_p->current;
                }
                else
                {
                    /* No action, as designed */
                    /* Not yet confirmed      */
                }
            }
            else
            {
                /* No action, as designed */
                /* Still confirmed        */
            }
        }
        else if (validity_p->current.value == IOEN_GLOB_DATA_VALID)
        {
            /* Changed to valid, restart counter  */
            /* Change of value, not yet confirmed */
            objCtrl_p->cycle = objCtrl_p->limitCycleValid;
            validity_p->last = validity_p->current;
        }
        else
        {
            /* Changed to invalid or unfresh, restart counter */
            /* Change of value, not yet confirmed             */
            objCtrl_p->cycle = objCtrl_p->limitCycleInvalid;
            validity_p->last = validity_p->current;
        }

    }
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ConfirmObjectValidityItem
 *
 * DESCRIPTION:
 *   This function sets confirmed valid or invalid validity.
 *
 * INTERFACE:
 *
 *   In Out:  ctrl_p     : Confirmation control object 
 *
 ******************************************************************/
void ioen_ConfirmObjectValidityItem (
    /* IN OUT */       IomObjectCtrlData_t      * const ctrl_p
)
{
    if (ctrl_p->validity.current.value == ctrl_p->validity.last.value)
    {
        /* No change in validity, Check if end of cycle */
        if (ctrl_p->cycle > 0)
        {
            /* No change in validity, decrement counter */
            ctrl_p->cycle--;

            if (ctrl_p->cycle == 0)
            {
                /* Confirmed */
                ctrl_p->validity.confirmed = ctrl_p->validity.current;
            }
            else
            {
                /* No action, as designed */
                /* Not yet confirmed      */
            }
        }
        else
        {
            /* No action, as designed */
            /* Still confirmed        */
        }
    }
    else if (ctrl_p->validity.current.value == IOEN_GLOB_DATA_VALID)
    {
        /* Changed to valid, restart counter  */
        /* Change of value, not yet confirmed */
        ctrl_p->cycle         = ctrl_p->limitCycleValid;
        ctrl_p->validity.last = ctrl_p->validity.current;
    }
    else
    {
        /* Changed to invalid or unfresh, restart counter */
        /* Change of value, not yet confirmed             */
        ctrl_p->cycle         = ctrl_p->limitCycleInvalid;
        ctrl_p->validity.last = ctrl_p->validity.current;
    }

}



