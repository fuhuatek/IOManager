/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_IomLocalCan.h
 *
 * DESCRIPTION:
 *   Functions, Types and defines used locally within the IOM CAN interface
 *
 *
***************************************************************/

#ifndef ioen_IoEngineLocalCan_h_included
#define ioen_IoEngineLocalCan_h_included


#include "ioen_IomLocal.h"



#define IOEN_MAX_CAN_PORT_NUMBER        ( 2)
#define IOEN_CAN_PORT_QUEUE_LEN         ( 1)

#define A825_CAN_ID_IGNORE_FS_MASK      (0x1FFFFFF3) /* mask unused top 3 bits, FS                                   (bits 0 and 1 = RCI, bits 2 and 3 = FS) */
#define A825_CAN_ID_IGNORE_RCI_MASK     (0x1FFFFFFC) /* mask unused top 3 bits, Redundancy Channel Identifier        (bits 0 and 1 = RCI, bits 2 and 3 = FS) */
#define A825_CAN_ID_IGNORE_FS_RCI_MASK  (0x1FFFFFF0) /* mask unused top 3 bits, FS and Redundancy Channel Identifier (bits 0 and 1 = RCI, bits 2 and 3 = FS) */
#define A825_CAN_ID_RCI_MASK            (0x00000003) /* bits 0 and 1 = RCI */
#define A825_CAN_ID_FS_MASK             (0x0000000C) /* bits 2 and 3 = FS  */
#define A825_CAN_ID_FS_SHIFT            (2)          /* bits 2 and 3 = FS  */
#define A825_CAN_ID_INVALID             (0xFFFFFFFF) /* invalid CAN ID     */

#define IOEN_CAN_ND                     ( 0)
#define IOEN_CAN_NO                     ( 1)
#define IOEN_CAN_FT                     ( 2)
#define IOEN_CAN_NCD                    ( 3)




/*
 * Control structure for can messages.
 * Used to handle bus channel switching according to ICD: CDS
 */
typedef struct CANMessageControl_t
{
    UInt32_t                   canIdMasked;
    UInt32_t                   lastbus;       /* Stores the APEX ID returned by CREATE_QUEUING_PORT range 1..number of ports */
    const CanMessageConfig_t  *msgConfigSource1_p;  /* RCI = 0 */
    const CanMessageConfig_t  *msgConfigSource2_p;  /* RCI = 1 */
    IomObjectCtrlData_t        msgValidCtrl;
    CANMessage_t               prevMsg;       /* Used to store the previous message for filtering purposes */
} CANMessageControl_t;



/*
 * These pointers are initialized in ioen_initializeIOM (ioen_icInitInputConverter). They are assumed to
 * to the global message buffer and global parameter buffer.
 */

extern       CANMessageControl_t      ioen_mrcCanMessageControl[IOEN_MAX_CAN_MESSAGES_CONFIGURED];
extern       APEX_INTEGER             ioen_apexCanBusIdRxA;
extern       APEX_INTEGER             ioen_apexCanBusIdRxB;
extern       APEX_INTEGER             ioen_apexCanBusIdTxA;
extern       APEX_INTEGER             ioen_apexCanBusIdTxB;
extern       UInt32_t                 ioen_nofCanMessages;
extern       IOMCanRoutingPortTbl_t * ioen_mrcConfigRoutingTable_p;

void        ioen_imcDoOneInputMapping (
                /* IN     */ const Validity_t                     validity,
                /* IN     */ const simpleMapConfigCanIn_t * const mapConfig_p,
                /* IN     */ const CANMessage_t           * const msg_p
            );


void        ioen_imcLookupMessage (
               /* IN     */ const UInt32_t                       canId,
               /*    OUT */ const CanMessageConfig_t          ** msgConfig_p,
               /*    OUT */       CANMessageControl_t         ** msgCtrl_p
            );

void        ioen_imcCheckMessageFreshness (
               /* IN     */ void
            );

void        ioen_imcHandleBus (
               /* IN     */ const SInt32_t   portId
            );

void        ioen_imcProcessCanMessage (
                /* IN OUT */       CANMessageControl_t    * const msgctrl_p,
                /* IN     */ const CanMessageConfig_t     * const msgConfig_p,
                /* IN     */ const CANMessage_t           * const msg_p
            );

void        ioen_imcInvalidateCanMessage (
                /* IN OUT */       CANMessageControl_t    * const msgctrl_p
            );

void        ioen_omcProcessOutputParametersCan (
                /* IN     */ const CanMessageConfig_t     * const msgCfg_p,
                /* IN     */ const Byte_t                 * const outParamBuffer,
                /* IN     */       Byte_t                 * const outMsgBuffer
            );


#endif
