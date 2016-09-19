/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_IomConfigRuntime.h
 *
 * DESCRIPTION:
 *   Types used at runtime of the IO Manger
 *
 *
***************************************************************/



#ifndef IOMAppConfigRuntime_h_included
#define IOMAppConfigRuntime_h_included

#include "ioen_IomConfig.h"


/* =================================================================================== */
/* Runtime structure definitions */
/* =================================================================================== */

/* Message header structure prepended before each AFDX message in the input buffer */
typedef struct AfdxMessageHeader_t
{
    UInt32_t freshness;         /* IOEN_MSG_UNFRESH or IOEN_MSG_FRESH */
    UInt32_t crcTest;           /* IOEN_FAILED or IOEN_PASSED         */
    UInt32_t frameCounterTest;  /* IOEN_FAILED or IOEN_PASSED         */
    UInt32_t padding[5];
    /* extra padding space for the platforms not providing an extra message header
     * for integrity data as provided by in IMA Platform.
     * On IDU this is not provided, so we have to add this extra padding because
     * The signal offsets computed by the config tool are assuming 64 byte space at the
     * beginning of a message, independently of the platform.
     */
    Byte_t EXTRA_PADDING[IOEN_A664MSG_HEADER_EXTRA_PADDING];
} AfdxMessageHeader_t;



/* Control structure to perform the refreshing of data for A429 */
typedef struct A429MessageCtrlData_t
{
    A429MessageInfo_t    *msgInfo_p;      /* Also used as robustness, when not 0   */
    IomObjectCtrlData_t   ctrl;           /* Used to calculate freshness of data   */
} A429MessageCtrlData_t;

/* Lookup structure to save searching configuration for label config each time */
typedef struct A429MessageCtrl_t
{
    APEX_INTEGER          apexPortId;                                         /* Port ID returned by APEX create port function */
    A429MessageCtrlData_t data[IOEN_A429_MAX_LABELS][IOEN_A429_MAX_SDI];      /* space for all labels of one port              */
} A429MessageCtrl_t;


/* A429 Raw Data in message buffer per SDI */
typedef struct A429RawData_t
{
    UInt32_t           freshness;      /* Validity of data       */
    UInt32_t           raw;            /* A429 received raw data */
} A429RawData_t;

/* A429 Raw Data in message buffer per Label */
typedef struct A429MessageData_t
{
    A429RawData_t      data[IOEN_A429_MAX_SDI]; /* A429 received raw data per Source Destination Index */
} A429MessageData_t;




/* Output Port Information */
typedef struct OutputPortInfo_t
{
    APEX_INTEGER       apexPortId;     /* Port Id returned by the APEX driver on creation of the output port */
    SInt32_t           countdownValue; /* Countdown Value in number of App Periods      */
    UInt32_t           countdownInit;  /* Countdown Init Value in number of App Periods */
    UInt16_t           fcValue;        /* Freshness counter value                       */
    UInt16_t           spare;          /* 32 bit alignment                              */
} OutputPortInfo_t;

/* Output Ports Information */
typedef struct OutputPortsInfo_t
{
    OutputPortInfo_t   port[IOEN_MAX_TX_PORT_NUMBER];
} OutputPortsInfo_t;



/************************************************************************/
/* Dynamic information about the source sets                            */
typedef struct SelectionSetInfo_t
{
    UInt32_t                    selectedSource;                                /* currently selected source in a set                       */
    UInt32_t                    nextSource;                                    /* Next source in a set to be selected                      */
    UInt32_t                    sourceLockTimeoutMs;                           /* duration of selected source lock in millisecs            */
    UInt32_t                    healthScore [IOEN_MAX_SOURCES_PER_INPUT];      /* number of parameters in this source with an FS of NO     */
    IomObjectCtrlData_t         ctrl[IOEN_MAX_SOURCES_PER_INPUT];              /* Used to calculate freshness of set validity              */
    IomObjectCtrlData_t         lock[IOEN_MAX_SOURCES_PER_INPUT];              /* Used to calculate the lock interval of the source        */
} SelectionSetInfo_t;


/************************************************************************/





#endif
