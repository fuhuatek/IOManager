
#include "ioen_IOMAppConfig.h"
#include <stdio.h>

#ifdef IOEN_PLATFORM_IDU
#include <string.h>
#include <stdlib.h>


#include <apex/apexLib.h>

#define SYSCALL_GET_IDU_ID 1

IDU_ID ioen_Get_Idu_Id()
{
	RETURN_CODE_TYPE retCode = -1;
	int id;
	int tmp;
	vThreadsOsInvoke (SYSCALL_CUSTOM, &retCode, SYSCALL_GET_IDU_ID, &id, 0, 0, 0);

	tmp = (id & 0x00000E00) >> 9 ;
	
  switch(tmp)
  {
  	case 1:
  		id = IDU_RO;
  	  break;
  	case 2:
  		id = IDU_LI;
  	  break;
  	case 4:
  		id = IDU_LO;
  	  break;
  	case 5:
  		id = IDU_CE;
  	  break;
  	case 6:
  		id = IDU_RI;
  	  break;
  	default:
  		id = INVALIDID;
  		break;  	
  }
	
	return id;
}
#endif

#ifdef IOEN_PLATFORM_ADS2

extern char *Global_Commandline_Options;

IDU_ID ioen_Get_Idu_Id()
{
    int id;
    if (strncmp(Global_Commandline_Options, "DUID=", 5) == 0)
    {
        id = atoi(&Global_Commandline_Options[5]);
        printf("DUID=%d\n", id);
    }

    return id;
}
#endif

#ifdef IOEN_PLATFORM_IMA
IDU_ID ioen_Get_Idu_Id()
{
	return IDU_LO;
}
#endif
