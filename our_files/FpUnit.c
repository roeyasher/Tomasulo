#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

FpReservationStation_Line *CreateNewFPRSNode(){

	FpReservationStation_Line *temp = NULL;
	temp = (FpReservationStation_Line*) malloc(sizeof(FpReservationStation_Line));
	memset(temp, 0, sizeof(FpReservationStation_Line));
	temp->next = NULL;
	return temp;		/*NULL is returned if failure occured*/
}

FP_PipelineStage *CreateNewFPPLNode(){

	FP_PipelineStage *temp = NULL;
	temp = (FP_PipelineStage*) malloc(sizeof(FP_PipelineStage));
	memset(temp, 0, sizeof(FP_PipelineStage));
	temp->next = NULL;
	return temp;		/*NULL is returned if failure occured*/
}

void Initialize_FpReservationStations(){

	int lengthAdd=Configuration->add_nr_reservation;
	int lengthMUL=Configuration->mul_nr_reservation;

	/*Create Reservation station for ADD unit*/
	int i=0;
	FpReservationStation_Line *node=NULL;
	FpReservationStation_ADD=CreateNewFPRSNode();
	node = FpReservationStation_ADD;

	for (i=1;i<lengthAdd;i++){
		node->next = CreateNewFPRSNode();
		node = node->next;
	}
	FP_RS_ADD_Cnt = 0;

	/*Create Reservation station for MUL unit*/
	i=0;
	FpReservationStation_MUL=CreateNewFPRSNode();
	node = FpReservationStation_MUL;

	for (i=1;i<lengthMUL;i++){
		node->next = CreateNewFPRSNode();
		node = node->next;
	}
	FP_RS_MULL_Cnt = 0;
}

void Initialize_FpPipelines(){

	int lengthADD=Configuration->add_delay;
	int lengthMUL=Configuration->mul_delay;
	int i=0;

	FP_PipelineStage *node = NULL;

	/*Create Pipeline for ADD unit*/
	FP_executionPipeline_ADD= CreateNewFPPLNode();
	node = FP_executionPipeline_ADD;

	for (i=1;i<lengthADD;i++){
		node->next = CreateNewFPPLNode();
		node = node->next;
	}

	/*Create Pipeline for MUL unit*/
	i=0;
	FP_executionPipeline_MUL= CreateNewFPPLNode();
	node = FP_executionPipeline_MUL;

	for (i=1;i<lengthMUL;i++){
		node->next = CreateNewFPPLNode();
		node = node->next;
	}
}

void FP_EvictFromReservationStation(){

	int i=0;
	int lengthADD=Configuration->add_nr_reservation;
	int lengthMUL=Configuration->mul_nr_reservation;

	FpReservationStation_Line *line=FpReservationStation_ADD;

	/*evict for ADD reservation station*/
	while (line != NULL){
		if (line->done == TRUE){
			line->busy=FALSE;
			line->done=FALSE;
			line->inExecution=FALSE;
			line->NumOfRightOperands=0;
			line->robNum = 0;
			line->Qj = 0;
			line->Qk = 0;
			FP_RS_ADD_Cnt--;
		}
		line=line->next;
	}
	

	/*evict for MUL reservation station*/
	line=FpReservationStation_MUL;

	while (line != NULL){
		if (line->done == TRUE){
			line->busy=FALSE;
			line->done=FALSE;
			line->inExecution=FALSE;
			line->NumOfRightOperands=0;
			line->robNum = 0;
			line->Qj = 0;
			line->Qk = 0;
			FP_RS_MULL_Cnt--;
		}
		line=line->next;
	}
	
}

void FP_ReservationStationToExecution(){

	int j;
	int i=0;
	int lengthADD=Configuration->add_nr_reservation;
	int lengthMUL=Configuration->mul_nr_reservation;
	FpReservationStation_Line *line=FpReservationStation_ADD;

	/*send from ADD reservation station to ADD execution unit*/
	while (line != NULL){

		if ((line->NumOfRightOperands == 2) && (line->inExecution==FALSE)){
			FP_executionPipeline_ADD->busy = TRUE;
			FP_executionPipeline_ADD->OPCODE = line->OPCODE;
			FP_executionPipeline_ADD->operand1 = line->Vj;
			FP_executionPipeline_ADD->operand2 = line->Vk;
			FP_executionPipeline_ADD->numOfSupplier = line->robNum;
			line->inExecution=TRUE;

			for (j=0;j<TRACE_SIZE;j++){
				if (trace[j].issued == line->issued)
					break;
			}
			trace[j].execution=cycle;
			break;
		}
		line=line->next;
	}

	/*send from MUL reservation station to MUL execution unit*/
		line=FpReservationStation_MUL;
		while (line != NULL){

			if ((line->NumOfRightOperands == 2) && (line->inExecution==FALSE)){
				FP_executionPipeline_MUL->busy=TRUE;
				FP_executionPipeline_MUL->OPCODE=line->OPCODE;
				FP_executionPipeline_MUL->operand1=line->Vj;
				FP_executionPipeline_MUL->operand2=line->Vk;
				FP_executionPipeline_MUL->numOfSupplier = line->robNum;
				line->inExecution=TRUE;

				for (j=0;j<TRACE_SIZE;j++){
					if (trace[j].issued == line->issued)
					break;
				}
				trace[j].execution=cycle;
				break;
			}
				line=line->next;
	}
}

BOOL FP_InsertToReservationStations_ADD(){

	FpReservationStation_Line *available=NULL,*iter=FpReservationStation_ADD;
	int lengthADD=Configuration->add_nr_reservation;
	int i=0;

	/*find an available line in reseravation station*/
	while (iter != NULL){

		if (iter->busy==FALSE){
			available=iter;
			break;
		}
		iter=iter->next;
	}

	/*case no available lines in reservation station*/
	if (available == NULL)
		return FALSE;

	/*if there is a line available, check if instruction is mine. if so, take it to reseervation station*/
	if(instr.OPCODE==ADDS || instr.OPCODE==SUBS)
		available->OPCODE = instr.OPCODE;
	else
		return FALSE;

	/*operand j*/
	if (FP_Registers[instr.SRC0].busy == FALSE){
		available->Vj = FP_Registers[instr.SRC0].value;
		available->NumOfRightOperands++;			/*operand j is ready*/
	}
	else{
		available->Qj = FP_Registers[instr.SRC0].robNum;	/*copy label of supplier if register is busy*/
	}

	/*operand k. in FP ops there are no immediate*/
	if (FP_Registers[instr.SRC1].busy == FALSE){
		available->Vk = FP_Registers[instr.SRC1].value;
		available->NumOfRightOperands++;			/*operand j is ready*/
	}
	else{
		available->Qk = FP_Registers[instr.SRC1].robNum;/*copy label of supplier if register is busy*/
	}

	/*update destination register*/
	FP_Registers[instr.DST].busy=TRUE;
	FP_Registers[instr.DST].robNum = instr.numRob;;

	available->robNum = instr.numRob;
	available->busy=TRUE;
	available->done=FALSE;
	available->inExecution=FALSE;

	available->issued=cycle;
	trace[cycle].issued=cycle;
	trace[cycle].valid=TRUE;
	strcpy(trace[cycle].instruction,instr.name);

	FP_RS_ADD_Cnt++;
	return TRUE;
}

BOOL FP_InsertToReservationStations_MUL(){


	FpReservationStation_Line *available=NULL,*iter=FpReservationStation_MUL;
	int lengthMUL=Configuration->mul_nr_reservation;
	int i=0;

	/*find an available line in reseravation station*/

	while (iter != NULL){
		/*if not busy then assign to available*/
		if (iter->busy == FALSE){
			available = iter;
			break;
		}
		iter = iter->next;
	}

	/*case no available lines in reservation station*/
	if (available == NULL)
		return FALSE;

	/*if there is a line available, check if instruction is mine. if so, take it to reseervation station*/
	if(instr.OPCODE==MULTS)
		available->OPCODE = instr.OPCODE;
	else
		return FALSE;

	/*operand j*/
	if (FP_Registers[instr.SRC0].busy == FALSE){
		available->Vj = FP_Registers[instr.SRC0].value;
		available->NumOfRightOperands++;			/*operand j is ready*/
	}
	else{
		available->Qj = FP_Registers[instr.SRC0].robNum;	/*copy label of supplier if register is busy*/
	}

	/*operand k. in FP ops there are no immediate*/
	if (FP_Registers[instr.SRC1].busy == FALSE){
		available->Vk = FP_Registers[instr.SRC1].value;
		available->NumOfRightOperands++;			/*operand j is ready*/
	}
	else{
		available->Qk = FP_Registers[instr.SRC1].robNum; /*copy label of supplier if register is busy*/
	}

	/*update destination register*/
	FP_Registers[instr.DST].busy=TRUE;
	FP_Registers[instr.DST].robNum = instr.numRob;;

	available->robNum = instr.numRob;
	available->busy=TRUE;
	available->done=FALSE;
	available->inExecution=FALSE;

	available->issued=cycle;
	trace[cycle].issued=cycle;
	trace[cycle].valid=TRUE;
	strcpy(trace[cycle].instruction,instr.name);

	FP_RS_MULL_Cnt++;
	return TRUE;

}

void FP_AdvanceFpPipeline_ADD(){

	int j;
	int lengthADD=Configuration->add_delay;
	FP_PipelineStage *last=NULL, *prevLast=NULL, *temp=NULL,*NewNode=NULL;
	FpReservationStation_Line *line=NULL;
	StoreBuffer *storeLine=NULL;
	int i=0;

	/*start with ADD unit*/
	/*get last to point to last stage of pipeline*/
	last=FP_executionPipeline_ADD;
	prevLast = last;
	while (last->next != NULL) {
		prevLast = last;
		last=last->next;
	}

	/*calculate result value*/
	if (last->busy == TRUE){
		
		switch(last->OPCODE){
		case ADDS:
			last->result=(last->operand1)+(last->operand2);
			break;

		case SUBS:
			last->result=(last->operand1)-(last->operand2);
			break;

		default:
			//Opcode not in FP_ADD unit
			break;

		}

		/*operate as CDB and update reservation station lines waiting for result and FP register file*/
		/*update reservation stations. first ADD*/

		line=FpReservationStation_ADD;

		while (line != NULL){

			/*update ADD reservation station*/
			if (!strcmp(line->Qj,last->LabelOfSupplier)){
				line->Vj=last->result;
				memset((void*)line->Qj,0,LABEL_SIZE);
				line->NumOfRightOperands++;
			}

			if (!strcmp(line->Qk,last->LabelOfSupplier)){
				line->Vk=last->result;
				memset((void*)line->Qk,0,LABEL_SIZE);
				line->NumOfRightOperands++;
			}

			/*update instruction in reservation station as done*/
			if (!strcmp(last->LabelOfSupplier,line->label)){
				line->done=TRUE;
					for (j=0;j<TRACE_SIZE;j++){
				if (trace[j].issued == line->issued)
					break;
			}
				trace[j].CDB=cycle-1;
			}
			line=line->next;
		}
		/*update MUL reservation station*/
		line=FpReservationStation_MUL;
		while (line != NULL){

			/*update ADD reservation station*/
			if (!strcmp(line->Qj,last->LabelOfSupplier)){
				line->Vj=last->result;
				memset((void*)line->Qj,0,LABEL_SIZE);
				line->NumOfRightOperands++;
			}

			if (!strcmp(line->Qk,last->LabelOfSupplier)){
				line->Vk=last->result;
				memset((void*)line->Qk,0,LABEL_SIZE);
				line->NumOfRightOperands++;
			}
			/*impossible for instruction to come from MUL as it's in ADD piepline*/		
			line=line->next;
		}

			/*update store buffers*/
			storeLine=StoreBufferResarvation;

		while (storeLine != NULL){

				if (!strcmp(storeLine->Qj,last->LabelOfSupplier)){
					storeLine->vj=last->result;
					memset((void*)storeLine->Qj,0,LABEL_SIZE);
					storeLine->NumOfRightOperands++;
				}
				storeLine=storeLine->next;
		}

	}
	/*advance ADD pipeline one stage forward*/
	memset(last,0,sizeof(FP_PipelineStage)); //TODO check if WORK!
	last->busy = FALSE;
	last->next = FP_executionPipeline_ADD;
	FP_executionPipeline_ADD = last;
	prevLast->next = NULL;
}



void FP_AdvanceFpPipeline_MUL(){

	int j;
	int lengthMUL=Configuration->mul_delay;
	FP_PipelineStage *last=NULL, *prevLast=NULL, *temp=NULL,*NewNode=NULL;
	FpReservationStation_Line *line=NULL;
	StoreBuffer *storeLine=NULL;

	int i=0;

	/*Now advance MUL pipeline and update waiting instructions in reservation stations and registers*/
	/*get last to point to last stage of pipeline*/
	last=FP_executionPipeline_MUL;
	prevLast = last;
	while (last->next != NULL) {
		prevLast = last;
		last=last->next;
	}

	/*calculate result value*/
	if (last->busy == TRUE){

		switch(last->OPCODE){

		case MULTS:
			last->result=(last->operand1)*(last->operand2);
			break;
			
		default:
		//Opcode not in FP_MUL unit
			break;
		}

		/*operate as CDB and update reservation station lines waiting for result and FP register file*/

		/*update reservation stations. first ADD then MUL*/

		line=FpReservationStation_ADD;

		// Preapre Values for CDB struct
		temp_fp.numOfRobSupplier = last->numOfSupplier;
		temp_fp.result = last->result;

			
				
			
	}

		/*Now advance MUL pipeline as well*/
	memset(last,0,sizeof(FP_PipelineStage)); //TODO check if WORK!
	last->busy = FALSE;
	last->next = FP_executionPipeline_MUL;
	FP_executionPipeline_MUL = last;
	prevLast->next = NULL;

	}

BOOL isFP_RS_ADD_FULL(){
	return (FP_RS_ADD_Cnt == Configuration->add_nr_reservation);
}

BOOL isFP_RS_MULL_FULL(){
	return (FP_RS_MULL_Cnt == Configuration->mul_nr_reservation);
}

BOOL UpdateResultInRS(){

	
}

BOOL simulateClockCycle_FpUnit(){

	BOOL isInstructionTakenByUnit=FALSE;
	UpdateResultInRS();
	FP_ReservationStationToExecution();
	FP_AdvanceFpPipeline_ADD();
	FP_AdvanceFpPipeline_MUL();
	// TODO One Cycle more
	CDBControlFP(&temp_fp);
	FP_EvictFromReservationStation();

	if (!isFP_RS_ADD_FULL && !isRobFull()){
		if (FP_InsertToReservationStations_ADD())
			isInstructionTakenByUnit = TRUE;
	}

	if (!isFP_RS_MULL_FULL && !isRobFull()){
		if (FP_InsertToReservationStations_MUL())
			isInstructionTakenByUnit = TRUE;
	}

	return isInstructionTakenByUnit;
}


/*

while (line != NULL){

if (!strcmp(line->Qj,last->LabelOfSupplier)){
line->Vj=last->result;
memset((void*)line->Qj,0,LABEL_SIZE);
line->NumOfRightOperands++;
}

if (!strcmp(line->Qk,last->LabelOfSupplier)){
line->Vk=last->result;
memset((void*)line->Qk,0,LABEL_SIZE);
line->NumOfRightOperands++;
}
//impossible for instruction to come from ADD reservation station as it's in MUL pipeline
line = line->next;
		}

		//update MUL reservation station
		line = FpReservationStation_MUL;
		while (line != NULL){

			if (!strcmp(line->Qj, last->LabelOfSupplier)){
				line->Vj = last->result;
				memset((void*)line->Qj, 0, LABEL_SIZE);
				line->NumOfRightOperands++;
			}

			if (!strcmp(line->Qk, last->LabelOfSupplier)){
				line->Vk = last->result;
				memset((void*)line->Qk, 0, LABEL_SIZE);
				line->NumOfRightOperands++;
			}

			//update instruction in reservation station as done
			if (!strcmp(last->LabelOfSupplier, line->label)){
				line->done = TRUE;
				for (j = 0; j<TRACE_SIZE; j++){
					if (trace[j].issued == line->issued)
						break;
				}
				trace[j].CDB = cycle - 1;
			}
			line = line->next;
		}

		//update store buffers
		storeLine = StoreBufferResarvation;
		for (i = 0; i<Configuration->mem_nr_store_buffers; i++){
			if (!strcmp(storeLine->Qj, last->LabelOfSupplier)){
				storeLine->vj = last->result;
				memset((void*)storeLine->Qj, 0, LABEL_SIZE);
				storeLine->NumOfRightOperands++;
			}
			storeLine = storeLine->next;
		}

*/


