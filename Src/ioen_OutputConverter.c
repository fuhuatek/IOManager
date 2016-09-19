/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_OutputConverter.c
 *
 * FILE DESCRIPTION:
 *   This module performs all the conversion of application output parameters to output signals.
 *
***************************************************************/

#include "ioen_IomLocal.h"



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWrite16bData
 *
 * DESCRIPTION:
 *   This function reads a 16bit parameter from the Parameter Buffer and writes a word into the Output Message Buffer
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWrite8bData (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    Byte_t data;

    data = *((Byte_t*)(outParamBuffer + parConfig_p->parOffset));

    *((Byte_t*)(outMsgBuffer + sigConfig_p->offByte)) = data;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWrite16bData
 *
 * DESCRIPTION:
 *   This function reads a 16bit parameter from the Parameter Buffer and writes a word into the Output Message Buffer
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWrite16bData (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt16_t data;

    data = HTON16(*((UInt16_t*)(outParamBuffer + parConfig_p->parOffset)));

    *((UInt16_t*)(outMsgBuffer + sigConfig_p->offByte)) = data;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWrite32bData
 *
 * DESCRIPTION:
 *   This function reads a 32bit parameter from the Parameter Buffer and writes a word into the Output Message Buffer
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWrite32bData (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t data;

    data = HTON32(*((UInt32_t*)(outParamBuffer + parConfig_p->parOffset)));

    *((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)) = data;
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWrite64bData
 *
 * DESCRIPTION:
 *   This function reads a 64bit parameter from the Parameter Buffer and writes a word into the Output Message Buffer
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWrite64bData (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt64_t data;

    data = HTON64(*((UInt64_t*)(outParamBuffer + parConfig_p->parOffset)));

    *((UInt64_t*)(outMsgBuffer + sigConfig_p->offByte)) = data;
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWriteMultipleBytes
 *
 * DESCRIPTION:
 *   This function reads multiple bytes from the Parameter Buffer and writes them into the Output Message Buffer
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWriteMultipleBytes (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    Byte_t          * sig_p;    /* Copy address in the Output Message Buffer    */
    const Byte_t    * param_p;  /* Copy address in the the Parameter Buffer     */
    Byte_t          * sigEnd_p; /* Address of the copy-end in the output signal */

    /* Initialize the copy */
    sig_p    = outMsgBuffer + sigConfig_p->offByte;
    sigEnd_p = sig_p + sigConfig_p->sizeBits / 8;
    param_p  = outParamBuffer + parConfig_p->parOffset;

    while(sig_p < sigEnd_p)
    {
        *sig_p = *param_p;
        param_p++;
        sig_p++;
    }
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWriteBoolean
 *
 * DESCRIPTION:
 *   This function extracts a bit from the Parameter Buffer (according to A664 boolean convention)
 *   with shift and mask and copies the result into the Output Message Buffer
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWriteBoolean (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t data;
    UInt32_t word;
    UInt32_t mask;
    UInt32_t *word_p;

    word_p = (UInt32_t*)(outMsgBuffer + sigConfig_p->offByte);

    /* Obtain the 32-bits word containing the boolean */
    if (ioen_outBoolIsInteger)
        data = *((UInt32_t*)(outParamBuffer + parConfig_p->parOffset));
    else
        data = *((Byte_t*)(outParamBuffer + parConfig_p->parOffset));

    /* Shift the bit of interest to the defined position */
    data = data << sigConfig_p->offBits;
    mask = 1 << sigConfig_p->offBits;
    data = data & mask;

    word = NTOH32(*word_p);
    word = word & ~mask;
    word = word | data;
    *word_p = HTON32(word);
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWrite32bBitfield
 *
 * DESCRIPTION:
 *   This function reads a 32bits word from the Parameter Buffer, extract the required bits
 *   with shift and mask copies the result into the Output Message Buffer
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWrite32bBitfield (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t data;
    UInt32_t mask;
    UInt32_t word;
    UInt32_t *word_p;

    word_p = (UInt32_t*)(outMsgBuffer + sigConfig_p->offByte);

    data = *((UInt32_t*)(outParamBuffer + parConfig_p->parOffset));
    if (sigConfig_p->sizeBits == 32 )
    {
        mask = 0xFFFFFFFF;
    }
    else
    {
        mask = (((UInt32_t)1 << sigConfig_p->sizeBits) - 1) << sigConfig_p->offBits;
    }
    data = data << sigConfig_p->offBits;
    data = data & mask;

    word = HTON32(*word_p);
    word = word & ~mask;    /* delete relevant bits word */
    word = word | data;
    *word_p = HTON32(word);
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWriteA429BNR_Float
 *
 * DESCRIPTION:
 *   This function converts a floating  point value into Signed BNR A429 signal
 *   with shift and mask copies the result into the Output Message
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWriteA429BNR_Float (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t a429word;
    UInt32_t ivalue;
    Float32_t value;
    UInt32_t mask;
    UInt32_t cleanMask;
    UInt32_t signBit;

    a429word = NTOH32(*((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)));
    value = *((Float32_t  *)(outParamBuffer + parConfig_p->parOffset));
    ivalue = (UInt32_t) (value / sigConfig_p->lsbValue);

    mask = (((UInt32_t)1 << sigConfig_p->sizeBits) - 1);
    ivalue = (ivalue & mask) << sigConfig_p->offBits;

    cleanMask = ~(mask << sigConfig_p->offBits);

    a429word = a429word & cleanMask;
    a429word = a429word | ivalue;

    signBit = (UInt32_t)1 << 28;
    if (value < 0)
    {
        a429word = a429word | signBit;
    } else
    {
        a429word = a429word & (~signBit);
    }

    *((UInt32_t*)(ioen_outMsgBuffer + sigConfig_p->offByte)) = HTON32(a429word);
}





/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWriteA429UBNR_Float
 *
 * DESCRIPTION:
 *   This function converts a floating  point value into Unsigned BNR A429 signal
 *   with shift and mask copies the result into the Output Message
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWriteA429UBNR_Float (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t  a429word;
    UInt32_t  ivalue;
    Float32_t value;
    UInt32_t  mask;
    UInt32_t  cleanMask;

    a429word = NTOH32(*((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)));
    value    = *((Float32_t  *)(outParamBuffer + parConfig_p->parOffset));
    ivalue   = (UInt32_t) (value / sigConfig_p->lsbValue);

    mask   = (((UInt32_t)1 << sigConfig_p->sizeBits) - 1);
    ivalue = (ivalue & mask) << sigConfig_p->offBits;

    cleanMask = ~(mask << sigConfig_p->offBits);

    a429word = a429word & cleanMask;
    a429word = a429word | ivalue;

    *((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)) = HTON32(a429word);
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWriteA429BNR_Int
 *
 * DESCRIPTION:
 *   This function converts an integer into Signed BNR A429 signal
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWriteA429BNR_Int (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t a429word;
    UInt32_t ivalue;
    SInt32_t value;
    UInt32_t mask;
    UInt32_t cleanMask;
    UInt32_t signBit;

    a429word = NTOH32(*((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)));
    value    = *((SInt32_t  *)(outParamBuffer + parConfig_p->parOffset));
    ivalue   = (UInt32_t) (value / sigConfig_p->lsbValue);

    mask   = (((UInt32_t)1 << sigConfig_p->sizeBits) - 1);
    ivalue = (ivalue & mask) << sigConfig_p->offBits;

    cleanMask = ~(mask << sigConfig_p->offBits);

    a429word = a429word & cleanMask;
    a429word = a429word | ivalue;

    signBit = (UInt32_t)1 << 28;
    if (value < 0)
    {
        a429word = a429word | signBit;
    } else
    {
        a429word = a429word & (~signBit);
    }

    *((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)) = HTON32(a429word);
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWriteA429UBNR_Int
 *
 * DESCRIPTION:
 *   This function converts an integer into Unsigned BNR A429 signal
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWriteA429UBNR_Int (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t a429word;
    UInt32_t ivalue;
    SInt32_t value;
    UInt32_t mask;
    UInt32_t cleanMask;

    a429word = NTOH32(*((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)));
    value = *((SInt32_t  *)(outParamBuffer + parConfig_p->parOffset));
    ivalue = (UInt32_t) (value / sigConfig_p->lsbValue);

    mask = (((UInt32_t)1 << sigConfig_p->sizeBits) - 1);
    ivalue = (ivalue & mask) << sigConfig_p->offBits;

    cleanMask = ~(mask << sigConfig_p->offBits);

    a429word = a429word & cleanMask;
    a429word = a429word | ivalue;

    *((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)) = HTON32(a429word);
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWriteA429BCD_Float
 *
 * DESCRIPTION:
 *   This function converts a float into BCD A429 signal
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWriteA429BCD_Float (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t a429word;
    Float32_t value;
    UInt32_t bcdValue;
    UInt32_t multiplier;
    UInt32_t a429Sign;
    UInt32_t iValue;
    SInt16_t bits;
    UInt32_t mask;
    UInt32_t cleanMask;

    a429word = NTOH32(*((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)));
    value    = *((Float32_t  *)(outParamBuffer + parConfig_p->parOffset));

    if (value < 0.0)
    {
        a429Sign = IOEN_A429_SSM_BCD_NO_MINUS;
        value    = -value;
    }
    else
    {
        a429Sign = IOEN_A429_SSM_BCD_NO_PLUS;
    }

    iValue     = (UInt32_t) (value / sigConfig_p->lsbValue);
    bcdValue   = 0;
    bits       = sigConfig_p->sizeBits;
    multiplier = 0;
    
    while (bits > 0)
    {
        bcdValue   = bcdValue + ((iValue % 10) << multiplier);
        multiplier = multiplier + 4;
        iValue     = iValue / 10;
        bits       = bits - 4;
    }

    mask = (((UInt32_t)1 << sigConfig_p->sizeBits) - 1);
    bcdValue = (bcdValue & mask) << sigConfig_p->offBits;

    cleanMask = ~(mask << sigConfig_p->offBits);

    a429word = a429word & cleanMask;
    a429word = a429word | bcdValue;
    a429word = a429word & IOEN_A429_SSM_CLEAR_MASK;
    a429word = a429word | a429Sign;

    *((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)) = HTON32(a429word);
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWriteA429BCD_Int
 *
 * DESCRIPTION:
 *   This function converts an Integer into BCD A429 signal
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWriteA429BCD_Int (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t a429word;
    UInt32_t bcdValue;
    UInt32_t multiplier;
    UInt32_t a429Sign;
    SInt32_t iValue;
    SInt16_t bits;
    UInt32_t mask;
    UInt32_t cleanMask;

    a429word = NTOH32(*((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)));
    iValue = *((SInt32_t  *)(outParamBuffer + parConfig_p->parOffset));

    if (iValue < 0)
    {
        a429Sign = IOEN_A429_SSM_BCD_NO_MINUS;
        iValue   = -iValue;
    }
    else
    {
        a429Sign = IOEN_A429_SSM_BCD_NO_PLUS;
    }

    iValue = iValue / sigConfig_p->lsbValue;

    bcdValue   = 0;
    bits       = sigConfig_p->sizeBits;
    multiplier = 0;
    
    while (bits > 0)
    {
        bcdValue   = bcdValue + ((iValue % 10) << multiplier);
        multiplier = multiplier + 4;
        iValue     = iValue / 10;
        bits       = bits - 4;
    }

    mask = (((UInt32_t)1 << sigConfig_p->sizeBits) - 1);
    bcdValue = (bcdValue & mask) << sigConfig_p->offBits;

    cleanMask = ~(mask << sigConfig_p->offBits);

    a429word = a429word & cleanMask;
    a429word = a429word | bcdValue;
    a429word = a429word & IOEN_A429_SSM_CLEAR_MASK;
    a429word = a429word | a429Sign;

    *((UInt32_t*)(outMsgBuffer + sigConfig_p->offByte)) = HTON32(a429word);
}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_ocWriteValidityBitfield
 *
 * DESCRIPTION:
 *   This function implements the Special function Output validity status (used for CAN only).
 *   It sets the given bit field the Output Message Buffer to 1 or 0,
 *   depending on the status of the logic parameter
 *
 * INTERFACE:
 *
 *   In:  parConfig_p    : Parameter Config
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  outParamBuffer : Buffer where the data to output is read from
 *   In:  outMsgBuffer   : Buffer to write output data to
 *
 ******************************************************************/
void ioen_ocWriteValidityBitfield (
    /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
    /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
    /* IN     */ const Byte_t                 * const outParamBuffer,
    /* IN     */       Byte_t                 * const outMsgBuffer
)
{
    UInt32_t data;
    UInt32_t status;
    UInt32_t mask;
    UInt32_t word;
    UInt32_t *word_p;

    word_p = (UInt32_t*)(outMsgBuffer + sigConfig_p->offByte);

    status = *((UInt32_t*)(outParamBuffer + parConfig_p->valOffset));
    
    if (status == IOEN_VALIDITY_NORMALOP)
    {
        /* Source is valid, set output parameter to 1 */
        data = 1;
    }
    else
    {
        /* Source is invalid, set output parameter to 0 */
        data = 0;
    }

    if (sigConfig_p->sizeBits == 32 )
    {
        mask = 0xFFFFFFFF;
    }
    else
    {
        mask = (((UInt32_t)1 << sigConfig_p->sizeBits) - 1) << sigConfig_p->offBits;
    }
    data = data << sigConfig_p->offBits;
    data = data & mask;

    word = HTON32(*word_p);
    word = word & ~mask;    /* delete relevant bits word */
    word = word | data;
    *word_p = HTON32(word);
}

