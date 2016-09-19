/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_IomApi.h
 *
 * DESCRIPTION:
 *   Exported functions of the IOM
 *
 *
***************************************************************/

#ifndef IOEngineApi_h_included
#define IOEngineApi_h_included

#include "ioen_Platform.h"
#include "ioen_GeneralTypes.h"
#include "ioen_ByteSwapping.h"
#include <apex/apexLib.h>
#include "ioen_IomConfig.h"



/* Control structure for current confirmation */
typedef struct Valid_t
{
    Validity_t      current;                      /* Current validity of item             */
    Validity_t      last;                         /* Previous validity of item            */
    Validity_t      confirmed;                    /* Value of confirmed validity          */
} Valid_t;



/* Control structure to perform the refreshing of data */
typedef struct IomObjectCtrlData_t
{
    Byte_t          limitCycleValid;     /* Number of cycles for next freshness check           */
    Byte_t          limitCycleInvalid;   /* Number of cycles for next unfreshness check         */
    Byte_t          cycle;               /* Current cycle count                                 */
    Byte_t          newData;             /* flag to indicate new data was received in the cycle */
    Valid_t         validity;
} IomObjectCtrlData_t;

/* Control structure to perform the refreshing of data */
typedef struct IomMessageCtrlData_t
{
    APEX_INTEGER         apexPortId;     /* Port ID returned by APEX create port function       */
    UInt32_t             readCycle;      /* Current cycle count before read is required         */
    UInt16_t             fcCtrl;         /* hold the previous Freshness counter value           */
    UInt16_t             spare;          /* 32 bit alignment                                    */
    IomObjectCtrlData_t  objCtrl;        /* Control structure to perform the refreshing of data */
} IomMessageCtrlData_t;



IDU_ID      ioen_Get_Idu_Id(void);


void        ioen_readMessagesAfdx (
               /*        */       void
            );


void        ioen_writeMessagesAfdx (
               /*        */       void
            );

void        ioen_writeMessagesCan (
               /*        */       void
            );

void        ioen_readMessagesCan (
                /*        */ void
            );

void        ioen_readMessagesA429 (
                /*        */ void
            );

void        ioen_processInputAfdxA429 (
               /*        */       void
            );

void        ioen_processOutputAfdx (
               /*        */       void
            );

void        ioen_initializeIom (
                /* IN     */       Bool_t                       inBoolIsInt,
                /* IN     */       Bool_t                       outBoolIsInt,
                /* IN     */       Byte_t               * const inMessageBufferAfdxA429_p,
                /* IN     */       Byte_t               * const inParamBuffer_p,
                /* IN     */       Byte_t               * const inParamBufferSource_p,
                /* IN     */       Byte_t               * const outMessageBuffer_p,
                /* IN     */ const Byte_t               * const outParamBuffer_p,
                /* IN     */       IomObjectCtrlData_t          (* inObjectCtrl)[IOEN_MAX_SOURCES_PER_INPUT]
            );

void        ioen_createPorts (
                /* IN     */ const IOMConfigHeader_t        * const iom_p,
                /* IN OUT */       IOMCanRoutingPortTbl_t   * const iomCanRtTbl_p,
                /* IN */       IOMA429RoutingPortTbl_t   * const iomA429RtTbl_p,
                /* IN     */ const UInt32_t                         appPeriodNanosec
            );


#endif
