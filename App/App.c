/*
 * App.c
 */


/* includes */
#ifndef PLATFORM_ADS2APEX
#include "copyright_wrs.h"
#include "vxWorks.h"
#include "taskLib.h"
#endif
#include "apex/apexLib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "LibError.h"


/* Entry points to Platform Independent Synoptic Application Main */
extern void applicationStep();
extern void applicationCreatePorts(int);
extern void applicationInit();


/* defines */ 
#define TIME_250mSec 250000000ll
#define TIME_16mSec5  16500000ll
#define TIME_1Sec 1000000000ll
#define SOFT 0
#define STACK_SIZE   8000
#define BASE_PRIORITY 100
#define INFINITE INFINITE_TIME_VALUE

//FIXME
#define FOREVER while(1)
#define NELEMENTS(x) (sizeof(x)/sizeof(x[1]))



/* Functions prototypes */
void applicationProcess(void);

/* globals variables */
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
    /* NAME                    ENTRY_POINT             STACK_SIZE    BASE_PRIORITY    PERIOD             TIME_CAPACITY    DEADLINE */
    {"applicationProcess",     applicationProcess,     STACK_SIZE,   BASE_PRIORITY,   TIME_16mSec5,      INFINITE,        SOFT},
};


void applicationProcess(void)
{
    RETURN_CODE_TYPE retCode = -1;
    
    applicationInit();
    
    FOREVER
    {
        /* Wait next period expiration */
        PERIODIC_WAIT (&retCode);
        CHECK_CODE ("PERIODIC_WAIT", retCode);  
        
        applicationStep();
    }
}


/* Init routine to set up AppIORecv application */
void InitApp(void)
{
    int i;
    RETURN_CODE_TYPE retCode = -1;
    int proc_id;

    CREATE_ERROR_HANDLER(
        (SYSTEM_ADDRESS_TYPE)DefaultErrorHandler,  /* in : Error handler entry point */
        (STACK_SIZE_TYPE)0x4000,                   /* in : Error handler stack size  */
        &retCode                                   /* out: Return code               */
        );
        
    if (retCode != NO_ERROR)
    {
        printf("CLOCK:Create UserError Handler failure(%s)\n",
        codeToStr(retCode));
    }
    
//    applicationCreatePorts(TIME_16mSec5);
             
     /* Create all Application processes and start them*/
    for (i = 0; i < NELEMENTS(processTable); i++) {
        CREATE_PROCESS (
            &processTable[i],  /* process attribute */
            (PROCESS_ID_TYPE *)&proc_id,    /* process Id */
            &retCode);
    
        if (retCode != NO_ERROR)
            printf ("CLOCK:CREATE_PROCESS: can't create process %s %s)\n", 
                processTable[i].NAME, codeToStr(retCode));
            
        START (proc_id, &retCode);
        
        if (retCode != NO_ERROR)
            printf("CLOCK:START: can't start process %s (%s)\n",
                processTable[i].NAME, codeToStr(retCode));
        
    }
      
    /**************************************************/
    /* enter NORMAL partition mode to begin operation */
    /**************************************************/
    SET_PARTITION_MODE (NORMAL, &retCode);
    CHECK_CODE ("SET_PARTITION_MODE", retCode);   
  
    return;
}
