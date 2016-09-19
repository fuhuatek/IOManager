/* Main module for IOManager */

#include <stdio.h>
#include <string.h>

#ifndef PLATFORM_ADS2APEX
#include <vxworks.h>
#endif

#include "sdRgnLib.h"
#include "apex/apexLib.h"
#include "apex/APEX_Partition.h"
#include "ioen_IOMAppConfig.h"
#include "ioen_Platform.h"
#include "LibError.h"

#ifdef STATIC_DU_CONFIG
#include "iom_staticConfiguration.h"
#endif

extern Byte_t iomConfig_HF_IDUCENTER[];                                 /* Stores the Configuration to perform IO Map */
extern Byte_t iomConfig_HF_IDULEFTINBOARD[];                            /* Stores the Configuration to perform IO Map */
extern Byte_t iomConfig_HF_IDURIGHTINBOARD[];                           /* Stores the Configuration to perform IO Map */
extern Byte_t iomConfig_HF_IDULEFTOUTBOARD[];                           /* Stores the Configuration to perform IO Map */
extern Byte_t iomConfig_HF_IDURIGHTOUTBOARD[];                          /* Stores the Configuration to perform IO Map */

static IOMConfigHeader_t * IOMconfig = 0;

/*Note: the following value are only intended for prototyping the engine. Actual values need to be defined in the production SW. */
#define IOEN_MAX_INPUT_PARAMETER_BUFFER_SIZE (8*1024)         /* Maximum size of the Input Parameter Buffer (in Bytes) */
#define IOEN_MAX_INPUT_MESSAGE_BUFFER_SIZE (128*1024)         /* Maximum size of the Input Message Buffer (in Bytes) */


/* Compliant with ICD411 */
static IOMCanRoutingPortTbl_t canRoutingTable[] = {{0x8482004,"apexport_PFD_FROM_A825_A",0},
                                                   {0x8482005,"apexport_PFD_FROM_A825_B",0},
                                                   {0x8482404,"apexport_PFD_FROM_A825_A",0},
                                                   {0x8482405,"apexport_PFD_FROM_A825_B",0},
                                                   {-1       ,""                      ,-1}};

/*
 * Set IOM Config according to DUID
 */   
static void getIOMConfig()
{
    /* default configuration */
    IOMconfig = (IOMConfigHeader_t *)&(iomConfig_HF_IDUCENTER[0]);
    
    #ifndef STATIC_DU_CONFIG
    int duid = (int)ioen_Get_Idu_Id();
    switch(duid)
    {
        case IDU_LO:
            IOMconfig = (IOMConfigHeader_t *)&(iomConfig_HF_IDULEFTOUTBOARD[0]);
            break;
        case IDU_LI:
            IOMconfig = (IOMConfigHeader_t *)&(iomConfig_HF_IDULEFTINBOARD[0]);
            break;
        case IDU_RI:
            IOMconfig = (IOMConfigHeader_t *)&(iomConfig_HF_IDURIGHTINBOARD[0]);
            break;
        case IDU_RO:
            IOMconfig = (IOMConfigHeader_t *)&(iomConfig_HF_IDURIGHTOUTBOARD[0]);
            break;
        case IDU_CE:
            IOMconfig = (IOMConfigHeader_t *)&(iomConfig_HF_IDUCENTER[0]);
            break;
        default:
            IOMconfig = (IOMConfigHeader_t *)&(iomConfig_HF_IDULEFTOUTBOARD[0]);
    }
    #else
    IOMconfig = IOM_STATIC_CONFIG;
    #endif
}


Byte_t *paramOutputBuffer = 0;
Byte_t inMsgBuffer[IOEN_MAX_INPUT_MESSAGE_BUFFER_SIZE];      /* Input Message Buffer */

/*
 * Depending on personality (left/right) we may need to read different config files because
 * Side has effect on priority of source selection
 */
void applicationCreatePorts(int appPeriodMs)
{
    getIOMConfig();
    ioen_createPorts((IOMConfigHeader_t *)IOMconfig);
	if (strcmp(APPNAME, "PFDIOM") == 0) 
	{
		ioen_createCanPorts((IOMConfigHeader_t *)IOMconfig,canRoutingTable,FALSE);    
	}
}

void applicationStep()
{
    /* TODO
     * if IOMInputConfig
     *     ioen_readMessages()
     *	   ioen_processInputMappings()
     * if IOMOutputConfig
     *	   ioen_processOutputMappings()
     *     ioen_writeMessages()
     */


    /* read all message into inMsgBuffer */
    ioen_readMessages((IOMConfigHeader_t *)IOMconfig, (char*)inMsgBuffer);

	if (strcmp(APPNAME, "PFDIOM") == 0) 
	{
		ioen_startCanInput();
		ioen_readCanMessages();
	}

    ioen_processIOM((IOMConfigHeader_t *)IOMconfig); 

}

void applicationInit()
{    
    STATUS status;
    SD_RGN_INFO paramOutputMem;
    char   shmem[128] = SHMNAME;
    char   App[128];
    int    i;
    
    PRINT("Initializing %s\n", APPNAME);
    PRINT("Initializing shm %s\n", shmem);

    status = sdRgnInfoGet(shmem, &paramOutputMem);
    if(status != OK )
    {
        PRINT("sdRgnInfoGet for %sInParamMem is failed/n", APPNAME);
    }
    paramOutputBuffer = (unsigned char *)(paramOutputMem.virtualAdrs);
    PRINT("(%s) paramOutputBuffer %p\n", APPNAME, paramOutputBuffer);
    memset((void*)paramOutputBuffer, 0x00, paramOutputMem.size);    
    
    memset((void*)paramOutputBuffer , 0, IOEN_MAX_INPUT_PARAMETER_BUFFER_SIZE);
    memset((void*)inMsgBuffer       , 0, IOEN_MAX_INPUT_MESSAGE_BUFFER_SIZE);
    
    /* initialize source selection and io mapper */
    ioen_initSourceSelection(inMsgBuffer);
    ioen_initializeIOM(inMsgBuffer, paramOutputBuffer);
    if (strcmp(APPNAME, "PFDIOM") == 0) 
    {
        ioen_initializeCanInput(paramOutputBuffer, (IOMConfigHeader_t *)IOMconfig);
    }
    
    PRINT("In io%sProcess\n", APPNAME);
}


