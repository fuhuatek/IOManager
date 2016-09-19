/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_Platform.h
 *
 * DESCRIPTION:
 *   Defines to indicate which platform the IOM is to be run on
 *
 *
***************************************************************/


#ifndef IOEN_PLATFORM_H_
#define IOEN_PLATFORM_H_

#define LITTLE_ENDIAN 0x1234
#define BIG_ENDIAN    0x4321


#define IOEN_IMA_A664HEADER_PADDING_SIZE 32

#ifdef IOEN_PLATFORM_ADS2
#define TARGET_BYTE_ORDER LITTLE_ENDIAN
#define IOEN_A664MSG_HEADER_EXTRA_PADDING IOEN_IMA_A664HEADER_PADDING_SIZE
#endif

#ifdef IOEN_PLATFORM_IDU
#define TARGET_BYTE_ORDER BIG_ENDIAN
#define IOEN_A664MSG_HEADER_EXTRA_PADDING IOEN_IMA_A664HEADER_PADDING_SIZE
#endif

#ifdef IOEN_PLATFORM_IMA
#define TARGET_BYTE_ORDER BIG_ENDIAN
#define IOEN_A664MSG_HEADER_EXTRA_PADDING 0
#endif

#ifdef IOEN_PLATFORM_DUMMY
#define TARGET_BYTE_ORDER BIG_ENDIAN
#define IOEN_A664MSG_HEADER_EXTRA_PADDING IOEN_IMA_A664HEADER_PADDING_SIZE
#endif

#endif /* IOEN_PLATFORM_H_ */

