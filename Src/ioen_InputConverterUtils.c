/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_InputConverterUtils.c
 *
 * FILE DESCRIPTION:
 *   This module contains utility functions to aid the conversion of input signals to application input parameters.
 *
***************************************************************/

#include "ioen_IomLocal.h"



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icuIsFloat
 *
 * DESCRIPTION:
 *   This function checks a value to see if it is a valid floating point number
 *
 * INTERFACE:
 *   Global Data      :  ioen_inParamBuffer
 *
 *   In:  floatToCheck    : value to check if it is a valid floating point number
 *
 *   Return: TRUE : number is a valid floating point number
 *
 ******************************************************************/
Bool_t ioen_icuIsFloat (
    /* IN     */ const UInt32_t floatToCheck
)
{
    Bool_t isValidFloat;

    if (   ((( floatToCheck & IOEN_FLOAT_32_INFINITY_INTEGER_VALUE ) == 0) && ( floatToCheck != 0))            /* Invalid exponent (denormalised number) */
        || (( floatToCheck & IOEN_FLOAT_32_INFINITY_INTEGER_VALUE ) == IOEN_FLOAT_32_INFINITY_INTEGER_VALUE)   /* infinity / NaN                         */
       )
    {
        isValidFloat = FALSE;
    }
    else
    {
        isValidFloat = TRUE;
    }

    return isValidFloat;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icuIsDouble
 *
 * DESCRIPTION:
 *   This function checks a value to see if it is a valid double precision floating point number
 *
 * INTERFACE:
 *   Global Data      :  ioen_inParamBuffer
 *
 *   In:  doubleToCheck    : value to check if it is a valid double precision floating point number
 *
 *   Return: TRUE : number is a valid double precision floating point number
 *
 ******************************************************************/
Bool_t ioen_icuIsDouble (
    /* IN     */ const UInt64_t doubleToCheck
)
{
    Bool_t isValidDouble;

    if (   ((( doubleToCheck & IOEN_FLOAT_64_INFINITY_INTEGER_VALUE ) == 0) && ( doubleToCheck != 0))           /* Invalid exponent (denormalised number) */
        || (( doubleToCheck & IOEN_FLOAT_64_INFINITY_INTEGER_VALUE ) == IOEN_FLOAT_64_INFINITY_INTEGER_VALUE)   /* infinity / NaN                         */
       )
    {
        isValidDouble = FALSE;
    }
    else
    {
        isValidDouble = TRUE;
    }

    return isValidDouble;
}



/******************************************************************
 * FUNCTION NAME:
 *   ioen_icuGetA429BCD
 *
 * DESCRIPTION:
 *   This function Convert BCD label into floating point value.
 *  
 *  Starting from lsb, groups of 4 bits represent a digit
 *  If the most significant digit may have less then 4 bits
 *  lsbValue is considered the value of units of the least significant digit
 *  SSM values 00 and 11 are considered as + and - sign,
 *  Other values of SSM are ignored, i.e. the resulting value will be positive but
 *  this should not occur since for NCD the signal is treated as invalid and not extracted
 * 
 * -----------------------------------------------------------------------------------------------------
 * |31 | 30 | 29 | 28,27,26  | 25,24,23,22 | 21,20,19,18 | 17,16,15,14 | 13,12,11,10 | 9 | 8 | 7 ... 0 |
 * |---------------------------------------------------------------------------------------------------|
 * | P |   SSM   | BCD Char1 |  BCD Char2  |  BCD Char3  |  BCD Char4  |  BCD Char5  |  SDI  |  Label  |
 * -----------------------------------------------------------------------------------------------------
 * 
 * P     = Parity=Odd parity
 * SSM   = Sign Status Matrix, 00= Positive, 01=No computed Data, 10=Functional Test, 11=negative
 * SDI   = Source / Destination Identifier
 * Label = 8 bit label ID, usually specified in Octal
 * 
 * INTERFACE:
 *
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  inMsgBuffer    : Input message buffer
 *
 *   Return: floating point value read from BCD message
 *
 ******************************************************************/
Float32_t ioen_icuGetA429BCD (
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer
)
{
    UInt32_t  a429word;
    UInt32_t  ivalue;
    UInt32_t  bcdvalue;
    Float32_t value;
    int       u;
    int       bits;

    u      = 1;
    ivalue = 0;

    a429word = NTOH32(*((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte)));

    bcdvalue = (a429word >> sigConfig_p->offBits) & ((1 << sigConfig_p->sizeBits) - 1);

    bits = sigConfig_p->sizeBits;

    /* convert BCD digits into unsigned integer number */
    /* most significant bits may be less then 4, but the whole word is already masked, */
    /* so it is safe to do as there where 4 bits */
    while (bits > 0)
    {
        ivalue     = ivalue + (u * (bcdvalue & 0xF));
        bcdvalue >>= 4;  /* each BCD Digit has 4 bits */
        u         *= 10; /* Decimal *10 multiplier    */
        bits      -= 4;  /* bits to go                */
    }

    value = ivalue * sigConfig_p->lsbValue;

    if ((a429word & 0x60000000) == 0x60000000) /* SSM = 11b means negative */
    {
        value = -value;
    }
    else
    {
        /* No action as desgined */
    }

    return value;
}


/******************************************************************
 * FUNCTION NAME:
 *   ioen_icuGetA429UBCD
 *
 * DESCRIPTION:
 *   This function Converts a BCD label into floating point value without considering sign, i.e. result is always positive.
 *  
 *  Starting from lsb, groups of 4 bits represent a digit
 *  If the most significant digit may have less then 4 bits
 *  lsbValue is considered the value of units of the least significant digit
 *  SSM values 00 and 11 are considered as + and - sign,
 *  Other values of SSM are ignored, i.e. the resulting value will be positive but
 *  this should not occur since for NCD the signal is treated as invalid and not extracted
 * 
 * -----------------------------------------------------------------------------------------------------
 * |31 | 30 | 29 | 28,27,26  | 25,24,23,22 | 21,20,19,18 | 17,16,15,14 | 13,12,11,10 | 9 | 8 | 7 ... 0 |
 * |---------------------------------------------------------------------------------------------------|
 * | P |   SSM   | BCD Char1 |  BCD Char2  |  BCD Char3  |  BCD Char4  |  BCD Char5  |  SDI  |  Label  |
 * -----------------------------------------------------------------------------------------------------
 * 
 * P     = Parity=Odd parity
 * SSM   = Sign Status Matrix, 00= Positive, 01=No computed Data, 10=Functional Test, 11=negative
 * SDI   = Source / Destination Identifier
 * Label = 8 bit label ID, usually specified in Octal
 * 
 * INTERFACE:
 *
 *   In:  sigConfig_p    : Input Signal Config
 *   In:  inMsgBuffer    : Input message buffer
 *
 *   Return: floating point value read from BCD message
 *
 ******************************************************************/
Float32_t ioen_icuGetA429UBCD (
    /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
    /* IN     */ const Byte_t               * const inMsgBuffer
)
{
    UInt32_t a429word;
    UInt32_t ivalue;
    UInt32_t bcdvalue;
    int      u;
    int      bits;

    u = 1;

    a429word = NTOH32(*((UInt32_t*)(inMsgBuffer + sigConfig_p->offByte)));

    bcdvalue = (a429word >> sigConfig_p->offBits) & ((1 << sigConfig_p->sizeBits) - 1);

    bits = sigConfig_p->sizeBits;

    /* convert BCD digits into unsigned integer number */
    /* most significant bits may be less then 4, but the whole word is already masked, */
    /* so it is safe to do as there where 4 bits */
    ivalue = 0;
    while (bits > 0)
    {
        ivalue     = ivalue + (u * (bcdvalue & 0xF));
        bcdvalue >>= 4;  /* each BCD Digit has 4 bits */
        u         *= 10; /* Decimal *10 multiplier    */
        bits      -= 4;  /* bits to go                */
    }

    return (Float32_t)ivalue * sigConfig_p->lsbValue;
}



