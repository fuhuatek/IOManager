/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_InputConverter.c
 *
 * FILE DESCRIPTION:
 *   This module performs the conversion of input signals to application input parameters.
 *
 *   The code is platform independent except for the the NTOH macros which convert a 16/32/64 bit
 *   value from Network Byte Order (bigendian) to the native byte order.
 *   It is assumed the the IDU and IMA platforms are using bigendian, ADS2 is using littleendian.
 *   For bigendian architecture the macros are not doing anything
 *   For bigendian architecture the IOM config is bigendian
 *   For littleendian architecture the IOM config is littleendian
 *
***************************************************************/

#include "ioen_IomLocal.h"





/******************************************************************
 * FUNCTION NAME:
 *   ioen_icMultipleBytes
 *
 * DESCRIPTION:
 *   This function reads multiple bytes from Input Message Buffer and copies it into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icMultipleBytes (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    Byte_t          * param_p;  /* Copy address in the the Parameter Buffer    */
    const Byte_t    * sig_p;    /* Copy address in the Input Message Buffer    */
    const Byte_t    * sigEnd_p; /* Address of the copy-end in the input signal */

    /*Initialize the copy */
    sig_p    = inMsgBuffer + sigConfig_p->offByte;
    sigEnd_p = sig_p + sigConfig_p->sizeBits / 8;
    param_p  = inParamBufferSrc + sigConfig_p->parOffset;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        while(sig_p < sigEnd_p)
        {
            *param_p = *sig_p;
            param_p++;
            sig_p++;
        }
    }
    else
    {
        /* Set to zero */
        while(sig_p < sigEnd_p)
        {
            *param_p = 0;
            param_p++;
            sig_p++;
        }
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icA664String
 *
 * DESCRIPTION:
 *   This function reads an A664 sting from Input Message Buffer and copies it into the Parameter Buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icA664String (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    Byte_t          * param_p;  /* Copy address in the the Parameter Buffer    */
    const Byte_t    * sig_p;    /* Copy address in the Input Message Buffer    */
    const Byte_t    * sigEnd_p; /* Address of the copy-end in the input signal */
    UInt16_t          length;   /* Length of string in AFDX Message            */

    /* Initialize the copy */

    /* Extract the length of the string form the fisrt 16 bit word */
    length   = NTOH16(*(UInt16_t *)(inMsgBuffer + sigConfig_p->offByte));

    /* Set start of data, first byte after length */
    sig_p    = inMsgBuffer + sigConfig_p->offByte + 2;

    /* Set output */
    param_p  = inParamBufferSrc + sigConfig_p->parOffset;
    sigEnd_p = sig_p + length;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        while(sig_p < sigEnd_p)
        {
            *param_p = *sig_p;
            param_p++;
            sig_p++;
        }
        *param_p = 0;
    }
    else
    {
        /* Set to zero */
        while(sig_p < sigEnd_p)
        {
            *param_p = 0;
            param_p++;
            sig_p++;
        }
        *param_p = 0;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic32bUInt
 *
 * DESCRIPTION:
 *   This function reads a 32bits Unsigned Integer from the Input Message Buffer and copies it into the Parameter Buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic32bUInt (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t   data;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        data = *((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte));
        data = NTOH32(data);

        *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = data;
    }
    else
    {
        /* copy default value */
        *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic32bInt
 *
 * DESCRIPTION:
 *   This function reads a 32bits Signed Integer from the Input Message Buffer and copies it into the Parameter Buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic32bInt (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    SInt32_t  data;
    Float32_t value;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        data  = *((SInt32_t*)(inMsgBuffer + sigConfig_p->offByte));   /* Read raw data */
        value =  NTOH32(data) * sigConfig_p->lsbValue;                 /* Scaling */

        *((SInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = (SInt32_t)value;
    }
    else
    {
        /* copy default value */
        *((SInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic32Float
 *
 * DESCRIPTION:
 * This function reads a 32bits word from the Input Message Buffer and copies it into the Parameter Buffer
 * The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic32Float (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t   data;
    Bool_t     isValidFloat;
    Validity_t newValidity;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        data = *((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte));
        data = NTOH32(data);

        isValidFloat = ioen_icuIsFloat(data);
        if (isValidFloat == TRUE)
        {
            /* Value OK */
            *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = data;

            newValidity = validity;
        }
        else
        {
            /* copy default value */
            *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;

            newValidity.value   = IOEN_GLOB_DATA_VALID;
            newValidity.ifValue = IOEN_VALIDITY_OUTOFRANGE;

            /* Range error, report BITE error */
            /* TBD */
        }
    }
    else
    {
        /* copy default value */
        *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;

        newValidity = validity;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &newValidity);
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic64Float
 *
 * DESCRIPTION:
 * This function reads a 64bits word from the Input Message Buffer and copies it into the Parameter Buffer
 * The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic64Float (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt64_t   data;
    Bool_t     isValidDouble;
    Validity_t newValidity;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        data = *((UInt64_t*)(inMsgBuffer + sigConfig_p->offByte));
        data = NTOH64(data);

        isValidDouble = ioen_icuIsDouble(data);
        if (isValidDouble == TRUE)
        {
            *((UInt64_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = data;

            newValidity = validity;
        }
        else
        {
            /* copy default value */
            *((UInt64_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;

            newValidity.value   = IOEN_GLOB_DATA_VALID;
            newValidity.ifValue = IOEN_VALIDITY_OUTOFRANGE;

            /* Range error, report BITE error */
            /* TBD */
        }
    }
    else
    {
        /* copy default value */
        *((UInt64_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;

        newValidity = validity;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &newValidity);

}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic8bBitfield
 *
 * DESCRIPTION:
 *   This function reads a 32bits word from the Input Message Buffer, extract the required bits
 *   with shift and mask copies the result into the Parameter Buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic8bBitfield (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t   data;
    UInt32_t   mask;
    UInt32_t   bits;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        bits = sigConfig_p->sizeBits;

        data = *((Byte_t*)(inMsgBuffer + sigConfig_p->offByte));
        data = NTOH32(data) >> sigConfig_p->offBits;
        if ( bits == 32 )
        {
            mask = 0xFFFFFFFF;
        }
        else
        {
            mask = (((UInt32_t)1 << bits) - 1);
        }

        data = data & mask;

        *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = data;
    }
    else
    {
        /* copy default value */
        *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}





/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic32bBitfield
 *
 * DESCRIPTION:
 *   This function reads a 32bits word from the Input Message Buffer, extract the required bits
 *   with shift and mask copies the result into the Parameter Buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic32bBitfield (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t   data;
    UInt32_t   mask;
    UInt32_t   bits;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        bits = sigConfig_p->sizeBits;

        data = *((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte));
        data = NTOH32(data) >> sigConfig_p->offBits;
        if ( bits == 32 )
        {
            mask = 0xFFFFFFFF;
        }
        else
        {
            mask = (((UInt32_t)1 << bits) - 1);
        }

        data = data & mask;

        *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = data;
    }
    else
    {
        /* copy default value */
        *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic64bData
 *
 * DESCRIPTION:
 *   This function reads a 64bits word from the Input Message Buffer and copies it into the Parameter Buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic64bData (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt64_t data;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        data = *((UInt64_t*)(inMsgBuffer + sigConfig_p->offByte));

        *((UInt64_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = NTOH64(data);
    }
    else
    {
        /* Set default value, NB: default values have a maximum of 32 bits */
        *((UInt64_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic64bBitfield
 *
 * DESCRIPTION:
 *   This function reads a 64bits word from the Input Message Buffer, extract the required bits
 *   with shift and mask copies the result into the Parameter Buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic64bBitfield (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt64_t data;
    UInt64_t mask;
    UInt32_t bits;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        bits = sigConfig_p->sizeBits;

        data = *((UInt64_t*)(inMsgBuffer + sigConfig_p->offByte));
        data = NTOH64(data) >> sigConfig_p->offBits;
        mask = (((UInt64_t)1 << bits) - 1);

        *((UInt64_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = data & mask;
    }
    else
    {
        /* Set default value, NB: default values have a maximum of 32 bits */
        *((UInt64_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icBoolean32
 *
 * DESCRIPTION:
 *   This function extracts a bit from the Input Message Buffer (32 bits) (according to A664 boolean convention)
 *   with shift and mask and copies the result into the Parameter Buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icBoolean32 (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t data;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        /*Obtain the 32-bits word containing the boolean */
        data = *((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte));

        /*Shift the bit of interest to the Least Significant Position */
        data = NTOH32(data) BIT_RSHIFT sigConfig_p->offBits;

        /*Apply a mask to obtain the bit in the Least Significant Position */
        data = data & IOEN_MASK_LSB_IN_32BITS_WORD;

        /* Copy the resulting data in the the Parameter Buffer */
        /* Note: Models are using 8 bit booleans */
        if (ioen_icBoolIsIntegerConverter)
        {
            *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = data;
        }
        else
        {
            *((Byte_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = (Byte_t)data;
        }
    }
    else
    {
        /* Copy the resulting data in the the Parameter Buffer */
        /* Note: Models are using 8 bit booleans */
        if (ioen_icBoolIsIntegerConverter)
        {
            *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;
        }
        else
        {
            *((Byte_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = (Byte_t)parConfig_p->defaultValue.uint32;
        }
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_icBoolean
 *
 * DESCRIPTION:
 *   This function extracts a bit from the Input Message Buffer (8 bits) (according to A664 boolean convention)
 *   with shift and mask and copies the result into the Parameter Buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icBoolean8 (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t data;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        /*Obtain the 8-bits word containing the boolean */
        data = *((Byte_t*)(inMsgBuffer + sigConfig_p->offByte));

        /*Shift the bit of interest to the Least Significant Position */
        /* Only one byte, so no need to endian swap */
        data = data BIT_RSHIFT sigConfig_p->offBits;

        /*Apply a mask to obtain the bit in the Least Significant Position */
        data = data & IOEN_MASK_LSB_IN_32BITS_WORD;

        /* Copy the resulting data in the the Parameter Buffer */
        /* Note: Models are using 8 bit booleans */
        if (ioen_icBoolIsIntegerConverter)
        {
            *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = data;
        }
        else
        {
            *((Byte_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = data;
        }
    }
    else
    {
        /* Copy the resulting data in the the Parameter Buffer */
        /* Note: Models are using 8 bit booleans */
        if (ioen_icBoolIsIntegerConverter)
        {
            *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.uint32;
        }
        else
        {
            *((Byte_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = (Byte_t)parConfig_p->defaultValue.uint32;
        }
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}




/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic32bData_Int2Float
 *
 * DESCRIPTION:
 *   Read 32 bit integer value and convert it to float
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic32bData_Int2Float (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t   data;
    Float32_t  dataFloat;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        data      = *((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte));
        dataFloat =  NTOH32(data);

        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = dataFloat;
    }
    else
    {
        /* copy default value */
        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.float32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic16bData_Int2Float
 *
 * DESCRIPTION:
 *   Read 16 bit integer value and convert it to float
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic16bData_Int2Float (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt16_t   uData;
    SInt16_t   sData;
    Float32_t  dataFloat;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {

        uData = *((UInt16_t*)(inMsgBuffer + sigConfig_p->offByte));
        uData =  NTOH16(uData);
        sData = (SInt16_t)uData;
        dataFloat = (Float32_t)sData * sigConfig_p->lsbValue;

        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = dataFloat;
    }
    else
    {
        /* copy default value */
        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.float32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic32bData_Float2Int
 *
 * DESCRIPTION:
 *   Read 32 bit float value and convert it to integer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic32bData_Float2Int (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t     data;
    SInt32_t     dataSint;
    Float32_t    dataFloat;
    Bool_t       isValidFloat;
    Validity_t   newValidity;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        data = NTOH32(*((UInt32_t *)(inMsgBuffer + sigConfig_p->offByte)));

        isValidFloat = ioen_icuIsFloat(data);

        if ( isValidFloat == TRUE )
        {
            /* Interpret data as float and cast it to int */
            dataFloat = *(Float32_t *)&data;
            dataSint  = (SInt32_t) dataFloat;

            *((SInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = dataSint;

            newValidity = validity;
        }
        else
        {
            /* invalid data, use default value */
            *((SInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;

            newValidity.value   = IOEN_GLOB_DATA_INVALID;
            newValidity.ifValue = IOEN_VALIDITY_NODATA;
        }
    }
    else
    {
        /* copy default value */
        *((SInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;

        newValidity = validity;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &newValidity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icA429BNR
 *
 * DESCRIPTION:
 *   Convert Signed BNR label into floating point value
 *   The parameter validity is copied into the Parameter Buffer
 *
 *    Mask out significant bits and multiply with lsbValue
 *    if bit 28==1, result is negative, and data is in twos complement
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
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icA429BNR (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t  a429word;
    UInt32_t  ivalue;
    SInt32_t  svalue;
    Float32_t value;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        a429word = NTOH32(*((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte)));
        ivalue   = ((a429word BIT_AND 0x1FFFFFFF) BIT_RSHIFT sigConfig_p->offBits) BIT_AND ((1 BIT_LSHIFT (sigConfig_p->sizeBits)) - 1);

        if (((a429word BIT_RSHIFT 28) BIT_AND 1) == 1)
        {
            /* Bit extend the sign bit, NB: sizeBits can include the sign bit 28 */
            ivalue = ivalue BIT_OR (BIT_NOT((1 BIT_LSHIFT sigConfig_p->sizeBits) - 1));
        }
        else
        {
            /* No action as desgined */
        }

        /* Convert to signed integer */
        svalue = (SInt32_t) ivalue;

        value = (Float32_t)svalue * sigConfig_p->lsbValue;
        *((Float32_t  *)(inParamBufferSrc + sigConfig_p->parOffset)) = value;
    }
    else
    {
        /* copy default value */
        *((Float32_t  *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.float32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icA429BNR_Float2Int
 *
 * DESCRIPTION:
 *   Convert Signed BNR floating point value into an integer
 *   The parameter validity is copied into the Parameter Buffer
 *
 *    Mask out significant bits and multiply with lsbValue
 *    if bit 28==1, result is negative, and data is in twos complement
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
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icA429BNR_Float2Int (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t  a429word;
    UInt32_t  ivalue;
    SInt32_t  svalue;
    Float32_t value;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        a429word = NTOH32(*((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte)));
        ivalue   = ((a429word BIT_AND 0x1FFFFFFF) BIT_RSHIFT sigConfig_p->offBits) & ((1 BIT_LSHIFT sigConfig_p->sizeBits) - 1);

        if (((a429word BIT_RSHIFT 28) BIT_AND 1) == 1)
        {
            /* Bit extend the sign bit, NB: sizeBits can include the sign bit 28 */
            ivalue = ivalue BIT_OR (BIT_NOT((1 BIT_LSHIFT sigConfig_p->sizeBits) - 1));
        }
        else
        {
            /* No action as desined */
        }

        /* Convert to signed integer */
        svalue = (SInt32_t) ivalue;

        value = (Float32_t)svalue * sigConfig_p->lsbValue;
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = (SInt32_t)value;
    }
    else
    {
        /* copy default value */
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icA429UBNR
 *
 * DESCRIPTION:
 *   Convert Unsigned BNR label into floating point value
 *   The parameter validity is copied into the Parameter Buffer
 *
 *    Mask out significant bits and multiply with lsbValue
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icA429UBNR (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t  a429word;
    UInt32_t  ivalue;
    Float32_t value;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        a429word = NTOH32(*((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte)));
        ivalue   = (a429word >> sigConfig_p->offBits) & ((1 << sigConfig_p->sizeBits) - 1);
        value    = ivalue * sigConfig_p->lsbValue;

        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = value;
    }
    else
    {
        /* copy default value */
        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.float32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icA429UBNR_Float2Int
 *
 * DESCRIPTION:
 *   Convert Unsigned BNR label floating point value into an integer
 *   The parameter validity is copied into the Parameter Buffer
 *
 *    Mask out significant bits and multiply with lsbValue
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icA429UBNR_Float2Int (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt32_t  a429word;
    UInt32_t  ivalue;
    Float32_t value;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        a429word = NTOH32(*((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte)));
        ivalue   = (a429word >> sigConfig_p->offBits) & ((1 << sigConfig_p->sizeBits) - 1);
        value    = ivalue * sigConfig_p->lsbValue;

        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = value;
    }
    else
    {
        /* copy default value */
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icA429BCD
 *
 * DESCRIPTION:
 *   Read and Convert BCD label into floating point value
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icA429BCD (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = ioen_icuGetA429BCD(sigConfig_p, inMsgBuffer);
    }
    else
    {
        /* copy default value */
        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.float32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icA429BCD_Float2Int
 *
 * DESCRIPTION:
 *   Read and Convert BCD label from a floating point value to an integer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icA429BCD_Float2Int (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = ioen_icuGetA429BCD(sigConfig_p, inMsgBuffer);
    }
    else
    {
        /* copy default value */
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icA429UBCD
 *
 * DESCRIPTION:
 *   Read and Convert BCD label into floating point value without considering sign, i.e. result is always positive
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icA429UBCD (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = ioen_icuGetA429UBCD(sigConfig_p, inMsgBuffer);
    }
    else
    {
        /* copy default value */
        *((Float32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.float32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icA429UBCD_Float2Int
 *
 * DESCRIPTION:
 *   Read and Convert BCD label from floating point value to integer without considering sign, i.e. result is always positive
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icA429UBCD_Float2Int (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = ioen_icuGetA429UBCD(sigConfig_p, inMsgBuffer);
    }
    else
    {
        /* copy default value */
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic8bInt
 *
 * DESCRIPTION:
 *   This function reads an 8bits word from the Input Message Buffer and stores it with sign extension as 32 bits
 *   in the parameter buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic8bInt (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    SInt32_t   val;
    Float32_t  value;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        val   = *((SInt8_t *)(inMsgBuffer + sigConfig_p->offByte));   /* Read raw data */
        value =  val * sigConfig_p->lsbValue;                         /* Scaling */

        *((SInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = (SInt32_t)value;
    }
    else
    {
        /* copy default value */
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic16bInt
 *
 * DESCRIPTION:
 *   This function reads an 16bits word from the Input Message Buffer and stores it with sign extension as 32 bits
 *   in the parameter buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic16bInt (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    SInt32_t   val;
    Float32_t  value;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        val   = *((SInt16_t*)(inMsgBuffer + sigConfig_p->offByte));   /* Read raw data */
        value =  NTOH16(val) * sigConfig_p->lsbValue;                 /* Scaling */

        *((SInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = (SInt32_t)value;
    }
    else
    {
        /* copy default value */
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic8bUInt
 *
 * DESCRIPTION:
 *   This function reads an 8bits word from the Input Message Buffer and stores it without sign extension as 32 bits
 *   in the parameter buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic8bUInt (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    Byte_t     val;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        val = *((Byte_t *)(inMsgBuffer + sigConfig_p->offByte));

        *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = val;
    }
    else
    {
        /* copy default value */
        *((Byte_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = (Byte_t)parConfig_p->defaultValue.uint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic16bUInt
 *
 * DESCRIPTION:
 *   This function reads a 16bits word from the Input Message Buffer and stores it without sign extension as 32 bits
 *   in the parameter buffer
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic16bUInt (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    UInt16_t   val;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        val = *((UInt16_t*)(inMsgBuffer + sigConfig_p->offByte));
        val = NTOH16(val);

        *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = (UInt32_t)val;
    }
    else
    {
        /* copy default value */
        *((UInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = (UInt16_t)parConfig_p->defaultValue.uint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_ic8bInt_Add
 *
 * DESCRIPTION:
 *   This function reads a 32bits word from the Input Message Buffer, extract the required bits
 *   with shift and mask.
 *   The parameter validity is copied into the Parameter Buffer
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_ic8bInt_Add (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    signed char  val;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_VALID)
    {
        val = *((signed char *)(inMsgBuffer + sigConfig_p->offByte));

        *((SInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) += val;
    }
    else
    {
        /* copy default value */
        *((SInt32_t *)(inParamBufferSrc + sigConfig_p->parOffset)) = parConfig_p->defaultValue.sint32;
    }

    /* Store parameter Validity */
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validity);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icMsgUnfresh
 *
 * DESCRIPTION:
 *   This function sets a boolean to
 *      TRUE if its message is unfresh
 *      FALSE if its message is fresh
 *   with shift and mask and copies the result into the Parameter Buffer
 *   The parameter's validity is always valid
 *
 * INTERFACE:
 *
 *   In:  validity      : Calculated validity based on configured validities
 *   In:  parConfig_p   : Parameter Config
 *   In:  sigConfig_p   : Input Signal Config
 *   In:  inMsgBuffer   : Input message buffer
 *   In:  inParamBufferSrc : Parameter buffer where the converted input signals are written to
 *
 ******************************************************************/
void ioen_icMsgUnfresh (
    /* IN     */ const Validity_t                   validity,
    /* IN     */ const ParamMappingConfig_t * const parConfig_p,
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer,
    /* IN     */       Byte_t               * const inParamBufferSrc
)
{
    Validity_t  validityNew;

    /* Check input validity */
    if (validity.value == IOEN_GLOB_DATA_LOST)
    {
        /* Note: Models are using 8 bit booleans */
        if (ioen_icBoolIsIntegerConverter)
        {
            *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = TRUE;
        }
        else
        {
            *((Byte_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = TRUE;
        }
    }
    else
    {
        /* Copy the resulting data in the the Parameter Buffer */
        /* Note: Models are using 8 bit booleans */
        if (ioen_icBoolIsIntegerConverter)
        {
            *((UInt32_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = FALSE;
        }
        else
        {
            *((Byte_t*)(inParamBufferSrc + sigConfig_p->parOffset)) = FALSE;
        }
    }

    /* Store parameter Validity NB: always valid */
    validityNew.spare          = 0;
    validityNew.selectedSource = 0;
    validityNew.value          = IOEN_GLOB_DATA_VALID;
    validityNew.ifValue        = IOEN_VALIDITY_NORMALOP;
    *(UInt32_t *)(inParamBufferSrc + sigConfig_p->valOffset) = * ((UInt32_t *) &validityNew);

}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icInitInputConverter
 *
 * DESCRIPTION:
 * This function initialises the Input Converter global data.
 * This function must be called before any conversion is performed.
 *
 * INTERFACE:
 *   Global Data      :  ioen_icBoolIsIntegerConverter
 *
 *   In:  bool_is_int : size of a boolean value (TRUE = 32 bits, FALSE = 8 bits)
 *
 ******************************************************************/
void ioen_icInitInputConverter (
    /* IN     */ const Bool_t               bool_is_int
)
{
    ioen_icBoolIsIntegerConverter = bool_is_int;
}


