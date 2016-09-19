/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_Util.c
 *
 * FILE DESCRIPTION:
 *   This module common utility functions for the IO Manager.
 *
***************************************************************/

#include "ioen_IomLocal.h"



/******************************************************************
 * FUNCTION NAME:
 *   ioen_UtilCheckCrc
 *
 * DESCRIPTION:
 *   This function calculates an ARINC 32 bit CRC for the given input data.
 *   A lookup table is used to improve performance.
 *   The Polynomial used for (ARINC 653) is
 *      0x04C11DB7, => x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
 *
 * INTERFACE:
 *
 *   Global Data      :  crc32LookupTable
 *
 *   In:  data_p       : pointer to input array to calculate the CRC on
 *   In:  msginfo_p    : Pointer to information where the CRC is stored
 *
 ******************************************************************/
Bool_t ioen_UtilCheckCrc (
   /* IN     */ const Byte_t             * const data_p,
   /* IN     */ const AfdxMessageInfo_t  * const msginfo_p
)
{
    Bool_t   crcOk;
    UInt32_t nofBytes;
    UInt32_t crc32;
    UInt32_t crcExpected;
    UInt32_t i;
    UInt32_t k;

    if (msginfo_p->crcOffset != 0)
    {
        /* Check Functional Status first */
		if (data_p[msginfo_p->crcFsbOffset] == IOEN_A664_FS_NO)
        {
            /* Normal operation, calculate CRC */

            crc32    = IOEN_CRC32_INIT_VALUE;  /* Start value */
            nofBytes = msginfo_p->crcOffset;

            for (i=0; i<nofBytes; i++)
            {
                k     = ((crc32 BIT_RSHIFT 24) BIT_XOR (UInt32_t)(data_p[i])) BIT_AND 0xFF;
                crc32 = (crc32 BIT_LSHIFT 8) BIT_XOR crc32LookupTable[k];
            }

            /* Finish XOR for 32 bit CRC */
            crc32 = crc32 BIT_XOR  IOEN_CRC32_FINISH_VALUE;

            /* Check calculated value against expected value */
            crcExpected = *((UInt32_t*)(data_p + msginfo_p->crcOffset));
            crcExpected = NTOH32(crcExpected);

            if (crc32 == crcExpected)
            {
                /* CRC validated */
                crcOk = TRUE;
            }
            else
            {
                crcOk = FALSE;
            }

        }
        else
        {
            /* Invalid functional status */
            crcOk = FALSE;
        }
    }
    else
    {
        /* Not configured, set to OK */
        crcOk = TRUE;
    }

    return crcOk;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_UtilSetCrc
 *
 * DESCRIPTION:
 *   This function calculates an ARINC 32 bit CRC for the given data, and sets it in the output data
 *   A lookup table is used to improve performance.
 *   The Polynomial used for (ARINC 653) is
 *      0x04C11DB7, => x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
 *
 * INTERFACE:
 *
 *   Global Data      :  crc32LookupTable
 *
 *   In Out:  data_p       : pointer to input array to calculate the CRC on
 *   In    :  msginfo_p    : Pointer to information where the CRC is stored
 *
 ******************************************************************/
void ioen_UtilSetCrc (
   /* IN OUT */       Byte_t             * const data_p,
   /* IN     */ const AfdxMessageInfo_t  * const msginfo_p
)
{
    UInt32_t nofBytes;
    UInt32_t crc32;
    UInt32_t i;
    UInt32_t k;


    if (msginfo_p->crcOffset != 0)
    {
        /* Set the Functional status before calculating CRC */
        *((Byte_t*)(data_p + msginfo_p->crcFsbOffset)) = IOEN_A664_FS_NO;

        crc32    = IOEN_CRC32_INIT_VALUE;  /* Start value */
        nofBytes = msginfo_p->crcOffset;

        for (i=0; i<nofBytes; i++)
        {
            k     = ((crc32 BIT_RSHIFT 24) BIT_XOR (UInt32_t)(data_p[i])) BIT_AND 0xFF;
            crc32 = (crc32 BIT_LSHIFT 8) BIT_XOR crc32LookupTable[k];
        }

        /* Finish XOR for 32 bit CRC */
        crc32 = crc32 BIT_XOR  IOEN_CRC32_FINISH_VALUE;

        /* Set calculated value */
        *((UInt32_t*)(data_p + msginfo_p->crcOffset)) = NTOH32(crc32);
    }
    else
    {
        /* No action as designed */
        /* CRC Not configured    */
    }

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_UtilFc
 *
 * DESCRIPTION:
 *   This function validates the freshness counter for the given input data.
 *   The freshness counter is a 16 bit value that increments for each message and wraps around to zero.
 *
 * INTERFACE:
 *
 *   Global Data      :  crc32LookupTable
 *
 *   In    :  data_p       : pointer to input array to calculate the FC on
 *   In    :  msginfo_p    : Pointer to information where the FC is stored
 *   In Out:  fcPrev_p     : Pointer to previous Freshness counter (FC)
 *
 ******************************************************************/
Bool_t ioen_UtilCheckFc (
   /* IN     */ const Byte_t             * const data_p,
   /* IN     */ const AfdxMessageInfo_t  * const msginfo_p,
   /* IN OUT */       UInt16_t           * const fcPrev_p
)
{
    Bool_t   fcOk;
    UInt16_t fcReceived;
    UInt16_t fcMin;
    UInt16_t fcMax;

    if (msginfo_p->fcOffset != 0)
    {
        /* Check Functional Status first */
		if (data_p[msginfo_p->fcFsbOffset] == IOEN_A664_FS_NO || data_p[msginfo_p->fcFsbOffset] == IOEN_A664_FS_FT)
        {
            /* Normal operation, calculate FC */

            /* If FC is in the range [minFC modulo  65536, maxFC module 65536] where        */
            /*    - minFC = previous FC + 1                                                 */
            /*    - maxFC = previous FC + 1 + InvalidConfirmationTime / Tx RefreshPeriod    */
            /* the message shall be considered fresh                                        */

             /* Check calculated value against received value */
            fcReceived = *((UInt16_t*)(data_p + msginfo_p->fcOffset));
            fcReceived = NTOH16(fcReceived);

            fcMin = *fcPrev_p + 1;
            fcMax = fcMin + (UInt32_t)( ((Float32_t)msginfo_p->invalidTime / msginfo_p->refreshPeriod) + 0.99);

            if (    (fcReceived >= fcMin)
                AND (fcReceived <= fcMax)
               )
            {
                /* FC validated */
                fcOk = TRUE;

                /* Save new value */
                *fcPrev_p = fcMin;
            }
            else
            {
                if (*fcPrev_p == 0)
                {
                    /* Starting point */
                    fcOk = TRUE;
                }
                else
                {
                    /* Invalid freshness counter */
                    fcOk = FALSE;
                }

                /* Save recived value, to allow startup, or recovery with only one message loss */
                *fcPrev_p = fcReceived;
            }

        }
        else
        {
            /* Invalid functional status */
            fcOk = FALSE;
        }
    }
    else
    {
        /* Not configured, set to OK */
        fcOk = TRUE;
    }

    return fcOk;
}

/******************************************************************
 * FUNCTION NAME:
 *   ioen_UtilSetFc
 *
 * DESCRIPTION:
 *   This function sets the freshness counter for the given input data.
 *   The freshness counter is a 16 bit value that increments for each message and wraps around to zero.
 *
 * INTERFACE:
 *
 *   Global Data      :  crc32LookupTable
 *
 *   In Out:  data_p       : pointer to output array to store the FC
 *   In    :  msginfo_p    : Pointer to information where the FC is stored
 *   In Out:  fcPrev_p     : Pointer to previous Freshness counter (FC)
 *
 ******************************************************************/
void ioen_UtilSetFc (
   /* IN OUT */       Byte_t             * const data_p,
   /* IN     */ const AfdxMessageInfo_t  * const msginfo_p,
   /* IN OUT */       UInt16_t           * const fcPrev_p
)
{
    UInt16_t fcNew;

    if (msginfo_p->fcOffset != 0)
    {
        /* Get new value */
        fcNew = *fcPrev_p + 1;

        /* Save new value */
        *fcPrev_p = fcNew;

        /* Set the Functional status */
        *((Byte_t*)(data_p + msginfo_p->fcFsbOffset)) = IOEN_A664_FS_NO;

        /* Set new value */
        *((UInt16_t*)(data_p + msginfo_p->fcOffset)) = NTOH16(fcNew);
    }
    else
    {
        /* No action as designed */
        /* FC Not configured     */
    }
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_UtilSetFc
 *
 * DESCRIPTION:
 *   This function sets the freshness counter for the given input data.
 *   The freshness counter is a 16 bit value that increments for each message and wraps around to zero.
 *
 * INTERFACE:
 *
 *   Global Data      :  crc32LookupTable
 *
 *   In Out:  data_p       : pointer to output array to store the FC
 *   In    :  msginfo_p    : Pointer to information where the FC is stored
 *   In Out:  fcPrev_p     : Pointer to previous Freshness counter (FC)
 *
 ******************************************************************/
Bool_t ioen_UtilIsZero (
   /* IN     */ const Byte_t             * const data_p,
   /* IN     */       UInt32_t                   nofBytes
)
{
    Bool_t   isZero;
    UInt16_t idx;

    isZero = TRUE;
    for (idx=0; idx < nofBytes; idx++)
    {
        if (data_p[idx] != 0)
        {
            isZero = FALSE;
        }
        else
        {
            /* No action as designed */
        }
    }

    return isZero;
}

