/***************************************************************
 * COPYRIGHT:
 *   2013-2015
 *   Chinese Aeronautical Radio Electronics Research Institute
 *   All Rights Reserved
 *
 * FILE NAME:
 *   ioen_InputMappingDio.c
 *
 * FILE DESCRIPTION:
 *   This module gets the IDU ID.
 *
***************************************************************/

#include "ioen_IomLocal.h"
#include <stdio.h>

#ifdef IOEN_PLATFORM_IDU
#include <string.h>
#include <stdlib.h>



#include <apex/apexLib.h>

#define SYSCALL_GET_IDU_ID 1

/******************************************************************
 * FUNCTION NAME:
 *   ioen_Get_Idu_Id
 *
 * DESCRIPTION:
 *   This function gets the IDU ID
 *
 * INTERFACE:
 *
 *   Return:  the IDU ID
 *
 ******************************************************************/
IDU_ID ioen_Get_Idu_Id(void)
{
    RETURN_CODE_TYPE    retCode;
    IDU_ID              id;
    UInt32_t            tmp;

    retCode = -1;
    vThreadsOsInvoke (SYSCALL_CUSTOM, &retCode, SYSCALL_GET_IDU_ID, &tmp, 0, 0, 0);

    if(tmp > 0 && tmp < 6)
        id = (IDU_ID)tmp;
    else
        id = INVALIDID;

    return id;
}
#endif

#ifdef IOEN_PLATFORM_ADS2

extern char *Global_Commandline_Options;

/******************************************************************
 * FUNCTION NAME:
 *   ioen_Get_Idu_Id
 *
 * DESCRIPTION:
 *   This function gets the IDU ID for the ADS2 platform
 *
 * INTERFACE:
 *
 *   Return:  the IDU ID
 *
 ******************************************************************/
IDU_ID ioen_Get_Idu_Id(void)
{
    IDU_ID id;

    if (strncmp(Global_Commandline_Options, "DUID=", 5) == 0)
    {
        id = atoi(&Global_Commandline_Options[5]);
        printf("DUID=%d\n", id);
    }

    return id;
}
#endif

#ifdef IOEN_PLATFORM_IMA
/******************************************************************
 * FUNCTION NAME:
 *   ioen_Get_Idu_Id
 *
 * DESCRIPTION:
 *   This function gets the IDU ID for the IMA platform
 *
 * INTERFACE:
 *
 *   Return:  the IDU ID
 *
 ******************************************************************/
IDU_ID ioen_Get_Idu_Id(void)
{
    return IDU_LO;
}
#endif
