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
#include "ioen_Platform.h"




/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_Freshness
 *
 * DESCRIPTION:
 *   This function checks the validity condition Freshness.
 *   This is the freshness of the input message.
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_Freshness (
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t   *freshness_p;
    Validity_t  validity;

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the address of the freshness status in the Input Message Buffer */
    /* Freshness is not in network byte order, since its written by us        */
    freshness_p = (UInt32_t*) (inMsgBuffer + conditionConfig_p->offset);

    /* Return the condition check status */
    if (*freshness_p == IOEN_MSG_FRESH)
    {
        validity.value   = IOEN_GLOB_DATA_VALID;
        validity.ifValue = IOEN_VALIDITY_NORMALOP;
    }
    else
    {
        validity.value   = IOEN_GLOB_DATA_LOST;
        validity.ifValue = IOEN_VALIDITY_UNFRESH;
    }

    return validity;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_A664FS
 *
 * DESCRIPTION:
 *   This function checks the validity condition A664 FS.
 *   This is the validity of the functional status of the data.
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_A664FS (
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    const Byte_t   *FS_p; /* Address of the Functional Status */
    Validity_t      validity;

    /* Obtain the address of the Functional Status in the Input Message Buffer */
    FS_p = (const Byte_t*) (inMsgBuffer + conditionConfig_p->offset);

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Check the value of the Functional Status */
    switch (*FS_p)
    {
        case IOEN_A664_FS_NO :
            validity.value   = IOEN_GLOB_DATA_VALID;
            validity.ifValue = IOEN_VALIDITY_NORMALOP;
            break;

        case IOEN_A664_FS_FT :
            validity.value   = IOEN_GLOB_DATA_VALID;
            validity.ifValue = IOEN_VALIDITY_TEST;
            break;

        case IOEN_A664_FS_NCD:
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_NCD;
            break;

        case IOEN_A664_FS_ND:
        default:
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_NODATA;
            break;
    }

    return validity;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_FreshnessFs
 *
 * DESCRIPTION:
 *   This function checks the validity condition Freshness.
 *   This function checks the validity condition A664 FS.
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_FreshnessFs (
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    Validity_t      validity;
    UInt32_t       *freshness_p;
    const Byte_t   *FS_p; /* Address of the Functional Status */

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the address of the freshness status in the Input Message Buffer */
    /* Freshness is not in network byte order, since its written by us        */
    freshness_p = (UInt32_t*) (inMsgBuffer + conditionConfig_p->offset);

    /* Return the condition check status */
    if (*freshness_p == IOEN_MSG_FRESH)
    {
        /* Message is fresh, check functional status */
        /* Obtain the address of the Functional Status in the Input Message Buffer */
        FS_p = (const Byte_t*) (inMsgBuffer + conditionConfig_p->offset2); /* offset2 used as FS Offset */

        validity.spare          = 0;
        validity.selectedSource = 0;

        /* Check the value of the Functional Status */
        switch (*FS_p)
        {
            case IOEN_A664_FS_NO :
                validity.value   = IOEN_GLOB_DATA_VALID;
                validity.ifValue = IOEN_VALIDITY_NORMALOP;
                break;

            case IOEN_A664_FS_FT :
                validity.value   = IOEN_GLOB_DATA_VALID;
                validity.ifValue = IOEN_VALIDITY_TEST;
                break;

            case IOEN_A664_FS_NCD:
                validity.value   = IOEN_GLOB_DATA_INVALID;
                validity.ifValue = IOEN_VALIDITY_NCD;
                break;

            case IOEN_A664_FS_ND:
            default:
                validity.value   = IOEN_GLOB_DATA_INVALID;
                validity.ifValue = IOEN_VALIDITY_NODATA;
                break;
        }
    }
    else
    {
        /* Message is unfresh */
        validity.value   = IOEN_GLOB_DATA_LOST;
        validity.ifValue = IOEN_VALIDITY_UNFRESH;
    }

    return validity;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcGet_SSM_BNR
 *
 * DESCRIPTION:
 *   This function checks the validity condition SSM BNR.
 *   This checks the value of an A429 SSM associated to a binary data (BNR).
 *
 * INTERFACE:
 *
 *   In:  ssmval            : SSM Value to check
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcGet_SSM_BNR
(
    /* IN     */ const UInt32_t                  ssmval
)
{
    Validity_t  validity;

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Compare the masked status with the expected value */
    switch (ssmval)
    {
        case 0:
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_NODATA;
            break;

        case 1:
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_NCD;
            break;

        case 2:
            validity.value   = IOEN_GLOB_DATA_VALID;
            validity.ifValue = IOEN_VALIDITY_TEST;
            break;

        case 3:
            validity.value   = IOEN_GLOB_DATA_VALID;
            validity.ifValue = IOEN_VALIDITY_NORMALOP;
            break;

        default:
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_NODATA;
            break;
    }

    return validity;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_SSM_BNR
 *
 * DESCRIPTION:
 *   This function checks the validity condition SSM BNR.
 *   This checks the value of an A429 SSM associated to a binary data (BNR).
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_SSM_BNR
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t   *status_p; /* Address of the status to be checked        */
    UInt32_t    ssmval;   /* Value of the status after applying the mask */
    Validity_t  validity;

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the address of the status in the Input Message Buffer */
    status_p = (UInt32_t*) (inMsgBuffer + conditionConfig_p->offset);

    /* Apply the mask to the status */
    ssmval = (NTOH32(*status_p) BIT_RSHIFT 29) BIT_AND 3;

    validity = ioen_vcGet_SSM_BNR (ssmval);

    return validity;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_SSM_BNR_Combine
 *
 * DESCRIPTION:
 *   This function checks the validity condition Freshness.
 *   This function checks the validity condition A664 FS.
 *   This function checks the validity condition SSM BNR.
 *   This checks the value of an A429 SSM associated to a binary data (BNR).
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_SSM_BNR_Combine
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t        ssmval;
    Validity_t      validity;
    const UInt32_t *freshness_p; /* Freshness address          */
    const Byte_t   *FS_p;        /* Functional Status address  */
    const UInt32_t *status_p;    /* SSM address                */

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the address of the freshness status in the Input Message Buffer */
    /* Freshness is not in network byte order, since its written by us        */
    freshness_p = (const UInt32_t*) (inMsgBuffer + conditionConfig_p->offset);  /* offset  used as Fresh Offset */
    FS_p        = (const Byte_t*)   (inMsgBuffer + conditionConfig_p->offset2); /* offset2 used as FS Offset    */
    status_p    = (const UInt32_t*) (inMsgBuffer + conditionConfig_p->offset3); /* offset3 used as SSM Offset   */
    ssmval      = (NTOH32(*status_p) BIT_RSHIFT 29) BIT_AND 3; /* Apply the mask to the SSM */

    /* Return the condition check status */
    if (*freshness_p == IOEN_MSG_FRESH)
    {
        /* Message is fresh, check functional status */

        switch (*FS_p)
        {
            case IOEN_A664_FS_NO :
                /* Check the value of the SSM */
                validity = ioen_vcGet_SSM_BNR (ssmval);
                break;

            case IOEN_A664_FS_FT :
                /* Check the value of the SSM */
                validity = ioen_vcGet_SSM_BNR (ssmval);

                if (validity.ifValue == IOEN_VALIDITY_NORMALOP)
                {
                    /* If no error, set to TEST */
                    validity.ifValue = IOEN_VALIDITY_TEST;
                }
                break;

            case IOEN_A664_FS_NCD:
                validity.value   = IOEN_GLOB_DATA_INVALID;
                validity.ifValue = IOEN_VALIDITY_NCD;
                break;

            case IOEN_A664_FS_ND:
            default:
                validity.value   = IOEN_GLOB_DATA_INVALID;
                validity.ifValue = IOEN_VALIDITY_NODATA;
                break;
        }
    }
    else
    {
        /* Message is unfresh */
        validity.value   = IOEN_GLOB_DATA_LOST;
        validity.ifValue = IOEN_VALIDITY_UNFRESH;
    }

    return validity;

}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcGet_SSM_BCD
 *
 * DESCRIPTION:
 *   This function checks the validity condition SSM BCD.
 *   This checks the value of an A429 SSM associated to a binary coded data (BCD).
 *
 * INTERFACE:
 *
 *   In:  ssmval            : SSM Value to check
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcGet_SSM_BCD
(
    /* IN     */ const UInt32_t                  ssmval
)
{
    Validity_t  validity;

    validity.spare          = 0;
    validity.selectedSource = 0;

    switch (ssmval)
    {
        case 0:
        case 3:
            validity.value   = IOEN_GLOB_DATA_VALID;
            validity.ifValue = IOEN_VALIDITY_NORMALOP;
            break;

        case 1:
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_NCD;
            break;

        case 2:
            validity.value   = IOEN_GLOB_DATA_VALID;
            validity.ifValue = IOEN_VALIDITY_TEST;
            break;

        default:
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_NODATA;
            break;
    }

    return validity;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_SSM_BCD
 *
 * DESCRIPTION:
 *   This function checks the validity condition SSM BCD.
 *   This checks the value of an A429 SSM associated to a binary coded data (BCD).
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_SSM_BCD
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t   *status_p; /* Address of the status to be checked         */
    UInt32_t    ssmval;   /* Value of the status after applying the mask */
    Validity_t  validity;

    /* Obtain the address of the status in the Input Message Buffer */
    status_p = (UInt32_t*) (inMsgBuffer + conditionConfig_p->offset);

    /* Apply the mask to the status */
    ssmval = (NTOH32(*status_p) BIT_RSHIFT 29) BIT_AND 3;
    
    validity = ioen_vcGet_SSM_BCD (ssmval);

    return validity;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_SSM_BCD_Combine
 *
 * DESCRIPTION:
 *   This function checks the validity condition SSM BCD.
 *   This checks the value of an A429 SSM associated to a binary coded data (BCD).
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_SSM_BCD_Combine
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t        ssmval;
    Validity_t      validity;
    const UInt32_t *freshness_p; /* Freshness address          */
    const Byte_t   *FS_p;        /* Functional Status address  */
    const UInt32_t *status_p;    /* SSM address                */

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the address of the freshness status in the Input Message Buffer */
    /* Freshness is not in network byte order, since its written by us        */
    freshness_p = (const UInt32_t*) (inMsgBuffer + conditionConfig_p->offset);  /* offset  used as Fresh Offset */
    FS_p        = (const Byte_t*)   (inMsgBuffer + conditionConfig_p->offset2); /* offset2 used as FS Offset    */
    status_p    = (const UInt32_t*) (inMsgBuffer + conditionConfig_p->offset3); /* offset3 used as SSM Offset   */
    ssmval      = (NTOH32(*status_p) BIT_RSHIFT 29) BIT_AND 3; /* Apply the mask to the SSM */

    /* Return the condition check status */
    if (*freshness_p == IOEN_MSG_FRESH)
    {
        /* Message is fresh, check functional status */

        switch (*FS_p)
        {
            case IOEN_A664_FS_NO :
                /* Check the value of the SSM */
                validity = ioen_vcGet_SSM_BCD (ssmval);
                break;

            case IOEN_A664_FS_FT :
                /* Check the value of the SSM */
                validity = ioen_vcGet_SSM_BCD (ssmval);

                if (validity.ifValue == IOEN_VALIDITY_NORMALOP)
                {
                    /* If no error, set to TEST */
                    validity.ifValue = IOEN_VALIDITY_TEST;
                }
                break;

            case IOEN_A664_FS_NCD:
                validity.value   = IOEN_GLOB_DATA_INVALID;
                validity.ifValue = IOEN_VALIDITY_NCD;
                break;

            case IOEN_A664_FS_ND:
            default:
                validity.value   = IOEN_GLOB_DATA_INVALID;
                validity.ifValue = IOEN_VALIDITY_NODATA;
                break;
        }
    }
    else
    {
        /* Message is unfresh */
        validity.value   = IOEN_GLOB_DATA_LOST;
        validity.ifValue = IOEN_VALIDITY_UNFRESH;
    }

    return validity;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcGet_SSM_DIS
 *
 * DESCRIPTION:
 *   This function checks the validity condition SSM DIS.
 *   This checks the value of an A429 SSM associated to a DISCRETE data (DIS).
 *
 * INTERFACE:
 *
 *   In:  ssmval            : SSM Value to check
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcGet_SSM_DIS
(
    /* IN     */ const UInt32_t                  ssmval
)
{
    Validity_t  validity;

    validity.spare          = 0;
    validity.selectedSource = 0;

    switch (ssmval)
    {
        case 0:
            validity.value   = IOEN_GLOB_DATA_VALID;
            validity.ifValue = IOEN_VALIDITY_NORMALOP;
            break;

        case 1:
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_NCD;
            break;

        case 2:
            validity.value   = IOEN_GLOB_DATA_VALID;
            validity.ifValue = IOEN_VALIDITY_TEST;
            break;

        case 3:
        default:
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_NODATA;
            break;
    }

    return validity;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_SSM_DIS
 *
 * DESCRIPTION:
 *   This function checks the validity condition SSM DIS.
 *   This checks the value of an A429 SSM associated to a DISCRETE data (DIS).
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_SSM_DIS
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t   *status_p; /* Address of the status to be checked       */
    UInt32_t    ssmval;   /* Value of the status after applying the mask */
    Validity_t  validity;

    /* Obtain the address of the status in the Input Message Buffer */
    status_p = (UInt32_t*) (inMsgBuffer + conditionConfig_p->offset);

    /* Apply the mask to the status */
    ssmval   = (NTOH32(*status_p) BIT_RSHIFT 29) BIT_AND 3;

    validity = ioen_vcGet_SSM_DIS (ssmval);

    return validity;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_SSM_DIS
 *
 * DESCRIPTION:
 *   This function checks the validity condition SSM DIS.
 *   This checks the value of an A429 SSM associated to a DISCRETE data (DIS).
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_SSM_DIS_Combine
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t        ssmval;
    Validity_t      validity;
    const UInt32_t *freshness_p; /* Freshness address          */
    const Byte_t   *FS_p;        /* Functional Status address  */
    const UInt32_t *status_p;    /* SSM address                */

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the address of the freshness status in the Input Message Buffer */
    /* Freshness is not in network byte order, since its written by us        */
    freshness_p = (const UInt32_t*) (inMsgBuffer + conditionConfig_p->offset);  /* offset  used as Fresh Offset */
    FS_p        = (const Byte_t*)   (inMsgBuffer + conditionConfig_p->offset2); /* offset2 used as FS Offset    */
    status_p    = (const UInt32_t*) (inMsgBuffer + conditionConfig_p->offset3); /* offset3 used as SSM Offset   */
    ssmval      = (NTOH32(*status_p) BIT_RSHIFT 29) BIT_AND 3; /* Apply the mask to the SSM */

    /* Return the condition check status */
    if (*freshness_p == IOEN_MSG_FRESH)
    {
        /* Message is fresh, check functional status */

        switch (*FS_p)
        {
            case IOEN_A664_FS_NO :
                /* Check the value of the SSM */
                validity = ioen_vcGet_SSM_DIS (ssmval);
                break;

            case IOEN_A664_FS_FT :
                /* Check the value of the SSM */
                validity = ioen_vcGet_SSM_DIS (ssmval);

                if (validity.ifValue == IOEN_VALIDITY_NORMALOP)
                {
                    /* If no error, set to TEST */
                    validity.ifValue = IOEN_VALIDITY_TEST;
                }
                break;

            case IOEN_A664_FS_NCD:
                validity.value   = IOEN_GLOB_DATA_INVALID;
                validity.ifValue = IOEN_VALIDITY_NCD;
                break;

            case IOEN_A664_FS_ND:
            default:
                validity.value   = IOEN_GLOB_DATA_INVALID;
                validity.ifValue = IOEN_VALIDITY_NODATA;
                break;
        }
    }
    else
    {
        /* Message is unfresh */
        validity.value   = IOEN_GLOB_DATA_LOST;
        validity.ifValue = IOEN_VALIDITY_UNFRESH;
    }

    return validity;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_ValidityValue
 *
 * DESCRIPTION:
 *   This function checks the validity condition validity value.
 *   This is the special function validity value, which checks the functional status of an extra parameter.
 *   This parameter has already been routed, so it is always ready to be used.
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_ValidityValue
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t    data;
    UInt32_t    mask;
    UInt32_t    bits;
    Validity_t  validity;

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the validity value from the Input Message Buffer (bit field) */
    if (conditionConfig_p->access == 4)
    {
        data = *((UInt32_t*)(inMsgBuffer + conditionConfig_p->offset));
        data = NTOH32(data) BIT_RSHIFT conditionConfig_p->offBits;
    }
    else if (conditionConfig_p->access == 2)
    {
        data = *((UInt16_t*)(inMsgBuffer + conditionConfig_p->offset));
        data = NTOH16(data) BIT_RSHIFT conditionConfig_p->offBits;
    }
    else
    {
        data = *((Byte_t*)(inMsgBuffer + conditionConfig_p->offset));
        data = data BIT_RSHIFT conditionConfig_p->offBits;
    }

    bits = conditionConfig_p->sizeBits;

    if (bits == 32)
    {
        mask = 0xFFFFFFFF;
    }
    else
    {
        mask = (((UInt32_t)1 BIT_LSHIFT bits) - 1);
    }

    data = data BIT_AND mask;

    /* Compare the validity value with the expected value (32 bit binary value) */
    if (data == conditionConfig_p->minValue.uint32)
    {
        /* If the masked status matches the expected value, then set to valid */
        validity.value   = IOEN_GLOB_DATA_VALID;
        validity.ifValue = IOEN_VALIDITY_NORMALOP;
    }
    else
    {
        /* Otherwise, invalid */
        validity.value   = IOEN_GLOB_DATA_INVALID;
        validity.ifValue = IOEN_VALIDITY_NODATA;
    }

    return validity;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_RangeInt
 *
 * DESCRIPTION:
 *   This function checks the validity condition range check Signed integer.
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_RangeInt
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t    data;
    SInt32_t    sdata;
    UInt32_t    maskHighestBit;
    UInt32_t    maskNegExtend;
    UInt32_t    mask;
    UInt32_t    bits;
    Validity_t  validity;

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the validity value from the Input Message Buffer (bit field in 32 bit word) */
    if (conditionConfig_p->access == 4)
    {
        data = *((UInt32_t*)(inMsgBuffer + conditionConfig_p->offset));
        data = NTOH32(data) BIT_RSHIFT conditionConfig_p->offBits;
    }
    else if (conditionConfig_p->access == 2)
    {
        data = *((UInt16_t*)(inMsgBuffer + conditionConfig_p->offset));
        data = NTOH16(data) BIT_RSHIFT conditionConfig_p->offBits;
    }
    else
    {
        data = *((Byte_t*)(inMsgBuffer + conditionConfig_p->offset));
        data = data BIT_RSHIFT conditionConfig_p->offBits;
    }

    bits = conditionConfig_p->sizeBits;

    if (bits == 32)
    {
        maskHighestBit = 0x80000000;
        mask           = 0xFFFFFFFF;
        maskNegExtend  = 0x00000000;
    }
    else
    {
                                                               /* eg. for 17 bits */
        maskHighestBit = ((UInt32_t)1 BIT_LSHIFT (bits - 1));  /* eg. 0x00010000  */
        mask           = (((UInt32_t)1 BIT_LSHIFT bits) - 1);  /* eg. 0x0001FFFF  */
        maskNegExtend  = BIT_NOT mask;                         /* eg. 0xFFFE0000  */
    }

    /* Clear unwanted bits */
    data = data BIT_AND mask;
    
    /* Check if negative */
    if ((data BIT_AND maskHighestBit) == maskHighestBit)
    {
        /* Bit extend the sign bit */
        data = data BIT_OR maskNegExtend;
    }
    else
    {
        /* No action as designed */
    }

    /* Convert to signed integer */
    sdata = (SInt32_t) data;
    sdata = (SInt32_t)(sdata * conditionConfig_p->lsbValue);
    /* Compare the value with the expected value */
    if (    (sdata >= conditionConfig_p->minValue.sint32)
        AND (sdata <= conditionConfig_p->maxValue.sint32)
       )
    {
        /* If the masked status matches the expected value, then set to valid */
        validity.value   = IOEN_GLOB_DATA_VALID;
        validity.ifValue = IOEN_VALIDITY_NORMALOP;
    }
    else
    {
        /* Otherwise, invalid */
        validity.value   = IOEN_GLOB_DATA_INVALID;
        validity.ifValue = IOEN_VALIDITY_OUTOFRANGE;
    }

    return validity;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_RangeUint
 *
 * DESCRIPTION:
 *   This function checks the validity condition range check Unsigned integer.
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_RangeUint
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t    data;
	Float32_t   temp;
    UInt32_t    mask;
    UInt32_t    bits;
    Validity_t  validity;
    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the validity value from the Input Message Buffer (bit field in 32 bit word) */
    if (conditionConfig_p->access == 4)
    {
        data = *((UInt32_t*)(inMsgBuffer + conditionConfig_p->offset));
        data = NTOH32(data) BIT_RSHIFT conditionConfig_p->offBits;
    }
    else if (conditionConfig_p->access == 2)
    {
        data = *((UInt16_t*)(inMsgBuffer + conditionConfig_p->offset));
        data = NTOH16(data) BIT_RSHIFT conditionConfig_p->offBits;
    }
    else
    {
        data = *((Byte_t*)(inMsgBuffer + conditionConfig_p->offset));
        data = data BIT_RSHIFT conditionConfig_p->offBits;
    }

    bits = conditionConfig_p->sizeBits;

    if (bits == 32)
    {
        mask = 0xFFFFFFFF;
    }
    else
    {
        mask = (((UInt32_t)1 BIT_LSHIFT bits) - 1);
    }

    data = data BIT_AND mask;
	temp = data * conditionConfig_p->lsbValue;
	/*check the range after *LSB, if it is > max of uint, it is outofrange*/
	if( temp > 4294967295.0)
	{
	    validity.value   = IOEN_GLOB_DATA_INVALID;
        validity.ifValue = IOEN_VALIDITY_OUTOFRANGE;
		return validity;
	}

    data = (UInt32_t)(data * conditionConfig_p->lsbValue);
    /* Compare the value with the expected value */
    if (    (data >= conditionConfig_p->minValue.uint32)
        AND (data <= conditionConfig_p->maxValue.uint32)
       )
    {
        /* If the masked status matches the expected value, then set to valid */
        validity.value   = IOEN_GLOB_DATA_VALID;
        validity.ifValue = IOEN_VALIDITY_NORMALOP;
    }
    else
    {
        /* Otherwise, invalid */
        validity.value   = IOEN_GLOB_DATA_INVALID;
        validity.ifValue = IOEN_VALIDITY_OUTOFRANGE;
    }

    return validity;
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_RangeFloat
 *
 * DESCRIPTION:
 *   This function checks the validity condition range check floating point.
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_RangeFloat
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t    data;
    Float32_t   dataFloat;
    Bool_t      isValidFloat;
    Validity_t  validity;

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the validity value from the Input Message Buffer (bit field in 32 bit word) */
    data = *((UInt32_t*)(inMsgBuffer + conditionConfig_p->offset));
    data = NTOH32(data);
    
    isValidFloat = ioen_icuIsFloat(data);
    if (isValidFloat == TRUE)
    {
        dataFloat = *((Float32_t*)&data);
        dataFloat = dataFloat * conditionConfig_p->lsbValue;
        /* Compare the validity value with the expected value (32 bit floating value) */
        if (    (dataFloat >= conditionConfig_p->minValue.float32)
            AND (dataFloat <= conditionConfig_p->maxValue.float32)
           )
        {
            /* If the masked status matches the expected value, then set to valid */
            validity.value   = IOEN_GLOB_DATA_VALID;
            validity.ifValue = IOEN_VALIDITY_NORMALOP;
        }
        else
        {
            /* Otherwise, invalid */
            validity.value   = IOEN_GLOB_DATA_INVALID;
            validity.ifValue = IOEN_VALIDITY_OUTOFRANGE;
        }

    }
    else
    {
        /* Otherwise, invalid */
        validity.value   = IOEN_GLOB_DATA_INVALID;
        validity.ifValue = IOEN_VALIDITY_OUTOFRANGE;
    }

    return validity;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_vcCheckCondition_RangeFloatBnr
 *
 * DESCRIPTION:
 *   This function checks the validity condition range check floating point encoded as a BNR.
 * 
 *    Mask out significant bits and multiply with lsbValue
 *    if bit 28==1, result is negative (counting from 0 to 31)
 *
 * ----------------------------------------------------------------------------------------------
 * |31 | 30 | 29 | 28 | 27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10 | 9 | 8 | 7 ... 0 |
 * |--------------------------------------------------------------------------------------------|
 * | P |   SSM   | S  |                  Data                                 |  SDI  |  Label  |
 * ----------------------------------------------------------------------------------------------
 *
 * P     = Parity=Odd parity
 * SSM   = Sign Status Matrix, 00= Failure Waring, 01=No computed Data, 10=Functional Test, 11=Normal Operation
 * S     = sign, 0= Positive, 1=negative
 * SDI   = Source / Destination Identifier
 * Label = 8 bit label ID, usually specified in Octal
 *
 * INTERFACE:
 *
 *   In:  inMsgBuffer       : Input message buffer
 *   In:  conditionConfig_p : pointer to validity configuration
 *
 *   Return:  validity found
 *
 ******************************************************************/
Validity_t ioen_vcCheckCondition_RangeFloatBnr
(
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
)
{
    UInt32_t    a429word;
    UInt32_t    ivalue;
    SInt32_t    svalue;
    Float32_t   value;
    Validity_t  validity;

    validity.spare          = 0;
    validity.selectedSource = 0;

    /* Obtain the validity value from the Input Message Buffer (bit field in 32 bit word) */

    a429word  = NTOH32(*((UInt32_t*)(inMsgBuffer + conditionConfig_p->offset)));
    ivalue    = ((a429word & 0x1FFFFFFF) >> conditionConfig_p->offBits) & ((1 << (conditionConfig_p->sizeBits)) - 1);

    if (((a429word BIT_RSHIFT 28) BIT_AND 1) == 1)
    {
        /* Bit extend the sign bit, NB: sizeBits can include the sign bit 28 */
        ivalue = ivalue BIT_OR (BIT_NOT((1 BIT_LSHIFT conditionConfig_p->sizeBits) - 1));
    }
    else
    {
        /* No action as desgined */
    }

    /* Convert to signed integer */
    svalue = (SInt32_t) ivalue;

    /* Scale value */
    value = (Float32_t)svalue * conditionConfig_p->lsbValue;

    /* Compare the validity value with the expected value (32 bit floating value) */
    if (    (value >= conditionConfig_p->minValue.float32)
        AND (value <= conditionConfig_p->maxValue.float32)
       )
    {
        /* If the masked status matches the expected value, then set to valid */
        validity.value   = IOEN_GLOB_DATA_VALID;
        validity.ifValue = IOEN_VALIDITY_NORMALOP;
    }
    else
    {
        /* Otherwise, invalid */
        validity.value   = IOEN_GLOB_DATA_INVALID;
        validity.ifValue = IOEN_VALIDITY_OUTOFRANGE;
    }


    return validity;
}


