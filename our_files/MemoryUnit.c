

#include "MemoryUnit.h"



extern IntegerRegister Integer_Registers[NUM_OF_INT_REGISTERS];

extern FpRegister FP_Registers[NUM_OF_FP_REGISTERS];

extern FpReservationStation_Line *FpReservationStation_ADD;

extern FpReservationStation_Line *FpReservationStation_MUL;

extern Configuration_Data *Configuration;

extern Memory_PiplineStage *Memory_Unit;

extern LoadBuffer *LoadBufferResarvation;

extern StoreBuffer *StoreBufferResarvation;

extern Instruction instr;



extern int cycle;

extern Trace trace[TRACE_SIZE];



CdbWriteBack CdbToResarvation;

extern float PhysicalMemoryArray[];

Memory_Pipline BufferToMemory;



void IntilaizeMemoryArray()

{

	int i=0;

	for(i=0;i<MEMORY_SIZE;i++)

	{

		PhysicalMemoryArray[i]=0;

	}

}



static LoadBuffer *CreateNewNode(){

	/*this function create a new node for a linked list, for the load buffer*/

	LoadBuffer *temp = NULL;

	temp = (LoadBuffer*) malloc(sizeof(LoadBuffer));

	memset(temp, 0, sizeof(LoadBuffer));

	temp->next = NULL;

	

	return temp;		/*NULL is returned if failure occured*/

}

static StoreBuffer *CreateNewNodeForStore(){

	/*this function create a new node for a linked list for the store buffer*/

	StoreBuffer *temp = NULL;

	temp = (StoreBuffer*) malloc(sizeof(StoreBuffer));

	memset(temp, 0, sizeof(StoreBuffer));

	temp->next = NULL;

	

	return temp;		/*NULL is returned if failure occured*/

}



void IntilaizeLoadBuffer()

{

	/*this function intilaize the load buffer/reservation station*/

	int Number_of_MemReservation_station = Configuration->mem_nr_load_buffers;

	int i=0;

	LoadBuffer *new_node = NULL,*head = NULL;

	new_node = CreateNewNode();

	sprintf(new_node->Label,"LOAD%d",i+1);

	LoadBufferResarvation = new_node;



	for (i=1;i<Number_of_MemReservation_station;i++)

	{

		head = CreateNewNode();

		sprintf(head->Label,"LOAD%d",i+1);

		new_node->next = head;

		new_node= new_node->next;

	}

}

void IntilaizeStoreBuffer()

{

	/*this function intilaize the store buffer/reservation station*/

	int Number_of_MemReservation_station = Configuration->mem_nr_store_buffers;

	int i=0;

	StoreBuffer *new_node = NULL,*head = NULL;

	new_node = CreateNewNodeForStore();

	sprintf(new_node->Label,"STORE%d",i+1);

	StoreBufferResarvation= new_node;



	for(i=1;i<Number_of_MemReservation_station;i++)

	{

		head = CreateNewNodeForStore();

		sprintf(head->Label,"STORE%d",i+1);

		new_node->next = head;

		new_node=new_node->next;

	}



}

void IntializeMemPipline()

{

	/*this function intilaize the execute memory unit*/

	int length = Configuration->mem_delay;

	int i=0;

	Memory_PiplineStage *new_node = NULL,*head =NULL;

	head=(Memory_PiplineStage*)malloc(sizeof(Memory_PiplineStage));

	memset(head,0,sizeof(Memory_PiplineStage));

	head->next = NULL;

	head->OPCODE=-1;

	Memory_Unit = head;

	for(i=1;i<length;i++)

	{

		head=Memory_Unit;

		while (head->next != NULL){

			head=head->next;

		}

		new_node = (Memory_PiplineStage*)malloc(sizeof(Memory_PiplineStage));

		memset(new_node,0,sizeof(Memory_PiplineStage));

		new_node->OPCODE=-1;

		new_node->next = NULL;

		head->next = new_node;

	}

}



BOOL InsertNewLoadInstr(int count)

{	

	LoadBuffer *available = NULL, *iter = LoadBufferResarvation;

	int length = Configuration->mem_nr_load_buffers;

	int i=0;

	int address=0;

	/*fine available place in the resarvation station for load */

	if (instr.OPCODE != LD)

		return FALSE;



	for(i=0;i<length;i++)

	{

		/*if not busy , assign the current place as available*/

		if(iter->busy == FALSE)

		{

			available = iter;

			break;

		}

		iter = iter->next;

	}

	/*if available is still NULL then there is no empty place and return false */

	if (available == NULL){

		return FALSE;

	}

	/*but we need also to update that the resarvations are full and we cant continue the program*/



	/* now we need to calculate the right address, if its not possible we also stuck*/

	

	/*for load instruction checking the opcode*/

	switch(instr.OPCODE)

	{

		case LD:

			available->OPCODE = LD;

			break;	

		default:

			return FALSE;

			break;

	}

	/*if it is possible - calculating the address*/

	if(Integer_Registers[instr.SRC0].busy == FALSE)

	{

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



	return TRUE;

}



BOOL InsertNewStoreInstr(int count)

{

	StoreBuffer *available = NULL , *iter = StoreBufferResarvation;

	int length = Configuration->mem_nr_store_buffers;

	int i=0;

	int address = 0;

	/*fine available place in the resarvation station for load */

	if (instr.OPCODE != ST)

		return FALSE;



	for(i=0;i<length;i++)

	{

		/*if not busy , assign the current place as available*/

		if(iter->busy == FALSE)

		{

			available = iter;

			break;

		}

		iter = iter->next;

	}

	/*if available is still NULL then there is no empty place and return false */

	if (available == NULL){

		return FALSE;

	}

	/*but we need also to update that the resarvations are full and we cant continue the program*/



	/* now we need to calculate the right address, if its not possible we also stuck*/



	/*for store instruction - checking the opcode */

	switch(instr.OPCODE)

	{

		case ST:

			available->OPCODE = ST;

			break;	

		default:

			return FALSE;

			break;

	}

	/*check if we can calculate the address and the value is avaliable*/

	if(Integer_Registers[instr.SRC0].busy == FALSE)

	{

		address = Integer_Registers[instr.SRC0].value;

		address = address + instr.IMM;

		available->address = address;

	}

	else {

		return FALSE;

	}

	/*insert the FP value it is exist*/

	if(FP_Registers[instr.SRC1].busy == FALSE)

	{

		available->vj=FP_Registers[instr.SRC1].value;

		available->NumOfRightOperands++;

	}

	else

	{

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

	



	return TRUE;

}



LoadBuffer *FindTheLoadMinumum(int count)

{

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

	for(i=0;i<length_store;i++)

	{

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

BOOL IsNotTheSameAddress(int address, char Label[],int count)

{

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

	if(BufferToMemory.OPCODE == -1)

	{

		/*insert a bubble */

		head->OPCODE=-1;

	}

	else

	{

		/*insert the right instruction*/

		head->OPCODE=BufferToMemory.OPCODE;

		head->address=BufferToMemory.address;

		strcpy(head->labelofsupplier,BufferToMemory.labelofsupplier);

		head->Data_load=BufferToMemory.Data_load;

		head->Data_store=BufferToMemory.Data_store;

	}

}

BOOL BufferToMemoryExecution(int counter)

{

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

	for(i=0;i<length_load;i++){

		if(load!=NULL){

		load->checkforexecute = 0;

		load=load->next;}

	}

	for(i=0;i<length_store;i++){

		if(store!=NULL){

		store->checkforexecute = 0;

		store=store->next;}

	}

	/**/

	/* the logic for the pre execute level*/

	for(i=0;i<(length_load+length_store);i++)

	{

		minload = FindTheLoadMinumum(counter);

		minstore = FindTheStoreMinumum(counter);

		/*if both NULL there is no reason to still searching return a nop*/

		if((minload == NULL)&&(minstore == NULL))

		{

			break;

		}

		/*if the minstore is null and the minload is not we can execute minload instr*/

		if((minstore == NULL)&&(minload!=NULL))

		{

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

			else

			{

				if(minstore->NumOfRightOperands ==1)

				{

					if(IsNotTheSameAddress(minstore->address,minstore->Label,minstore->count)==TRUE)

					{

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

			if(IsNotTheSameAddress(minload->address,minload->Label,minload->count)==TRUE)

			{

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

	Memory_PiplineStage *execute = Memory_Unit;

	Memory_PiplineStage *NewNode = NULL;

	int i=0;

	for(i=1;i<length;i++)

	{

		execute=execute->next;

	}

	/*the execute operation*/

	switch(execute->OPCODE)

	{

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

	if(execute->OPCODE == LD)

	{

		strcpy(CdbToResarvation.label,execute->labelofsupplier);

		CdbToResarvation.value = execute->Data_load;

	}

	else if(execute->OPCODE == ST)

	{

		strcpy(CdbToResarvation.label,execute->labelofsupplier);

	}

	else

	{

		strcpy(CdbToResarvation.label,"roie");

	}

	/*if pipeline length is only 1*/

	if (length == 1){

		memset(execute,0,sizeof(Memory_PiplineStage));

		execute->next=NULL;

	}

	else {

		free(execute);/*last stage finished executing and updated everybody thus we release it*/

		NewNode=(Memory_PiplineStage*)malloc(sizeof(Memory_PiplineStage));

		NewNode->OPCODE=-1;

		

		

		memset(NewNode,0,sizeof(Memory_PiplineStage));

		NewNode->next=Memory_Unit;			/*point first node to old first stage of pipeline*/

				

		/*now begining of ALU is the NewNode*/

		

		Memory_Unit=NewNode;

	}

	/*we want last stage of pipeline to point to NULL as next stage*/

	execute=Memory_Unit;				/*point to begining*/

	/*advance till pointing to new last stage (as old last was released) */

	for (i=1;i<length;i++){

		execute=execute->next;		

	}

	execute->next=NULL;	

}



void CdbReturnValue()

{

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

	

	for(i=0;i<length_fp_add;i++)

	{

		if((strcmp(line->Qj,CdbToResarvation.label)==0)&&(line->busy==TRUE) )

		{

			line->Vj=CdbToResarvation.value;

			memset((void*)line->Qj,0,LABEL_SIZE);

			line->NumOfRightOperands++;

		}

		if((strcmp(line->Qk,CdbToResarvation.label)==0)&&(line->busy==TRUE))

		{

			line->Vk=CdbToResarvation.value;

			memset((void*)line->Qj,0,LABEL_SIZE);

			line->NumOfRightOperands++;

		}

		line=line->next;

	}

	line=FpReservationStation_MUL;

	for(i=0;i<length_fp_mul;i++)

	{

		if((strcmp(line->Qj,CdbToResarvation.label)==0)&&(line->busy==TRUE))

		{

			line->Vj=CdbToResarvation.value;

			memset((void*)line->Qj,0,LABEL_SIZE);

			line->NumOfRightOperands++;

		}

		if((strcmp(line->Qk,CdbToResarvation.label)==0)&&(line->busy==TRUE))

		{

			line->	Vk=CdbToResarvation.value;

			memset((void*)line->Qj,0,LABEL_SIZE);

			line->NumOfRightOperands++;

		}

		line=line->next;

	}

	

	

	for(i=0;i<NUM_OF_FP_REGISTERS;i++)

	{

		if((FP_Registers[i].busy == TRUE)&&(strcmp(FP_Registers[i].label,CdbToResarvation.label)==0))

		{

			FP_Registers[i].value = CdbToResarvation.value;

			FP_Registers[i].busy = FALSE;

			memset(FP_Registers[i].label,0,LABEL_SIZE);

		}

	}

	

	for(i=0;i<length_load_buffer;i++)

	{

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

	for(i=0;i<length_store_buffer;i++)

	{

		if(strcmp(store->Label,CdbToResarvation.label)==0)

		{	

			store->done=TRUE;

		}

		store=store->next;

	}



}

void EvictFromLoadAndStoreBuffer()

{

	/*finish and clean the instr that finished, and clean all the fields*/

	int i=0;

	LoadBuffer *load = LoadBufferResarvation;

	StoreBuffer *store = StoreBufferResarvation;

	for(i=0;i<Configuration->mem_nr_load_buffers;i++)

	{

		if(load->done==TRUE)

		{

			load->busy=FALSE;

			load->done=FALSE;

			load->inexecution=FALSE;

			load->count=0;

			load->checkforexecute=0;

			load->address=-1;

		}

		load=load->next;

	}

	for(i=0;i<Configuration->mem_nr_store_buffers;i++)

	{

		if(store->done==TRUE)

		{

			store->busy=FALSE;

			store->done=FALSE;

			store->inexecution=FALSE;

			store->count=0;

			store->issued=0;

			store->checkforexecute=0;

			memset(store->Qj,0,LABEL_SIZE);

			store->NumOfRightOperands=0;

		}

		store=store->next;

	}

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