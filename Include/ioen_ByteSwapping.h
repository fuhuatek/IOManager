/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_ByteSwapping.h
 *
 * DESCRIPTION:
 *   Inline functions, types and defines for byte swapping
 *
 *
***************************************************************/


#ifndef IOEN_BYTESWAPPING_H_
#define IOEN_BYTESWAPPING_H_

#include "ioen_GeneralTypes.h"
#include "ioen_Platform.h"

#if TARGET_BYTE_ORDER==LITTLE_ENDIAN

/* Byte Swapping */
/* static inline UInt16_t */
static UInt16_t
swap16(UInt16_t src)
{
    return ((src) >> 8) | (unsigned short)((src) << 8);
}

/* static inline UInt32_t */
static UInt32_t
swap32(UInt32_t src)
{
    return   (((src) << 24) & 0xff000000)
           | (((src) >> 24) & 0x000000ff)
           | (((src) << 8)  & 0x00ff0000)
           | (((src) >> 8)  & 0x0000ff00);
}

/* static inline UInt64_t */
static UInt64_t
swap64(UInt64_t src)
{
    union
    {
        UInt64_t    q;
        UInt32_t    l[2];
    } out, in;

    in.q = src;
    out.l[0] = swap32(in.l[1]);
    out.l[1] = swap32(in.l[0]);
    return out.q;
}

#define NTOH16(x) swap16(x)
#define NTOH32(x) swap32(x)
#define NTOH64(x) swap64(x)
#define HTON16(x) swap16(x)
#define HTON32(x) swap32(x)
#define HTON64(x) swap64(x)

#else

/* The following defines have no action on a big endian target */
#define NTOH16(x) (x)
#define NTOH32(x) (x)
#define NTOH64(x) (x)
#define HTON16(x) (x)
#define HTON32(x) (x)
#define HTON64(x) (x)

#endif

#endif /* IOEN_BYTESWAPPING_H_ */
