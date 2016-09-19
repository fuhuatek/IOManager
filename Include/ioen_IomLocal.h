/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_IomLocal.h
 *
 * DESCRIPTION:
 *   Functions, Types and defines used locally within the IOM
 *
 *
***************************************************************/

#ifndef ioen_IoEngineLocal_h_included
#define ioen_IoEngineLocal_h_included

#include <string.h>
#include <apex/apexLib.h>
#include "LibError.h"
#include "ioen_IomApi.h"
#include "ioen_IomConfigRuntime.h"

#define MS_TO_NS (1000000)

#ifndef IOM_DEBUG
    /* #define CHECK_CODE(msg, ret); */
    #define PRINT_DBG(...)
#else
extern unsigned int delayTick;    
#define CHECK_CODE(msg, ret)                                                \
    {                                                                       \
    if (ret != NO_ERROR && ret != NO_ACTION && ret != NOT_AVAILABLE)        \
        printf ("%s error : %s\n", msg, codeToStr(ret));                    \
    }
#define PRINT_DBG(...)                                                      \
    {                                                                       \
        printf (__VA_ARGS__);                                               \
        taskDelay(10*delayTick);                                            \
    }        
#endif


/*
 * These pointers are initialized in ioen_initializeIom (ioen_icInitInputConverter). They are assumed to
 * to the global message buffer and global parameter buffer.
 */
extern       Byte_t                      *ioen_inParamBuffer;
extern       Byte_t                      *ioen_inParamBufferSrc;
extern       IomObjectCtrlData_t         (*ioen_inObjectCtrl)[IOEN_MAX_SOURCES_PER_INPUT];
extern       Byte_t                      *ioen_inMsgBuffer;
extern       const Byte_t                *ioen_outParamBuffer;
extern       Byte_t                      *ioen_outMsgBuffer;
extern       const IOMConfigHeader_t     *ioen_iomConfig_p;

extern       const UInt32_t               crc32LookupTable[IOEN_CRC32_TABLE_SIZE];

extern       Bool_t                       ioen_icBoolIsIntegerConverter;
extern       Float32_t                    ioen_processPeriodicTimeMs;
extern       SelectionSetInfo_t           ioen_selectionSetlist [IOEN_MAX_NUMBER_OF_SELECTION_SETS];

extern       Bool_t                       ioen_outBoolIsInteger;



extern void       (*ioen_inputConverters[IOEN_NUMBER_INPUT_MAPPING_TYPES])(const Validity_t, const ParamMappingConfig_t*, const InputSignalConfig_t*, const Byte_t *, Byte_t *);
extern Validity_t (*ioen_validityChecker[IOEN_NUMBER_CONDITION_TYPES])(const Byte_t *, const ValidityConditionConfig_t *);
extern void       (*ioen_outputConverters[IOEN_NUMBER_OUTPUT_MAPPING_TYPES])(const ParamMappingConfig_t*, const InputSignalConfig_t*, const Byte_t *, Byte_t *);

extern IomMessageCtrlData_t ioen_afdxMsgCtrl        [IOEN_MAX_RX_PORT_NUMBER]; /* one entry per message (AFDX) */
extern A429MessageCtrl_t    ioen_a429MsgControl     [IOEN_A429_MAX_PORTS];     /* one entry per message (A429) */

extern OutputPortsInfo_t    ioen_outputPortsInfoCan;
extern OutputPortsInfo_t    ioen_outputPortsInfoAfdx;






void        ioen_aaInitSignalConfirmation (
                /* IN     */ const IOMConfigHeader_t   * const iom_p,
                /* IN     */ const Float32_t                   appPeriodMs
            );

void        ioen_aaInitOutputBuffer (
                /* IN     */ const IOMConfigHeader_t   * const iom_p,
                /*    OUT */       Byte_t              * const outMessageBuffer_p
            );

void        ioen_aaNewMessageAfdx (
                /* IN     */ const AfdxMessageInfo_t        * const msginfo_p,
                /* IN     */ const void                     * const messageStart_p,
                /* IN OUT */       IomMessageCtrlData_t     * const msgCtrl_p
            );

Bool_t      ioen_icuIsFloat (
                /* IN     */ const UInt32_t floatToCheck
            );

Bool_t      ioen_icuIsDouble (
                /* IN     */ const UInt64_t doubleToCheck
            );

Float32_t   ioen_icuGetA429BCD (
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer
            );

Float32_t   ioen_icuGetA429UBCD (
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer
            );

void        ioen_icMultipleBytes (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icA664String (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic32bInt (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic32bUInt (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );			

void        ioen_ic32Float (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic64Float (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic8bBitfield (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic32bBitfield (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic64bData (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic64bBitfield (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icBoolean32 (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icBoolean8 (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic32bData_Int2Float (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic16bData_Int2Float (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBufferSrc
            );

void        ioen_ic32bData_Float2Int (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icA429BNR (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icA429BNR_Float2Int (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icA429UBNR (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icA429UBNR_Float2Int (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icA429BCD (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icA429BCD_Float2Int (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icA429UBCD (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icA429UBCD_Float2Int (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic8bInt (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic16bInt (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic8bUInt (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic16bUInt (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_ic8bInt_Add (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icMsgUnfresh (
                /* IN     */ const Validity_t                   validity,
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const InputSignalConfig_t  * const sigConfig_p,
                /* IN     */ const Byte_t               * const inMsgBuffer,
                /* IN     */       Byte_t               * const inParamBuffer
            );

void        ioen_icInitInputConverter (
                /* IN     */ const Bool_t                       bool_is_int
            );


void        ioen_imaCopyToParamBuffer (
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const UInt16_t                     numParams,
                /* IN     */ const UInt32_t                     selectedSource,
                /* IN OUT */       Valid_t              * const valid_p
            );

void        ioen_imaDoInputMappings (
                /* IN     */ const ParamMappingConfig_t * const parConfig_p,
                /* IN     */ const UInt16_t                     numParams,
                /* IN OUT */       IomObjectCtrlData_t          objectCtrl[IOEN_MAX_SOURCES_PER_INPUT]
            );

void        ioen_imaProcessDataset (
                /* IN     */ const InputDatasetHeader_t     * const dataset_p,
                /* IN OUT */       IomObjectCtrlData_t              inObjectCtrl[IOEN_MAX_SOURCES_PER_INPUT]
            );

Validity_t  ioen_vcCheckCondition_Freshness (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcCheckCondition_A664FS (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcCheckCondition_FreshnessFs (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcCheckCondition_ValidityValue (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcGet_SSM_BNR
            (
                /* IN     */ const UInt32_t                          ssmval
            );

Validity_t  ioen_vcCheckCondition_SSM_BNR (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcCheckCondition_SSM_BNR_Combine
            (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcGet_SSM_DIS (
                /* IN     */ const UInt32_t                          ssmval
            );

Validity_t  ioen_vcCheckCondition_SSM_DIS (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcCheckCondition_SSM_DIS_Combine
            (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcGet_SSM_BCD
            (
                /* IN     */ const UInt32_t                          ssmval
            );

Validity_t  ioen_vcCheckCondition_SSM_BCD (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcCheckCondition_SSM_BCD_Combine
            (
                /* IN     */ const Byte_t                    * const inMsgBuffer,
                /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
            );

Validity_t  ioen_vcCheckCondition_RangeInt (
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
);

Validity_t  ioen_vcCheckCondition_RangeUint (
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
);

Validity_t  ioen_vcCheckCondition_RangeFloat (
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
);

Validity_t  ioen_vcCheckCondition_RangeFloatBnr (
    /* IN     */ const Byte_t                    * const inMsgBuffer,
    /* IN     */ const ValidityConditionConfig_t * const conditionConfig_p
);

Validity_t  ioen_ssCheckSourceValidity(
                /* IN     */ const Byte_t                   * const inMsgBuffer_p,
                /* IN     */ const ValidityConfig_t         * const validityConfig_p
            );

void        ioen_ssGetSourceValidity (
                /* IN     */ const ValidityConfig_t         * const validityLogic_p,
                /* IN     */ const UInt32_t                         numSources,
                /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
            );

Validity_t  ioen_ssCheckLicParameter (
                /* IN     */ const UInt32_t                     curSource,
                /* IN     */ const LicParamConfig_t     * const iomLicParamConfig_p,
                /* IN     */ const UInt32_t                     selectedSource,
                /* IN OUT */       UInt32_t             * const nextSource_p
            );

void        ioen_ssPerformHealthScoreSelection (
                /* IN     */ const UInt32_t                        curSet,
                /* IN     */ const SelectionSetConfig_t    * const iomSetConfig_p
            );

void        ioen_ssPerformLicSelection (
                /* IN     */ const IOMConfigHeader_t       * const iom_p,
                /* IN     */ const UInt32_t                        curSet,
                /* IN     */ const SelectionSetConfig_t    * const iomSetConfig_p
            );

void        ioen_ssPerformObjectValidSelection (
                /* IN     */ const UInt32_t                        curSet,
                /* IN     */ const SelectionSetConfig_t    * const iomSetConfig_p
            );

void        ioen_ssPerformSourceSelection (
                /* IN     */ const IOMConfigHeader_t    * const iom_p
            );

void        ioen_ssCalcSourceSelectionObjects (
                /* IN     */ const IOMConfigHeader_t    * const iom_p
            );

void        ioen_ssInitSourceSelection(
                /* IN     */ const IOMConfigHeader_t   * const iom_p,
                /* IN     */ const Float32_t                   appPeriodMs
            );

void        ioen_omaProcessOutputDatasetAfdx (
                /* IN     */ const OutputDatasetConfig_t   * const dataset_p
            );



void        ioen_ocWrite8bData (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWrite16bData (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWrite32bData (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWrite64bData (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWriteMultipleBytes (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWriteBoolean (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWrite32bBitfield (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWriteA429BNR_Float (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWriteA429UBNR_Float (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWriteA429BNR_Int (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWriteA429UBNR_Int (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWriteA429BCD_Float (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWriteA429BCD_Int (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_ocWriteValidityBitfield (
                /* IN     */ const ParamMappingConfig_t   * const parConfig_p,
                /* IN     */ const InputSignalConfig_t    * const sigConfig_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );

void        ioen_omProcessOutputDatasetCan (
                /* IN     */ const OutputDatasetConfig_t   * const dataset_p
            );

void        ioen_createRxPortsAfdx (
                /* IN     */ const IOMConfigHeader_t   * const iom_p,
                /* IN     */ const Float32_t                   appPeriodMs
            );

void        ioen_createTxPortsAfdx (
                /* IN     */ const IOMConfigHeader_t   * const iom_p,
                /* IN     */ const Float32_t                   appPeriodMs
            );

void        ioen_initCanControl (
                /* IN     */ const Float32_t                   appPeriodMs
            );

void        ioen_createPortsCan (
                /* IN     */ const IOMConfigHeader_t        * const iom_p,
                /* IN OUT */       IOMCanRoutingPortTbl_t   * const iomCanRtTbl_p,
                /* IN     */ const Float32_t                        appPeriodMs
            );

void        ioen_createPortsA429 (
                /* IN     */ const IOMConfigHeader_t        * const iom_p,
                /* IN  */       IOMA429RoutingPortTbl_t   * const iomA429RtTbl_p,
                /* IN     */ const Float32_t                        appPeriodMs
            );

void        ioen_imcStartInputCan (
                /*        */ void
            );

void        ioen_imaProcessDatasetSources (
                /* IN     */ const InputDatasetHeader_t     * const dataset_p,
                /* IN OUT */       IomObjectCtrlData_t              objectCtrl[IOEN_MAX_SOURCES_PER_INPUT]
            );

void        ioen_imaProcessDatasetValue (
                /* IN     */ const IOMConfigHeader_t        * const iom_p,
                /* IN     */ const InputDatasetHeader_t     * const dataset_p,
                /* IN OUT */       IomObjectCtrlData_t              objectCtrl[IOEN_MAX_SOURCES_PER_INPUT]
            );

UInt32_t    ioen_im4GetA429Label(
               /* IN     */ const UInt32_t   a429word
            );

UInt32_t    ioen_im4GetA429Sdi(
               /* IN     */ const UInt32_t   a429word
            );

void        ioen_im4ProcessA429Message(
                /* IN     */ const UInt32_t                    portIdx,
                /* IN     */ const UInt32_t                    a429Label
            );

void        ioen_updateMessageFreshness (
                /*        */ void
            );

void        ioen_ConfirmInitMsg (
    /* IN     */ const UInt32_t                      freshTime,
    /* IN     */ const UInt32_t                      unfreshTime,
    /* IN     */ const Float32_t                     appPeriodMs,
    /* IN OUT */       IomObjectCtrlData_t   * const ctrl_p
);

void        ioen_ConfirmInitParam (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         validTime,
    /* IN     */ const UInt32_t                         invalidTime,
    /* IN     */ const UInt32_t                         nofsources,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
);

void        ioen_ConfirmInitParamConfirmed (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         validTime,
    /* IN     */ const UInt32_t                         invalidTime,
    /* IN     */ const UInt32_t                         nofsources,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
);

void        ioen_ConfirmInitParamCan (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         validTime,
    /* IN     */ const UInt32_t                         invalidTime,
    /* IN     */ const UInt32_t                         nofParams,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_CAN_PARAM_PER_MESSAGE]
);

void        ioen_ConfirmInitParamSignal (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         nofsources,
    /* IN     */ const InputSignalConfig_t      * const sigConfig_p,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
);

void        ioen_ConfirmInitParamValidity (
    /* IN     */ const IOMConfigHeader_t        * const iom_p,
    /* IN     */ const Float32_t                        appPeriodMs,
    /* IN     */ const UInt32_t                         nofsources,
    /* IN     */ const ValidityConfig_t         * const validityLogic_p,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
);

void        ioen_ConfirmSetNewMsgData (
    /* IN OUT */       IomObjectCtrlData_t   * const ctrl_p
);

void        ioen_ConfirmObjectValidity (
    /* IN     */ const UInt32_t                         nofSources,
    /* IN OUT */       IomObjectCtrlData_t              ctrl[IOEN_MAX_SOURCES_PER_INPUT]
);

void        ioen_ConfirmObjectValidityItem (
    /* IN OUT */       IomObjectCtrlData_t      * const ctrl_p
);

void        ioen_ConfirmMsgFreshness (
    /* IN OUT */       IomObjectCtrlData_t   * const ctrl_p,
    /* IN OUT */       UInt32_t              * const value_p
);

void        ioen_AfdxInitSignalConfirmation (
    /* IN     */ const IOMConfigHeader_t   * const iom_p,
    /* IN     */ const Float32_t                   appPeriodMs
);

Bool_t      ioen_UtilCheckCrc (
   /* IN     */ const Byte_t             * const data_p,
   /* IN     */ const AfdxMessageInfo_t  * const msginfo_p
);

void        ioen_UtilSetCrc (
   /* IN OUT */       Byte_t             * const data_p,
   /* IN     */ const AfdxMessageInfo_t  * const msginfo_p
);

Bool_t      ioen_UtilCheckFc (
   /* IN     */ const Byte_t             * const data_p,
   /* IN     */ const AfdxMessageInfo_t  * const msginfo_p,
   /* IN OUT */       UInt16_t           * const fcPrev_p
);

void ioen_UtilSetFc (
   /* IN OUT */       Byte_t             * const data_p,
   /* IN     */ const AfdxMessageInfo_t  * const msginfo_p,
   /* IN OUT */       UInt16_t           * const fcPrev_p
);

Bool_t ioen_UtilIsZero (
   /* IN     */ const Byte_t             * const data_p,
   /* IN     */       UInt32_t                   nofBytes
);

#endif
