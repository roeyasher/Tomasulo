#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

void IntilaizeMemoryArray(){
	int i=0,j=0;
	for(i=0;i<MEMORY_SIZE;i++)
		for (j = 0; j<BUFFER_SIZE; j++)
			MainMemoryArray[i][j] = '0';
}

LoadBuffer *CreateLBNewNode(){

	/*this function create a new node for a linked list, for the load buffer*/
	LoadBuffer *temp = NULL;
	temp = (LoadBuffer*) malloc(sizeof(LoadBuffer));
	memset(temp, 0, sizeof(LoadBuffer));
	temp->next = NULL;
	return temp;		/*NULL is returned if failure occured*/
}

StoreBuffer *CreateNewSBNode(){

	/*this function create a new node for a linked list for the store buffer*/
	StoreBuffer *temp = NULL;
	temp = (StoreBuffer*) malloc(sizeof(StoreBuffer));
	memset(temp, 0, sizeof(StoreBuffer));
	temp->next = NULL;
	return temp;		/*NULL is returned if failure occured*/
}

Memory_PiplineStage *CreateNewMPLNode(){

	/*this function create a new node for a linked list for the store buffer*/
	Memory_PiplineStage *temp = NULL;
	temp = (Memory_PiplineStage*) malloc(sizeof(Memory_PiplineStage));
	memset(temp, 0, sizeof(Memory_PiplineStage));
	temp->next = NULL;
	temp->OPCODE=-1;
	return temp;		/*NULL is returned if failure occured*/
}

void IntilaizeLoadBuffer(){

	/*this function intilaize the load buffer/reservation station*/
	int Number_of_MemReservation_station = Configuration->mem_nr_load_buffers;
	int i=0;

	LoadBuffer *node = NULL;
	LoadBufferResarvation = CreateLBNewNode();
	node = LoadBufferResarvation;

	for (i=1;i<Number_of_MemReservation_station;i++){
		node->next = CreateLBNewNode();
		node = node->next;
	}

	LD_Buff_Cnt = 0;
}

void IntilaizeStoreBuffer()

{
	/*this function intilaize the store buffer/reservation station*/
	int Number_of_MemReservation_station = Configuration->mem_nr_store_buffers;
	int i=0;

	StoreBuffer *node = NULL;
	StoreBufferResarvation = CreateNewSBNode();
	node = StoreBufferResarvation;

	for(i=1;i<Number_of_MemReservation_station;i++){
		node->next = CreateNewSBNode();
		node = node->next;
	}

	ST_Buff_Cnt = 0;
}

void IntializeMemPipline(){

	/*this function intilaize the execute memory unit*/
	int length = Configuration->mem_delay;
	int i=0;

	Memory_PiplineStage *node = NULL;
	Memory_Unit = CreateNewMPLNode();
	node = Memory_Unit;

	for(i=1;i<length;i++){
		node->next = CreateNewMPLNode();
		node = node->next;
	}
}

BOOL InsertNewLoadInstr(){	

	LoadBuffer *available = NULL, *iter = LoadBufferResarvation;
	int length = Configuration->mem_nr_load_buffers;
	int i=0;
	int address=0;

	while(iter != NULL){

		/*if not busy , assign the current place as available*/
		if(iter->busy == FALSE){
			available = iter;
			break;
		}
		iter = iter->next;
	}

	/*if available is still NULL then there is no empty place and return false */
	if (available == NULL)
		return FALSE;

	/*but we need also to update that the resarvations are full and we cant continue the program*/
	/* now we need to calculate the right address, if its not possible we also stuck*/

	/*for load instruction checking the opcode*/
	available->OPCODE = LD;

	/*indicate that it is ok and the instruction was insert into the relevant reservation station*/
	available->busy = TRUE;
	available->inExecution = FALSE;
	available->robNum = instr.numRob;
	available->issued = cycle;

	/*insert the label to the destination register*/
	FP_Registers[instr.DST].busy = TRUE;
	FP_Registers[instr.DST].robNum = instr.numRob;

	LD_Buff_Cnt++;
	return TRUE;
	
}

BOOL InsertNewStoreInstr(){

	StoreBuffer *available = NULL , *iter = StoreBufferResarvation;
	int length = Configuration->mem_nr_store_buffers;
	int i=0;
	int address = 0;
	/*fine available place in the resarvation station for load */

	if (instr.OPCODE != ST)
		return FALSE;

	while(iter != NULL){
		/*if not busy , assign the current place as available*/
		if(iter->busy == FALSE){
			available = iter;
			break;
		}
		iter = iter->next;
	}

	/*if available is still NULL then there is no empty place and return false */
	if (available == NULL)
		return FALSE;

	/*but we need also to update that the resarvations are full and we cant continue the program*/
	/* now we need to calculate the right address, if its not possible we also stuck*/

	/*for store instruction - checking the opcode */
	available->OPCODE = ST;

	/*insert the FP value it is exist*/

	if(FP_Registers[instr.SRC1].busy == FALSE){

		available->Vj=FP_Registers[instr.SRC1].value;
		available->NumOfRightOperands++;
	}
	else{
		/*else waiting for the instruction to finish - by waiting to the instruction label*/
		available->Qj = FP_Registers[instr.SRC1].robNum;
	}

	/*insert the new instruction to the store buffer/reservation station and update the relevant fields*/

	available->busy = TRUE;
	available->inExecution = FALSE;
	available->robNum = instr.numRob;
	available->issued=cycle;

	trace[cycle].issued=cycle;
	trace[cycle].valid=TRUE;
	strcpy(trace[cycle].instruction,instr.name);

	ST_Buff_Cnt++;
	return TRUE;
}

//TODO check dependace adress between load after store!!!!!!!!!! (Roey)
BOOL IsNotTheSameAddress(int address, int robNum,int count){

	/*the function is checking if the instr we want to execute is in the right order and can be execute*/
	StoreBuffer *store = StoreBufferResarvation;
	int length = Configuration->mem_nr_store_buffers;
	int i=0;
	for(i=0;i<length;i++)
	{
		if ((address == store->address) && (store->robNum == robNum) && (store->inExecution == FALSE) && (store->busy == TRUE)){
			return FALSE;
		}
		store=store->next;
	}
	return TRUE;
}

BOOL LDBufferToMemoryExecution(){

	/*this is the main function witch responsable on the execute instr*/
	/*the function try to find a possible instr to execute by using some conditions and if it is ok it do so, else it is insert a nop to execute*/
	int j = 0;
	LoadBuffer *load = LoadBufferResarvation;

	/* the logic for the pre execute level*/
	while (load != NULL)
	{
		/*if the minstore is null and the minload is not we can execute minload instr*/
		if ((load->addressReady) && (load->inExecution == FALSE)){

			load->inExecution = TRUE;
			Memory_Unit->OPCODE = load->OPCODE;
			Memory_Unit->address = load->address;
			Memory_Unit->numOfRobSupplier = load->robNum;
			Memory_Unit->issued = load->issued;
			load->done = TRUE;
			break;
			// Write on the FP register
			//for (j = 0; j < TRACE_SIZE; j++){
			//	if (trace[j].issued == load->issued)
			//		break;
			//}
			//trace[j].execution = cycle;

			
			/*can execute and break the for a load instruction*/
		}

		load = load->next;
	}
}

void STBufferToMemoryExecution(){

	/*this is the main function witch responsable on the execute instr*/
	/*the function try to find a possible instr to execute by using some conditions and if it is ok it do so, else it is insert a nop to execute*/
	int j = 0;
	StoreBuffer *store = StoreBufferResarvation;

	/* the logic for the pre execute level*/
	while (store != NULL)
	{
		/*if the minstore is null and the minload is not we can execute minload instr*/
		if ((store->addressReady) && (store->inExecution == FALSE) && (store->NumOfRightOperands == 1)){

			store->inExecution = TRUE;
			Memory_Unit->OPCODE = store->OPCODE;
			Memory_Unit->address = store->address;
			Memory_Unit->numOfRobSupplier = store->robNum;
			Memory_Unit->issued = store->issued;
			store->done = TRUE;
			break;

			//for (j = 0; j < TRACE_SIZE; j++){
			//	if (trace[j].issued == store->issued)
			//		break;
			//}
			//trace[j].execution = cycle;

			/*can execute and break the for a load instruction*/
		}
		store = store->next;
	}

}


void ExecuteMemoryCmd()
{
	Memory_PiplineStage *execute = Memory_Unit, *prevExecute = NULL;
	int i=0;
	
	prevExecute = execute;
	while (execute->next != NULL) {
		prevExecute = execute;
		execute=execute->next;
	}

	/*the execute operation*/
	switch(execute->OPCODE){

	case LD:
		/*load execution*/
		execute->Data_load = (float)strtol(MainMemoryArray[execute->address], (char**)NULL, 16);
		temp_load.numOfRobSupplier = execute->numOfRobSupplier;
		temp_load.result = execute->Data_load;
		break;

	case ST:
		execute->Data_store = FP_Registers[prevExecute->address].value;
		updateFPRob(execute->numOfRobSupplier, execute->Data_store);
		
		/*store execution*/
		break;

	case -1:
		/*bubble execution*/
		break;

	default:
		//printf("wrong opcode in Memory Unit\n");
		break;
	}


	memset(execute,0,sizeof(Memory_PiplineStage)); //TODO check if WORK!
	execute->OPCODE = -1;
	execute->next = Memory_Unit;
	Memory_Unit = execute;
	prevExecute->next = NULL;

}


void EvictFromLoadAndStoreBuffer(){

	/*finish and clean the instr that finished, and clean all the fields*/
	int i=0;
	LoadBuffer *load = LoadBufferResarvation;
	StoreBuffer *store = StoreBufferResarvation;

	while(load != NULL){
		if(load->done==TRUE){
			load->busy=FALSE;
			load->done=FALSE;
			load->inExecution=FALSE;
			load->addressReady = FALSE;
			load->issued = 0;
			LD_Buff_Cnt--;
		}
		load=load->next;
	}

	while(store != NULL){
		if(store->done==TRUE){
			store->busy=FALSE;
			store->done=FALSE;
			store->inExecution = FALSE;
			load->addressReady = FALSE;
			store->issued=0;
			store->Qj = 0;
			ST_Buff_Cnt--;
		}
		store=store->next;
	}
}

BOOL isST_Buff_FULL(){
	return (ST_Buff_Cnt == Configuration->mem_nr_store_buffers);
}

BOOL isLD_Buff_FULL(){
	return (LD_Buff_Cnt == Configuration->mem_nr_load_buffers);
}

void SimulateClockCycle_LoadUnit(){

	/*a simulate clock cycle for the memory unit, the function calls the nessearlly function to run the */
	/*the memory unit properally*/

	ExecuteMemoryCmd();
	LDBufferToMemoryExecution();
	STBufferToMemoryExecution();
	EvictFromLoadAndStoreBuffer();
	return;
}

void readLine(FILE *file, char *my_string) {
	int count = 0, maximumLineLength = 128;
	char *lineBuffer;
	char ch;
	if (file == NULL) {
		printf("Error: file pointer is null.");
		exit(1);
	}

	
	lineBuffer = (char *)malloc(sizeof(char) * maximumLineLength);

	if (lineBuffer == NULL) {
		printf("Error allocating memory for line buffer.");
		exit(1);
	}

	ch = getc(file);
	if (ch == EOF)
	{
		return ;
	}

	while ((ch != '\n') && (ch != EOF)) {
		if (count == maximumLineLength) {
			maximumLineLength += 128;
			lineBuffer = (char *)realloc(lineBuffer, maximumLineLength);
			if (lineBuffer == NULL) {
				printf("Error reallocating space for line buffer.");
				exit(1);
			}
		}
		lineBuffer[count] = ch;
		count++;

		ch = getc(file);
	}

	lineBuffer[count] = '\0';
	
	strncpy(my_string, lineBuffer, (count + 1));
	free(lineBuffer);
	
	return;
}


void MemInToMainMemory(char *memory[],char *memin_txt)
{
	int pc_conter = 0, i = 0;
	FILE *file = NULL;
	char memoryline[BUFFER_SIZE];
	Instruction *node = NULL;
	file = FileOpen(memin_txt,"rt");
	
	while (i<MEMORY_SIZE) 
	{
		readLine(file, memoryline);
		strcpy(memory, memoryline);
		
		memory=memory + (512/4);
		i++;
	}

	fclose(memin_txt);
}



BOOL isST_Buff_empety(){
	int Number_of_MemReservation_station = Configuration->mem_nr_store_buffers;
	int i = 0;
	StoreBuffer *node = NULL;
	node = StoreBufferResarvation;
	for (i = 1; i<Number_of_MemReservation_station; i++){
		if (TRUE == node->busy){
			return FALSE;
		}
		node = node->next;
	}
	return TRUE;
}

BOOL isLD_Buff_emepty(){
	int Number_of_MemReservation_station = Configuration->mem_nr_load_buffers;
	int i = 0;

	LoadBuffer *node = NULL;
	node = LoadBufferResarvation;
	for (i = 1; i < Number_of_MemReservation_station; i++){
		if (TRUE == node->busy){
			return FALSE;
		}
		node = node->next;
	}
	return TRUE;
}
