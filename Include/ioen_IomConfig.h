/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_IomConfig.h
 *
 * DESCRIPTION:
 *   Types and defines for the configuration of the IO Manger
 *
 *
***************************************************************/



#ifndef IOMAppConfig_h_included
#define IOMAppConfig_h_included

#include "ioen_GeneralTypes.h"
#include "ioen_Platform.h"
#include "ioen_ByteSwapping.h"


#define IOEN_MAX_SOURCE_SELECTION_INDEX         (128 )         /* Maximum number of different source selection functions        */
#define IOEN_MAX_SOURCES_PER_INPUT              ( 12)          /* Maximum number of Sources Per Input.                          */
#define IOEN_MAX_CAN_PARAM_PER_MESSAGE          ( 32)          /* Maximum number of parameters Per CAN Input message.           */
#define IOEN_MAX_NUMBER_OF_SELECTION_SETS       (1024)         /* Maximum number of Selection sets used for source selection.   */
#define IOEN_MAX_RX_PORT_NUMBER                 (1024)         /* Maximum number of ports that can be managed by the IO Engine. */
#define IOEN_MAX_TX_PORT_NUMBER                 (1024)         /* Maximum number of ports that can be managed by the IO Engine. */

#define IOEN_MAX_CAN_MESSAGES_CONFIGURED        (64)           /* Must be checked by config generator tool */
#define IOEN_MAX_CAN_MESSAGES_PER_CYCLE         (64)           /* Maximum number of CAN messages that can be received from the APEX driver in one cycle */

#define IOEN_CRC32_TABLE_SIZE                   (256)          /* Number of elements in the 32 bit CRC lookup table */
#define IOEN_CRC32_INIT_VALUE                   (0xFFFFFFFF)   /* Seed value for 32 bit CRC                         */
#define IOEN_CRC32_FINISH_VALUE                 (0xFFFFFFFF)   /* Finish value for 32 bit CRC                       */

#define IOEN_SOURCE_ONE                         (0)
#define IOEN_SOURCE_LIC_PARAMETER               (1)
#define IOEN_SOURCE_HEALTH_SCORE                (2)
#define IOEN_OBJECT_VALID                       (3)


#define IOEN_CAN_PORT_TYPE_RX_A                 (0)
#define IOEN_CAN_PORT_TYPE_RX_B                 (1)
#define IOEN_CAN_PORT_TYPE_TX_A                 (2)
#define IOEN_CAN_PORT_TYPE_TX_B                 (3)
#define IOEN_CAN_PORT_TYPE_ROUTING_TX           (4)
#define IOEN_CAN_PORT_TYPE_TERMINATOR           (5)

#define IOEN_A429_MAX_PORTS                     (6)        /* The A429 HW only has 6 receive channels */
#define IOEN_A429_MAX_LABELS                    (256)      /* 8 bits are allowed for a Label Code     */
#define IOEN_A429_MAX_SDI                       (4)        /* 2 bits are allowed for a Label SDI      */
#define IOEN_A429_READS_PER_PORT_PER_CYCLE      (16)        /* One A429 Label per read                 */

/* Standard A429 LABEL defines */
#define IOEN_A429_LABEL_OFFSET                  (0)
#define IOEN_A429_LABEL_SIZE                    (8)
#define IOEN_A429_SDI_OFFSET                    (8)
#define IOEN_A429_SDI_SIZE                      (2)


/* Standard TRANSPORT defines */
#define IOEN_TRANSPORT_A664                     (0)
#define IOEN_TRANSPORT_A429                     (1)
#define IOEN_TRANSPORT_A825                     (2)

/*********************************************************/
/*    Output Mapping Configuration
 * Note: Input part needs to be renamed
 * *******************************************************/

#define IOEN_A429_SSM_BCD_NO_PLUS               (0x00000000)    /* A429 SSM for BCD NO Plus, North, East, Right, To, Above */
#define IOEN_A429_SSM_BCD_NO_MINUS              (0x60000000)    /* A429 SSM for BCD NO Minus, South, West, Left, From, Below */
#define IOEN_A429_SSM_BCD_FT                    (0x40000000)    /* A429 SSM for BCD Functional Test */
#define IOEN_A429_SSM_BCD_NCD                   (0x20000000)    /* A429 SSM for BCD No Computed Data */
#define IOEN_A429_SSM_BNR_FW                    (0x00000000)    /* A429 SSM for BNR FW */
#define IOEN_A429_SSM_BNR_NCD                   (0x20000000)    /* A429 SSM for BNR NCD */
#define IOEN_A429_SSM_BNR_FT                    (0x40000000)    /* A429 SSM for BNR FT */
#define IOEN_A429_SSM_BNR_NO                    (0x60000000)    /* A429 SSM for BNR NO */
#define IOEN_A429_SSM_DIS_FW                    (0x60000000)    /* A429 SSM for BNR FW */
#define IOEN_A429_SSM_DIS_NCD                   (0x20000000)    /* A429 SSM for BNR NCD */
#define IOEN_A429_SSM_DIS_FT                    (0x40000000)    /* A429 SSM for BNR FT */
#define IOEN_A429_SSM_DIS_NO                    (0x00000000)    /* A429 SSM for BNR NO */

#define IOEN_A429_SSM_CLEAR_MASK                (0x9FFFFFFF)    /* A429 SSM Clear Mask. */

#define IOEN_NO_VALID_SOURCE_FOUND              (0xFFFFFFFF) /*No valid Source could be found by Source Selection */

#define IOEN_MASK_LSB_IN_32BITS_WORD            (0x00000001) /*Mask used to obtain the LSB in a 32-bits word */

/* Lock state of a source in a set */
#define IOEN_SOURCE_HEALTH_NO_LOCK              (0)
#define IOEN_SOURCE_HEALTH_LOCK                 (1)
#define IOEN_SOURCE_HEALTH_LOCK_PERMANENT       (2)

/* Mode to check an LIC_PARAMETER value */
#define IOEN_SOURCE_PARAM_VALUE_ANY             (0)
#define IOEN_SOURCE_PARAM_VALUE_EXACT           (1)

/* Type of special function */
#define IOEN_SPECIAL_FUNCTION_ID_NONE           (0)
#define IOEN_SPECIAL_FUNCTION_ID_ALERT_ID       (1)
#define IOEN_SPECIAL_FUNCTION_ID_VALIDITY_PARAM (2)
#define IOEN_SPECIAL_FUNCTION_ID_BCD_MULT       (3)
#define IOEN_SPECIAL_FUNCTION_ID_DEFAULT        (4)


/* =================================================================================== */
/* Configuration Structure Definitions */
/* =================================================================================== */

/* some useful macros */
#define AFDX_INPUT_MSG_FIRST(IOM)             (AfdxMessageInfo_t      *)((void *)(IOM) + IOM->afdxInputMessageStart)
#define AFDX_OUTPUT_MSG_FIRST(IOM)            (AfdxMessageInfo_t      *)((void *)(IOM) + IOM->afdxOutputMessageStart)
#define AFDX_OUTPUT_DS_FIRST(IOM)             (OutputDatasetConfig_t  *)((void *)(IOM) + IOM->afdxOutputDatasetStart)

#define CAN_INPUT_MSGINFO_FIRST(IOM)          (CanMessageConfig_t  *)((void *)(IOM) + IOM->canInputMessageStart)
#define CAN_OUTPUT_MSGINFO_FIRST(IOM)         (CanMessageConfig_t  *)((void *)(IOM) + IOM->canOutputMessageStart)

#define CAN_INPUT_MSGINFO_SIZE(MSG)           (MSG->size)
#define CAN_INPUT_MSGINFO_NEXT(MSG)           (CanMessageConfig_t *)((void *)MSG + CAN_INPUT_MSGINFO_SIZE(MSG))


#define IOEN_PORT_NAME(h, p)                  (((char *)h) + h->stringTableStart + p->portNameOffset)

#define IOEN_PARAM_NAME(h, p)                 (((char *)h) + h->stringTableStart + p->parNameOffset)
#define IOEN_STATUS_NAME(h, p)                (((char *)h) + h->stringTableStart + p->valNameOffset)

#define IOEN_SIZEOF_PARAM_MAPPING_CONFIG(p)   (sizeof(ParamMappingConfig_t) + (p->numSources * sizeof(InputSignalConfig_t)))


#define IOEN_MSG_UNFRESH                        (0)  /* Message has not been received in the refresh time period */
#define IOEN_MSG_FRESH                          (1)  /* Message has been received in the refresh time period     */

#define IOEN_FAILED                             (0)  /* Test Result, eg. CRC test failed */
#define IOEN_PASSED                             (1)  /* Test Result, eg. CRC test passed */

/* Validity Status values used internally for failure confirmation */
#define IOEN_GLOB_DATA_INIT_VALUE               (0)  /* Invalid value, used for initialisation   */
#define IOEN_GLOB_DATA_VALID                    (1)  /* NORMAL OPERATION or FUNCTIONAL TEST      */
#define IOEN_GLOB_DATA_INVALID                  (2)  /* Eg. FS not Normal Operation, SSM invalid */
#define IOEN_GLOB_DATA_LOST                     (3)  /* UNFRESH, no data within refresh period   */

/* Validity Status values to Application */
#define IOEN_VALIDITY_NODATA                    (0)  /* NODATA     */
#define IOEN_VALIDITY_NCD                       (1)  /* NCD        */
#define IOEN_VALIDITY_TEST                      (2)  /* TEST       */
#define IOEN_VALIDITY_EMPTY                     (3)  /* EMPTY      */
#define IOEN_VALIDITY_NORMALOP                  (4)  /* NORMALOP   */
#define IOEN_VALIDITY_UNFRESH                   (5)  /* UNFRESH    */
#define IOEN_VALIDITY_OUTOFRANGE                (6)  /* OUTOFRANGE */


typedef enum
{
    IOEN_VALIDITY_CONDITION_TYPE_FRESHNESS        = 0,         /* Condition Type  1: Freshness */
    IOEN_VALIDITY_CONDITION_TYPE_A664_FS          = 1,         /* Condition Type  2: A664 Functional Status */
    IOEN_VALIDITY_CONDITION_TYPE_SSM_BNR          = 2,         /* Condition Type  3: SSM_BNR */
    IOEN_VALIDITY_CONDITION_TYPE_SSM_BCD          = 3,         /* Condition Type  4: SSM_BCD */
    IOEN_VALIDITY_CONDITION_TYPE_SSM_DIS          = 4,         /* Condition Type  5: SSM_DIS */
    IOEN_VALIDITY_CONDITION_TYPE_VALIDITY_VALUE   = 5,         /* Condition Type  6: signal == value */
    IOEN_VALIDITY_CONDITION_TYPE_RANGE_INT        = 6,         /* Condition Type  7: signed integer range check 32 bit */
    IOEN_VALIDITY_CONDITION_TYPE_RANGE_UINT       = 7,         /* Condition Type  8: unsigned integer range check 32 bit */
    IOEN_VALIDITY_CONDITION_TYPE_RANGE_FLOAT      = 8,         /* Condition Type  9: floating range check */
    IOEN_VALIDITY_CONDITION_TYPE_RANGE_FLOAT_BNR  = 9,         /* Condition Type 10: floating range check for A429 BNR */
    IOEN_VALIDITY_CONDITION_TYPE_FRESH_FS         = 10,        /* Condition Type 11: Freshness and A664 Functional Status */
    IOEN_VALIDITY_CONDITION_TYPE_FRESH_FS_SSM_BNR = 11,        /* Condition Type 11: Freshness and A664 Functional Status */
    IOEN_VALIDITY_CONDITION_TYPE_FRESH_FS_SSM_BCD = 12,        /* Condition Type 11: Freshness and A664 Functional Status */
    IOEN_VALIDITY_CONDITION_TYPE_FRESH_FS_SSM_DIS = 13,        /* Condition Type 11: Freshness and A664 Functional Status */

    IOEN_NUMBER_CONDITION_TYPES                   = 14          /* Number of conditions */
} IOEN_VALIDITY_CONDITION_E;

typedef enum
{
    IDU_LO         = 1,
    IDU_LI         = 2,
    IDU_RI         = 3,
    IDU_RO         = 4,
    IDU_CE         = 5,
    INVALIDID      = -1
} IDU_ID;

typedef enum
{
    IOEN_A664_FS_ND  = 0x00,     /*A664 No Data */
    IOEN_A664_FS_NO  = 0x03,     /*A664 Normal Operation */
    IOEN_A664_FS_FT  = 0x0C,     /*A664 Functional Test */
    IOEN_A664_FS_NCD = 0x30,     /*A664 Non-computed Data */
} IOEN_A664_FS_E;


typedef enum
{
    IOEN_INPUT_MAPPING_UINT32               = 0,        /* Read unsigned 32bits word */
    IOEN_INPUT_MAPPING_READ64BITS           = 1,        /* Read 64bits word */
    IOEN_INPUT_MAPPING_MULTIPLE_BYTES       = 2,        /* Read Multiple Bytes */
    IOEN_INPUT_MAPPING_A664_BOOLEAN32       = 3,        /* Read A664 32 bit Boolean */
    IOEN_INPUT_MAPPING_BITFIELD32           = 4,        /* A bitfield in a 32bit word */
    IOEN_INPUT_MAPPING_BITFIELD64           = 5,        /* A bitfield in a 64bit word */
    IOEN_INPUT_MAPPING_READ32BITS_INT2FLOAT = 6,        /* 32bits with int to float conversion */
    IOEN_INPUT_MAPPING_READ32BITS_FLOAT2INT = 7,        /* 32bits with float to int conversion */
    IOEN_INPUT_MAPPING_A429BNR              = 8,        /* A429 BNR conversion */
    IOEN_INPUT_MAPPING_A429UBNR             = 9,        /* A429 UBNR conversion */
    IOEN_INPUT_MAPPING_A429BCD              = 10,       /* A429 BCD conversion */
    IOEN_INPUT_MAPPING_A429UBCD             = 11,       /* A429 Unsigned BCD conversion */
    IOEN_INPUT_MAPPING_A429BNR_Float2Int    = 12,       /* A429 BNR with float to int conversion */
    IOEN_INPUT_MAPPING_A429UBNR_Float2nt    = 13,       /* A429 UBNR with float to int conversion */
    IOEN_INPUT_MAPPING_A429BCD_Float2Int    = 14,       /* A429 BCD with float to int conversion */
    IOEN_INPUT_MAPPING_A429UBCD_Float2Int   = 15,       /* A429 BCD with float to int conversion */
    IOEN_INPUT_MAPPING_INT8                 = 16,       /* Read 8bits with sign extension */
    IOEN_INPUT_MAPPING_INT16                = 17,       /* Read 16bits with sign extension */
    IOEN_INPUT_MAPPING_UINT8                = 18,       /* Read 8bits without sign extension */
    IOEN_INPUT_MAPPING_UINT16               = 19,       /* Read 16bit without sign extension */
    IOEN_INPUT_MAPPING_INT8_ADD             = 20,       /* Read 8bit int and add to parambuffer */
    IOEN_INPUT_MAPPING_A664STRING           = 21,       /* Read A664 String into C String */
    IOEN_INPUT_MAPPING_READ32FLOATS         = 22,       /* Read Single precision IEEE754 floats (with consistency check) */
    IOEN_INPUT_MAPPING_READ64FLOATS         = 23,       /* Read double precision IEEE754 floats (with consistency check) */
    IOEN_INPUT_MAPPING_INT32                = 24,       /* Read signed 32bits word */
    IOEN_INPUT_MAPPING_MESSAGE_UNFRESH      = 25,       /* Special function to set a bool to true, if its message is unfresh */
    IOEN_INPUT_MAPPING_A664_BOOLEAN8        = 26,       /* Read A825 8 bit Boolean */
    IOEN_INPUT_MAPPING_BITFIELD8            = 27,       /* A bitfield in a 8bit word */
    IOEN_INPUT_MAPPING_READ16BITS_INT2FLOAT = 28,       /* 16bits with int to float conversion */

    IOEN_NUMBER_INPUT_MAPPING_TYPES         = 29

} IOEN_INPUT_MAPPING_E;



typedef enum
{
    IOEN_OUTPUT_DS_TYPE_A664                   = 0,         /* Output DS Type 1: Pure A664 DS */
    IOEN_OUTPUT_DS_TYPE_EMBEDDED_A429          = 1,         /* Condition Type 2: A664 DS with Embedded A429 label */
} IOEN_OUTPUT_DS_E;

typedef enum
{
    IOEN_OUTPUT_SSM_TYPE_NONE     = 0,                     /* No SSM */
    IOEN_OUTPUT_SSM_TYPE_BNR      = 1,                     /* SSM Type BNR */
    IOEN_OUTPUT_SSM_TYPE_BCD      = 2,                     /* SSM Type BCD */
    IOEN_OUTPUT_SSM_TYPE_DIS      = 3,                     /* SSM Type DIS */
} IOEN_OUTPUT_SSM_E;


typedef enum
{
    IOEN_OUTPUT_MAPPING_WRITE8BITS              = 0,        /* Output Mapping Type: 8bits word */
    IOEN_OUTPUT_MAPPING_WRITE16BITS             = 1,        /* Output Mapping Type: 16bits word */
    IOEN_OUTPUT_MAPPING_WRITE32BITS             = 2,        /* Output Mapping Type: 32bits word */
    IOEN_OUTPUT_MAPPING_WRITE64BITS             = 3,        /* Output Mapping Type: 64bits word */
    IOEN_OUTPUT_MAPPING_MULTIPLE_BYTES          = 4,        /* Output Mapping Type: Multiple Bytes */
    IOEN_OUTPUT_MAPPING_A664_BOOLEAN            = 5,        /* Output Mapping Type: A664 Boolean */
    IOEN_OUTPUT_MAPPING_BITFIELD32              = 6,        /* Output Mapping Type: A664 Boolean */
    IOEN_OUTPUT_MAPPING_A429BNR_FLOAT           = 7,        /* A429 BNR conversion from Float */
    IOEN_OUTPUT_MAPPING_A429UBNR_FLOAT          = 8,        /* A429 UBNR conversion from Float */
    IOEN_OUTPUT_MAPPING_A429BNR_INTEGER         = 9,        /* A429 BNR conversion from Integer */
    IOEN_OUTPUT_MAPPING_A429UBNR_INTEGER        = 10,       /* A429 UBNR conversion from Integer */
    IOEN_OUTPUT_MAPPING_A429BCD_FLOAT           = 11,       /* A429 BCD conversion from Float */
    IOEN_OUTPUT_MAPPING_A429BCD_INTEGER         = 12,       /* A429 BCD conversion from Integer */
    IOEN_OUTPUT_MAPPING_VALIDITY_STATUS         = 13,       /* Special function Output validity status */

    IOEN_NUMBER_OUTPUT_MAPPING_TYPES            = 14,
} IOEN_OUTPUT_MAPPING_E;


/* IOM Validity type */
typedef struct Validity_t
{
    Byte_t spare;                         /* 32 bit alignment, not used                                                         */
    Byte_t selectedSource;                /* 1 to n, if source selection is active for this parameter, otherwise 0              */
    Byte_t value;                         /* Internal use: LOST, INVALID, VALID                                                 */
    Byte_t ifValue;                       /* External interface values: NODATA, NCD, TEST, EMPTY, NORMALOP, UNFRESH, OUTOFRANGE */
} Validity_t;


/* IOM Configuration Header */

typedef struct IOMConfigHeader_t
{
    UInt32_t magicNumber;                   /* C919DDCF */
    UInt32_t totalSize;                     /* size in bytes from Magic to CRC                      */

    UInt32_t afdxInputMessageStart;         /* start of afdx input message section                  */
    UInt32_t afdxInputMessageCount;         /* number of messages in configuration                  */

    UInt32_t afdxInputDatasetStart;         /* offset of first afdx input dataset (one or more inputs with the same FS) */
    UInt32_t afdxInputDatasetCount;         /* number of afdx input dataset                         */
    UInt32_t afdxInputDatasetMultiStart;    /* offset of first afdx input dataset with multiple sources (one or more inputs with the same FS) */
    UInt32_t afdxInputDatasetMultiCount;    /* number of afdx input dataset with multiple sources   */

    UInt32_t afdxOutputMessageStart;        /* start of afdx output message section                 */
    UInt32_t afdxOutputMessageCount;        /* number of afdx output messages in configuration      */
    UInt32_t afdxOutputDatasetStart;        /* offset of first afdx output dataset                  */
    UInt32_t afdxOutputDatasetCount;        /* number of afdx output datasets in configuration      */

    UInt32_t canInputMessageStart;          /* Offset of CAN input message section                  */
    UInt32_t canInputMessageCount;          /* number of CAN input messages in configuration        */
    UInt32_t canOutputMessageStart;         /* Offset of CAN output message section                 */
    UInt32_t canOutputMessageCount;         /* number of CAN output messages in configuration       */

    UInt32_t a429InputPortStart;            /* Offset of A429 Input Physical Port section           */
    UInt32_t a429InputPortCount;            /* number of A429 Input Physical Ports in configuration */
    UInt32_t a429InputMessageStart;         /* Offset of A429 input message (label) section         */
    UInt32_t a429InputMessageCount;         /* number of A429 input messages in configuration       */

    UInt32_t dioInputMessageStart;          /* Offset of DIO input message section                  */
    UInt32_t dioInputMessageCount;          /* number of DIO input messages in configuration        */
    UInt32_t dioOutputMessageStart;         /* Offset of DIO output message section                 */
    UInt32_t dioOutputMessageCount;         /* number of DIO output messages in configuration       */

    UInt32_t selectionSetStart;             /* offset of selection set table                        */
    UInt32_t selectionSetSize;              /* size of selection set table                          */

    UInt32_t stringTableStart;              /* offset of string table                               */
    UInt32_t stringTableSize;               /* size of string table                                 */
} IOMConfigHeader_t;



/* AFDX Message definition */
typedef struct AfdxMessageInfo_t
{
    UInt32_t messageId;                 /* Message ID from XML Configuration                                    */
    UInt32_t messageLength;             /* Length of message payload (what is read from port)                   */
    UInt32_t queueLength;               /* Queue length, 0 if sampling message                                  */
    UInt32_t refreshPeriod;             /* Sampling port freshness in ms                                        */
    UInt32_t validTime;                 /* Max duration, before a message is marked as fresh in ms              */
    UInt32_t invalidTime;               /* Max duration, before a message is marked as unfresh in ms            */
    UInt32_t messageHdrOffset;          /* Offset in message buffer                                             */
    UInt32_t portNameOffset;            /* Offset of port Name (or CVT name) into string table                  */
    UInt32_t crcFsbOffset;              /* Offset in message of Functional Status of expected CRC               */
    UInt32_t crcOffset;                 /* Offset in message of expected CRC                                    */
    UInt32_t fcFsbOffset;               /* Offset in message of Functional Status of expected Freshness Counter */
    UInt32_t fcOffset;                  /* Offset in message of expected Freshness Counter                      */
    UInt32_t schedOffset;               /* Start cycle for scheduling the first read and subsequent reads       */
    UInt32_t schedRate;                 /* Number of cycles before read is required                             */
} AfdxMessageInfo_t;

/* A429 Physical Port definition */
typedef struct A429PortInfo_t
{
    UInt32_t portId;                    /* Port ID from XML Configuration                      */
    UInt32_t messageLength;             /* Length of message payload (what is read from port)  */
    UInt32_t queueLength;               /* Queue length, 0 if sampling message                 */
    UInt32_t messageOffset;             /* Offset in message buffer of the start of data       */
    UInt32_t portNameOffset;            /* Offset of port Name (or CVT name) into string table */
} A429PortInfo_t;

/* A429 Message (Label) Configuration */
typedef struct A429MessageInfo_t
{
    Byte_t   code;                      /* Label Code in binary, eg. Label Code octal 271 = 0xB9 (binary) */
    Byte_t   sdi;                       /* Source Destination Identifier (SDI) 2 bit field range 0 to 3   */
    Byte_t   port;                      /* port index on which message is received                        */
    Byte_t   pad;                       /* alignment                                                      */
    UInt32_t validTime;                 /* Max duration, before a message is marked as fresh in ms        */
    UInt32_t invalidTime;               /* Max duration, before a message is marked as unfresh in ms      */
} A429MessageInfo_t;



/* CAN Message definition */
typedef struct CanMessageConfig_t
{
    UInt32_t messageId;                 /* Message ID from XML Configuration                             */
    UInt32_t canId;                     /* CAN Message ID                                                */
    UInt32_t freshTime;                 /* Max duration, before a message is marked as fresh in ms       */
    UInt32_t unfreshTime;               /* Max duration, before a message is marked as unfresh in ms     */
    UInt16_t size;                      /* Size of message structure plus size of corresponding mappings */
    Byte_t   messageLength;             /* Length of message payload 1 - 8 bytes                         */
    Byte_t   numMappings;               /* number of mappings                                            */
} CanMessageConfig_t;


/* Structure of CAN message as returned from port driver and ADS2 */
typedef struct CANMessage_t
{
    UInt32_t canId;     /* ID of CAN Message, includes Functional Status (FS) and Redundancy Channel Identifier (RCI) */
    UInt16_t nbytes;    /* Number of data types                                   */
    SInt16_t msgtype;   /* Reserved for test usage, set to 0 for normal operation */
    Byte_t   data[8];   /* Payload of CAN message                                 */

} CANMessage_t;

/* CAN Port Configuration */
typedef struct IOMCanRoutingPortTbl_t
{
    UInt32_t      canType;              /* CAN Port Type (0=Main Rx, 1=Main Tx, 2=Routing Rx, 3=Routing Tx)      */
    UInt32_t      canId;                /* CAN Message ID                                                        */
    Char_t        portName[32];         /* APEX Port name                                                        */
    APEX_INTEGER  queueLength;          /* APEX Port Queue Length                                                */
    APEX_INTEGER  apexCanBusId;         /* APEX Port ID for a CAN bus                                            */
    APEX_INTEGER  nofOutMsg;            /* Number of messages in the output queue (to forward to this APEX Port) */
    CANMessage_t  outMsg[IOEN_MAX_CAN_MESSAGES_PER_CYCLE];  /* List of CAN message to be routed to this port     */
} IOMCanRoutingPortTbl_t;

/* A429 Port Configuration */
typedef struct IOMA429RoutingPortTbl_t
{
    UInt32_t      Type;              /*  Port Type (0=Main Rx, 1=Main Tx, 2=Routing Rx, 3=Routing Tx)    */
    UInt32_t      portindex;                /* portindex in the config                                                     */
    Char_t        portName[32];         /* APEX Port name                                                      */
    APEX_INTEGER  queueLength;          /* APEX Port Queue Length                                              */
    APEX_INTEGER  apexA429portId;         /* APEX Port ID for a                                         */
    SInt32_t      transport_portindex;  /*sendport index in the table -1 means not transport*/
} IOMA429RoutingPortTbl_t;


/* Dataset Configuration Header. */
/* A dataset consits of one or more input parmeters that are associated with one functional status. */
/* The dataset has one or more sources */
typedef struct InputDatasetHeader_t
{
    UInt16_t numSources;                /* Number of Sources for param                                             */
    UInt16_t numParams;                 /* Number of parameters in dataset (associated with one functional status) */
    UInt32_t logicSize;                 /* Size of the logic configuration                                         */
    UInt32_t datasetSize;               /* Size of complete dataset configuration                                  */
} InputDatasetHeader_t;


/* Configuration of the Source Validity Check Condition of type: Configurable two-values */
typedef struct ValidityConditionConfig_t
{
    UInt16_t         msgIdx;            /* Index in message config of message which contains the validity              */
    UInt16_t         transport;         /* Source of data, 0 = AFDX, 1 = A429, 2 = CAN                                 */
    UInt32_t         offset;            /* Offset of the 32-bits field to be checked, in the message buffer (in bytes) */
    UInt32_t         offset2;           /* Extra offset for combining validation fields                                */
    UInt32_t         offset3;           /* Extra offset for combining validation fields                                */
    UInt32_t         access;            /* Size (in bytes) of the signal                                               */
    UInt32_t         sizeBits;          /* Size (in bits) of the signal in a 32 bit word                               */
    UInt32_t         offBits;           /* Offset (in bits) of the signal in a 32 bit word                             */
    Float32_t        lsbValue;          /* Resolution of one bit                                                       */
    DataConverter_t  minValue;          /* minimum value of the input parameter                                        */
    DataConverter_t  maxValue;          /* maximum value of the input parameter                                        */
} ValidityConditionConfig_t;



/* Configuration of the Validity Check per Source */
typedef struct ValidityConfig_t
{
    UInt16_t                    numConditions;      /* Number of Validity Check conditions. Range [1..4].             */
    UInt16_t                    sourceSet;          /* corresponding set to which this source belongs to              */
    Byte_t                      conditionType[4];   /* Validity Check conditions                                      */
    ValidityConditionConfig_t   condition[4];       /* Array of Validity Check Condition                              */
} ValidityConfig_t;


/* Configuration of the Validity Check or CAN */
typedef struct ValidityConfigCan_t
{
    UInt16_t                    numConditions;      /* Number of Validity Check conditions. Range [0..2].             */
    UInt16_t                    spare;              /* 32 bit alignment                                               */
    Byte_t                      conditionType[4];   /* Validity Check conditions, only two used                       */
    ValidityConditionConfig_t   condition[2];       /* Array of Validity Check Condition                              */
} ValidityConfigCan_t;


/* Configuration of a Parameter Mapping */
typedef struct ParamMappingConfig_t
{
    UInt32_t        parOffset;                 /* Offset of the Parameter in the Input Parameter Buffer                  */
    UInt32_t        valOffset;                 /* Offset of the Parameter Validity in the Input Parameter Buffer         */
    UInt32_t        parSize;                   /* Size in bytes of parameter data                                        */
    UInt16_t        numSources;                /* Number of possible sources for the selection of the parameter          */
    UInt16_t        padding;
    UInt32_t        parNameOffset;             /* offset into string table of status parameter name                      */
    UInt32_t        valNameOffset;             /* offset into string table of status parameter name                      */
    DataConverter_t defaultValue;              /* default value to be used when data is invalid, set to zero if not used */

} ParamMappingConfig_t;


/* Input Signal Configuration */
typedef struct InputSignalConfig_t
{
    UInt16_t         msgIdx;                   /* Index in message config of message which contains the input signal    */
    UInt16_t         transport;                /* Source of data, 0 = AFDX, 1 = A429, 2 = CAN                           */
    UInt32_t         offByte;                  /* Offset (in bytes) of the input signal                                 */
    UInt32_t         sizeBits;                 /* Size (in bits) of the input signal                                    */
    UInt16_t         offBits;                  /* Offset (in bits) of the input signal                                  */
    UInt16_t         type;                     /* Type of reading and conversion applied to the signal                  */
    Float32_t        lsbValue;                 /* Resolution of one bit                                                 */
    UInt32_t         parOffset;                /* Offset of the Parameter in the Input Parameter source Buffer          */
    UInt32_t         valOffset;                /* Offset of the Parameter Validity in the Input Parameter source Buffer */
} InputSignalConfig_t;


typedef struct simpleMapConfig_t
{
    ParamMappingConfig_t parconfig;
    InputSignalConfig_t  sigconfig;
} simpleMapConfig_t;

typedef struct simpleMapConfigCanIn_t
{
    ParamMappingConfig_t parconfig;
    InputSignalConfig_t  sigconfig;
    ValidityConfigCan_t  validity;
} simpleMapConfigCanIn_t;

typedef struct simpleMapConfigCanOut_t
{
    ParamMappingConfig_t parconfig;
    InputSignalConfig_t  sigconfig;
    InputSignalConfig_t  sigconfigValidity;
} simpleMapConfigCanOut_t;




/************************************************************************/
/* Source Table Configuration                                           */

/* Single Source Configuration */
typedef struct LicParamConfig_t
{
    UInt32_t        valueMode;   /* Mode of source value: Exact value, any value                                            */
    UInt32_t        valueExp;    /* expected value of LIC_PARAMETER                                                         */
    UInt32_t        valOffset;   /* Offset of the Parameter Validity in the Input Parameter Buffer                          */
    UInt32_t        parOffset;   /* Offset of the Parameter in the Input Parameter Buffer                                   */
    UInt32_t        parType;     /* Type of the Parameter in the Input Parameter Buffer, eg IOEN_INPUT_MAPPING_A664_BOOLEAN */
} LicParamConfig_t;


/* Set Configuration  */
typedef struct SelectionSetConfig_t
{
    UInt32_t nofSources;               /* Number of sources in a source set                                                                 */
    UInt32_t criteria;                 /* LIC_PARAMETER, OBJECT_VALID or SOURCE_HEALTH_SCORE                                                */
    UInt32_t sourceHealthMode;         /* Source Health Mode: No Lock, Lock Time, Permanent Lock                                            */
    UInt32_t sourceHealthValue;        /* lock time in ms                                                                                   */
    UInt32_t sourceOffset;             /* Offset (in words) of the first source (eg.LicParamConfig_t or ValidityConfig_t) in this set       */
    UInt32_t setConfigSize;            /* Total words of the source list for this set (eg. SelectionSetConfig_t + (n * LicParamConfig_t))   */
} SelectionSetConfig_t;

/* Selection Set List Configuration.                    */
/* The highest Priority source is the first in the list */
/* The lowest Priority source is the last in the list   */
typedef struct SelectionSetlistConfig_t
{
    UInt32_t nofSets;                  /* Total number of sets                                                                */
    UInt32_t setListOffset;            /* Offset (in words) to the first set (SelectionSetConfig_t) of the list in the config */
} SelectionSetlistConfig_t;

/* End Source Table Configuration                                       */
/************************************************************************/







/* IOM Output Embedded A429 Configuration */
typedef struct OutA429LabelConfig_t
{
    UInt32_t labelIdSdi;     /* Label ID and SDI, already in their corresponding positions */
    UInt32_t SSMType;        /* SSM Type                                                   */
} OutA429LabelConfig_t;


/* IOM Output Dataset Configuration */
typedef struct OutputDatasetConfig_t
{
    UInt32_t                 type;            /* Dataset Type                                          */
    UInt32_t                 size;            /* Size of the DS Configuration                          */
    UInt32_t                 FSOffset;        /* Offset of the Functional Status in the Message Buffer */
    UInt32_t                 DSOffset;        /* Offset of the Dataset in the Message Buffer           */
    UInt32_t                 numMappings;     /* Number of parameter mappings                          */
    OutA429LabelConfig_t     a429w;           /* a429 word specific data                               */
} OutputDatasetConfig_t;





#endif
