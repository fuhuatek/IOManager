# IOManager
How to use the functions of this library
========================================


before Process Create
---------------------
	ioen_createPorts(IOMConfigHeader_t *iom_p): Create all AFDX Ports
	
	/* Only if application has CAN Input, e.g. DMH */
	ioen_createCanPorts(IOMConfigHeader_t *iom_p): Create all AFDX Ports
	
At start of process, before entering main loop
----------------------------------------------

	ioen_initSourceSelection(Byte_t *messageBuffer_p)
	ioen_initializeIOM(Byte_t *messageBuffer_p, Byte_t *paramBuffer_p)	

	/* Only if the application has CAN Input (Currently only DMH), 
	   otherwise the functions but should not be called */
	ioen_initializeCanInput(Byte_t *paramBuffer_p, IOMConfigHeader_t *iom_p)

once per main loop
----------------------------------------------

	ioen_readMessages(IOMConfigHeader_t *iom_p, char *inMsgBuffer)			: Read all AFDX messages into buffer
	ioen_processIOM(IOMConfigHeader_t *iom_p)								: Process all AFDX messages and copy parameters to application 
	
	/* Only if the application has CAN Input (Currently only DMH), 
	   otherwise the functions but should not be called */
	ioen_startCanInput()  													: Prepare processing the CAN message (reset counters)
	ioen_readCanMessages()  												: Read and process all can messages
	
	/* Only if the application has AFDX Output, otherwise the functions will do nothing, 
	   but should not be called */

	ioen_processAfdxOutput(...) : Prepare all AFDX output messages
	ioen_sendAfdxMessages(...)	: Ok
