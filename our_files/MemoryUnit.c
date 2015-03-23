#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"


CdbWriteBack CdbToResarvation;
Memory_Pipline BufferToMemory;

void IntilaizeMemoryArray()

{
	int i=0;
	for(i=0;i<MEMORY_SIZE;i++)
		PhysicalMemoryArray[i]=0;
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

void IntilaizeLoadBuffer()

{
	/*this function intilaize the load buffer/reservation station*/

	int Number_of_MemReservation_station = Configuration->mem_nr_load_buffers;
	int i=0;

	LoadBuffer *node = NULL;
	LoadBufferResarvation = CreateLBNewNode();
	sprintf(LoadBufferResarvation->Label,"LOAD%d",i+1);
	node = LoadBufferResarvation;

	for (i=1;i<Number_of_MemReservation_station;i++){

		node->next = CreateLBNewNode();
		node = node->next;
		sprintf(node->Label, "LOAD%d", i + 1);
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
	sprintf(StoreBufferResarvation->Label,"STORE%d",i+1);
	node = StoreBufferResarvation;

	for(i=1;i<Number_of_MemReservation_station;i++){
		node->next = CreateNewSBNode();
		node = node->next;
		sprintf(node->Label,"STORE%d",i+1);
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

BOOL InsertNewLoadInstr(int count){	

	LoadBuffer *available = NULL, *iter = LoadBufferResarvation;
	int length = Configuration->mem_nr_load_buffers;
	int i=0;
	int address=0;

	/*fine available place in the resarvation station for load */
	if (instr.OPCODE != LD)
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

	/*for load instruction checking the opcode*/
	if(instr.OPCODE == LD)
		available->OPCODE = LD;
	else
		return FALSE;

	/*if it is possible - calculating the address*/
	if(Integer_Registers[instr.SRC0].busy == FALSE){
		address = Integer_Registers[instr.SRC0].value;
		address = address + instr.IMM;
		available->address = address;
	}
	else {
		return FALSE;
	}

	/*indicate that it is ok and the instruction was insert into the relevant reservation station*/
	available->busy = TRUE;
	available->inexecution = FALSE;
	available->count = count;
	available->checkforexecute=0;

	/*insert the label to the destination register*/
	FP_Registers[instr.DST].busy = TRUE;
	memset(FP_Registers[instr.DST].label,0,LABEL_SIZE);
	strcpy(FP_Registers[instr.DST].label,available->Label);

	LD_Buff_Cnt++;
	return TRUE;
	
}

BOOL InsertNewStoreInstr(int count){

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
	if(instr.OPCODE == ST)
		available->OPCODE = ST;
	else
		return FALSE;

	/*check if we can calculate the address and the value is avaliable*/
	if(Integer_Registers[instr.SRC0].busy == FALSE){
		address = Integer_Registers[instr.SRC0].value;
		address = address + instr.IMM;
		available->address = address;
	}
	else {
		return FALSE;
	}

	/*insert the FP value it is exist*/

	if(FP_Registers[instr.SRC1].busy == FALSE){

		available->vj=FP_Registers[instr.SRC1].value;
		available->NumOfRightOperands++;
	}
	else{
		/*else waiting for the instruction to finish - by waiting to the instruction label*/
		strcpy(available->Qj,FP_Registers[instr.SRC1].label);
	}

	/*insert the new instruction to the store buffer/reservation station and update the relevant fields*/

	available->checkforexecute=0;
	available->busy = TRUE;
	available->inexecution = FALSE;
	available->count = count;
	available->issued=cycle;
	trace[cycle].issued=cycle;
	trace[cycle].valid=TRUE;
	strcpy(trace[cycle].instruction,instr.name);

	ST_Buff_Cnt++;
	return TRUE;
}

LoadBuffer *FindTheLoadMinumum(int count){

	/*this function is responseble to fine the load minimum inst, by the cycle variable with the condition of checkforexecute*/
	/*that if we tried to execute this intr we dont return it*/

	LoadBuffer *load = LoadBufferResarvation;
	LoadBuffer *minload = NULL;

	int length_load = Configuration->mem_nr_load_buffers;
	int i=0;
	int min=count;

	for(i=0;i<length_load;i++)
	{
		if((load->busy == TRUE)&&(load->checkforexecute == 0)&&(load->inexecution==FALSE))
		{
			if(load->count<=min)
			{
				min=load->count;
				minload = load;
			}
		}
		load = load->next;
	}

	/*return the minimum load instr or null if its empty*/
	return minload;
}

StoreBuffer *FindTheStoreMinumum(int count)
{
	/*this function is responseble to fine the store minimum inst, by the cycle variable with the condition of checkforexecute*/
	/*that if we tried to execute this intr we dont return it*/
	StoreBuffer *store = StoreBufferResarvation;
	StoreBuffer *minstore = NULL;

	int length_store = Configuration->mem_nr_store_buffers;
	int i=0;
	int min=count;

	for(i=0;i<length_store;i++){
		if((store->busy == TRUE)&&(store->checkforexecute==0)&&(store->inexecution==FALSE))
		{
			if(store->count<=min)
			{
				min=store->count;
				minstore=store;
			}	
		}
		store=store->next;
	}
	/*return the minimum load instr or null if its empty*/
	return minstore;
}

BOOL IsNotTheSameAddress(int address, char Label[],int count){

	/*the function is checking if the instr we want to execute is in the right order and can be execute*/
	StoreBuffer *store = StoreBufferResarvation;
	int length = Configuration->mem_nr_store_buffers;
	int i=0;
	for(i=0;i<length;i++)
	{
		if((address == store->address)&&(strcmp(Label,store->Label)!=0)&&(count > store->count)&&(store->inexecution==FALSE)&&(store->busy==TRUE)){
			return FALSE;
		}
		store=store->next;
	}
	return TRUE;
}

void AddTheInstrToExecute()
{
	Memory_PiplineStage *head = Memory_Unit;
	//int length = Configuration->mem_delay;
	int i=0;

	if(BufferToMemory.OPCODE == -1){
		/*insert a bubble */
		head->OPCODE=-1;
	}
	else{
		/*insert the right instruction*/
		head->OPCODE=BufferToMemory.OPCODE;
		head->address=BufferToMemory.address;
		strcpy(head->labelofsupplier,BufferToMemory.labelofsupplier);
		head->Data_load=BufferToMemory.Data_load;
		head->Data_store=BufferToMemory.Data_store;
	}
}

BOOL BufferToMemoryExecution(int counter){

	/*this is the main function witch responsable on the execute instr*/
	/*the function try to find a possible instr to execute by using some conditions and if it is ok it do so, else it is insert a nop to execute*/
	int j=0;
	LoadBuffer *load = LoadBufferResarvation;
	StoreBuffer *store = StoreBufferResarvation; /*still not creat*/
	LoadBuffer *minload = NULL;
	StoreBuffer *minstore = NULL;

	int i=0;
	int length_load = Configuration->mem_nr_load_buffers;
	int length_store = Configuration->mem_nr_store_buffers;

	/*intialize from the last time , every time checking again and clean all*/
	while(load != NULL){
			load->checkforexecute = 0;
			load=load->next;
	}
	while(store != NULL){
			store->checkforexecute = 0;
			store=store->next;
	}
	/**/
	/* the logic for the pre execute level*/
	for(i=0;i<(length_load+length_store);i++)
	{
		minload = FindTheLoadMinumum(counter);
		minstore = FindTheStoreMinumum(counter);

		/*if both NULL there is no reason to still searching return a nop*/
		if((minload == NULL)&&(minstore == NULL)){
			break;
		}

		/*if the minstore is null and the minload is not we can execute minload instr*/
		if((minstore == NULL)&&(minload!=NULL)){
			minload->inexecution=TRUE;
			BufferToMemory.OPCODE = minload->OPCODE;
			BufferToMemory.address = minload->address;
			strcpy(BufferToMemory.labelofsupplier,minload->Label);
			for (j=0;j<TRACE_SIZE;j++){
				if (trace[j].issued == minload->issued)
					break;
			}
			trace[j].execution=cycle;
			return TRUE;
			/*can execute and break the for a load instruction*/
		}

		/*the same for minstore +  checking that the operands is exist and it can be execute(we have the value)*/
		else if((minload == NULL)&&(minstore!=NULL))
		{
			if(minstore->NumOfRightOperands ==1)
			{
				if(IsNotTheSameAddress(minstore->address,minstore->Label,minstore->count)== TRUE)
				{
					minstore->inexecution=TRUE;
					BufferToMemory.OPCODE = minstore->OPCODE;
					BufferToMemory.address = minstore->address;
					strcpy(BufferToMemory.labelofsupplier,minstore->Label);
					BufferToMemory.Data_store=minstore->vj;
					for (j=0;j<TRACE_SIZE;j++){
						if (trace[j].issued == minstore->issued)
							break;
					}
					trace[j].execution=cycle;
					return TRUE;
					/*can execute and break the for a store instruction*/
				}
			}
			minstore->checkforexecute=1;
		}
		else
		{
			if(minload->count < minstore->count)
			{
				minload->inexecution=TRUE;
				BufferToMemory.OPCODE = minload->OPCODE;
				BufferToMemory.address = minload->address;
				strcpy(BufferToMemory.labelofsupplier,minload->Label);
				for (j=0;j<TRACE_SIZE;j++){
					if (trace[j].issued == load->issued)
						break;
				}
				trace[j].execution=cycle;
				return TRUE;
				/*can execute and break the for a load instruction*/
			}
			else{
				if(minstore->NumOfRightOperands ==1){
					if(IsNotTheSameAddress(minstore->address,minstore->Label,minstore->count)==TRUE){
						minstore->inexecution=TRUE;
						BufferToMemory.OPCODE= minstore->OPCODE;
						BufferToMemory.address= minstore->address;
						strcpy(BufferToMemory.labelofsupplier,minstore->Label);
						BufferToMemory.Data_store=minstore->vj;	
						for (j=0;j<TRACE_SIZE;j++){
							if (trace[j].issued == minstore->issued)
								break;
						}
						trace[j].execution=cycle;
						return TRUE;
						/*can execute and break the for a store instruction*/
					}
				}
				minstore->checkforexecute=1;
			}
			if(IsNotTheSameAddress(minload->address,minload->Label,minload->count)==TRUE){
				minload->inexecution=TRUE;
				BufferToMemory.OPCODE = minload->OPCODE;
				BufferToMemory.address = minload->address;
				strcpy(BufferToMemory.labelofsupplier,minload->Label);
				for (j=0;j<TRACE_SIZE;j++){
					if (trace[j].issued == minload->issued)
						break;
				}
				trace[j].execution=cycle;
				return TRUE;
				/*can execute and break the for a load instruction*/
			}
			minload->checkforexecute=1;
		}
	}

	/**/
	BufferToMemory.OPCODE=-1;
	return FALSE;
}

void ExecuteMemoryAndWriteToCdb()
{
	int length = Configuration->mem_delay;
	Memory_PiplineStage *execute = Memory_Unit, *prevExecute = NULL;
	Memory_PiplineStage *NewNode = NULL;

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
		execute->Data_load=PhysicalMemoryArray[execute->address];
		break;

	case ST:
		PhysicalMemoryArray[execute->address]=execute->Data_store;
		/*store execution*/
		break;

	case -1:
		/*bubble execution*/
		break;

	default:
		//printf("wrong opcode in Memory Unit\n");
		break;
	}

	if(execute->OPCODE == LD){
		strcpy(CdbToResarvation.label,execute->labelofsupplier);
		CdbToResarvation.value = execute->Data_load;
	}
	else if(execute->OPCODE == ST){
		strcpy(CdbToResarvation.label,execute->labelofsupplier);
	}
	else{
		strcpy(CdbToResarvation.label,"Empty");
	}

	memset(execute,0,sizeof(Memory_PiplineStage)); //TODO check if WORK!
	execute->next = Memory_Unit;
	Memory_Unit = execute;
	prevExecute->next = NULL;

}



void CdbReturnValue(){

	/*this function return the calculate value to the units that waiting for this value depends if it is a store instr or load instr*/
	int length_fp_mul = Configuration->mul_nr_reservation;
	int length_fp_add = Configuration->add_nr_reservation;
	int length_load_buffer = Configuration->mem_nr_load_buffers;
	int i=0,j=0;
	int length_store_buffer = Configuration->mem_nr_store_buffers;

	LoadBuffer *load = LoadBufferResarvation;
	StoreBuffer *store = StoreBufferResarvation;
	FpReservationStation_Line *line = NULL;

	line = FpReservationStation_ADD;
	while(line != NULL){
		if((strcmp(line->Qj,CdbToResarvation.label)==0)&&(line->busy==TRUE) ){
			line->Vj=CdbToResarvation.value;
			memset((void*)line->Qj,0,LABEL_SIZE);
			line->NumOfRightOperands++;
		}
		if((strcmp(line->Qk,CdbToResarvation.label)==0)&&(line->busy==TRUE)){
			line->Vk=CdbToResarvation.value;
			memset((void*)line->Qj,0,LABEL_SIZE);
			line->NumOfRightOperands++;
		}
		line=line->next;
	}

	line=FpReservationStation_MUL;
	while(line != NULL){
		if((strcmp(line->Qj,CdbToResarvation.label)==0)&&(line->busy==TRUE)){
			line->Vj=CdbToResarvation.value;
			memset((void*)line->Qj,0,LABEL_SIZE);
			line->NumOfRightOperands++;
		}
		if((strcmp(line->Qk,CdbToResarvation.label)==0)&&(line->busy==TRUE)){
			line->	Vk=CdbToResarvation.value;
			memset((void*)line->Qj,0,LABEL_SIZE);
			line->NumOfRightOperands++;
		}
		line=line->next;
	}

	for(i=0;i<NUM_OF_FP_REGISTERS;i++){
		if((FP_Registers[i].busy == TRUE)&&(strcmp(FP_Registers[i].label,CdbToResarvation.label)==0)){
			FP_Registers[i].value = CdbToResarvation.value;
			FP_Registers[i].busy = FALSE;
			memset(FP_Registers[i].label,0,LABEL_SIZE);
		}
	}

	for(i=0;i<length_load_buffer;i++){
		if(strcmp(load->Label,CdbToResarvation.label)==0)
		{
			load->done=TRUE;
			for (j=0;j<TRACE_SIZE;j++){
				/*if (trace[j].issued == line->issued)*/
				/*gadi : the first mistake is here its should be the variable*/ 
				/*load as you can see that we run on it and not the variable name:line*/
				if (trace[j].issued == load->issued)
					break;
			}
			trace[j].CDB=cycle-1;
		}
		load=load->next;
	}

	for(i=0;i<length_store_buffer;i++){
		if(strcmp(store->Label,CdbToResarvation.label)==0){	
			store->done=TRUE;
		}
		store=store->next;
	}
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
			load->inexecution=FALSE;
			load->count=0;
			load->checkforexecute=0;
			load->address=-1;
			LD_Buff_Cnt--;
		}
		load=load->next;
	}

	while(store != NULL){
		if(store->done==TRUE){
			store->busy=FALSE;
			store->done=FALSE;
			store->inexecution=FALSE;
			store->count=0;
			store->issued=0;
			store->checkforexecute=0;
			memset(store->Qj,0,LABEL_SIZE);
			store->NumOfRightOperands=0;
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

BOOL SimulateClockCycle_LoadUnit(int count,int flag){

	/*a simulate clock cycle for the memory unit, the function calls the nessearlly function to run the */
	/*the memory unit properally*/
	BOOL isInstructionTakenByUnit=FALSE;

	CdbReturnValue();
	ExecuteMemoryAndWriteToCdb();
	BufferToMemoryExecution(count);
	AddTheInstrToExecute();

	if (InsertNewLoadInstr(count)){
		isInstructionTakenByUnit=TRUE;
	}
	if (InsertNewStoreInstr(count)){
		isInstructionTakenByUnit = TRUE;
	}
	EvictFromLoadAndStoreBuffer();
	return isInstructionTakenByUnit;	
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
	
	//rewind(memin_txt);
	int pc_conter = 0, i = 0;
	FILE *file = NULL;
	char memoryline[BUFFER_SIZE];
	Instruction *node = NULL;
	file = FileOpen(memin_txt,"rt");
	
	

	
	
	//MainMemory[i] = "goni";
	
	//while(fread(instruction_line,sizeof(char),8,memin_txt) != EOF)
//	memset(memory[i][0], 0, BUFFER_SIZE);
	//fscanf(file, "%s", MainMemory[i]);
	while (i<MEMORY_SIZE) 
	{
		readLine(file, memoryline);
		strcpy(memory, memoryline);
		
		memory=memory + (512/4);
		i++;
		//memset(memory[i], 0, BUFFER_SIZE);
	}
	fclose(memin_txt);

}
