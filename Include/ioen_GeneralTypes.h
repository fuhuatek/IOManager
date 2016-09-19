/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_GeneralTypes.h
 *
 * DESCRIPTION:
 *   Common Types and defines for IOM
 *
 *
***************************************************************/

#ifndef IOMAPTypes_h_included
#define IOMAPTypes_h_included

/* #include <limits.h> */

typedef char               Char_t;
typedef unsigned char      Bool_t;
typedef unsigned char      Byte_t;
typedef signed   char      SInt8_t;

typedef unsigned short     UInt16_t;
typedef signed   short     SInt16_t;

typedef unsigned int       UInt32_t;
typedef signed   int       SInt32_t;

typedef unsigned long long UInt64_t;
typedef signed   long long SInt64_t;

typedef float              Float32_t;
typedef double             Float64_t;

typedef unsigned long      ErrorId_t; /* This type is used to report errors. */


/* UNION to convert between different data types */
typedef union DataConverter_t
{
    UInt32_t    uint32;
    SInt32_t    sint32;
    Float32_t   float32;
} DataConverter_t;


/* Generally used Constants */
#define NULL_PTR            ((void *)0)


/* Ranges for Primitive Types */
#define MIN_SIGNED_32 (-2147483648L) /* 2^31 0x80000000 -2147483648L */
#define MAX_SIGNED_32 (2147483647L)  /* 2^31-1 0x7FFFFFFF 2147483647 */


/* 0x7F800000 Positive infinity */
/* 0xFF800000 Negative infinity */
/* 0x7FC00000 Not a Number      */
#define IOEN_FLOAT_32_INFINITY_INTEGER_VALUE (0x7F800000UL)

/* 0x7FF0000000000000ULL Positive infinity */
/* 0xFFF0000000000000ULL Negative infinity */
/* 0x7FFFFFFFFFFFFFFFULL Not a Number      */
#define IOEN_FLOAT_64_INFINITY_INTEGER_VALUE (0x7FF0000000000000ULL)

#ifndef AND
    #define AND           &&
#endif
#ifndef OR
    #define OR            ||
#endif
#ifndef NOT
    #define NOT           !
#endif
#ifndef BIT_AND
    #define BIT_AND       &
#endif
#ifndef BIT_OR
    #define BIT_OR        |
#endif
#ifndef BIT_XOR
    #define BIT_XOR       ^
#endif
#ifndef BIT_LSHIFT
    #define BIT_LSHIFT    <<
#endif
#ifndef BIT_RSHIFT
    #define BIT_RSHIFT    >>
#endif
#ifndef BIT_NOT
    #define BIT_NOT       ~
#endif

/* Ceil function for positive floating point numbers */
#define CEILING_POS(X) ((X-(UInt32_t)(X)) > 0 ? (UInt32_t)(X+1) : (UInt32_t)(X))

#endif
