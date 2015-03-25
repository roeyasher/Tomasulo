#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

FpReservationStation_Line *CreateNewFPRSNode(){
	FpReservationStation_Line *temp = NULL;
	temp = (FpReservationStation_Line*) malloc(sizeof(FpReservationStation_Line));
	memset(temp, 0, sizeof(FpReservationStation_Line));
	temp->next = NULL;
	return temp;
}

FP_PipelineStage *CreateNewFPPLNode(){
	FP_PipelineStage *temp = NULL;
	temp = (FP_PipelineStage*) malloc(sizeof(FP_PipelineStage));
	memset(temp, 0, sizeof(FP_PipelineStage));
	temp->next = NULL;
	return temp;
}

void Initialize_FpReservationStations(){

	int lengthAdd=Configuration->add_nr_reservation;
	int lengthMUL=Configuration->mul_nr_reservation;
	
	/*Create Reservation station for ADD unit*/
	int i = 0;
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
/*send from reservation station execution unit*/
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
			strcpy(FP_executionPipeline_ADD->name, line->name);
			line->done = TRUE;
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
				strcpy(FP_executionPipeline_MUL->name, line->name);
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
/*The function insert new inst to the RS*/
BOOL FP_InsertToReservationStations_ADD(){

	FpReservationStation_Line *available=NULL,*iter=FpReservationStation_ADD;
	int lengthADD=Configuration->add_nr_reservation;
	int i=0;

	while (iter != NULL){
		/*find available node*/
		if (iter->busy==FALSE){
			available=iter;
			break;
		}
		iter=iter->next;
	}
	/*nothing available */
	if (available == NULL)
		return FALSE;

	/*j is always from register*/
	if (FP_Registers[instr.SRC0].busy == FALSE){
		available->Vj = FP_Registers[instr.SRC0].value;
		available->NumOfRightOperands++;			/*operand j is ready*/
	}
	else{available->Qj = FP_Registers[instr.SRC0].robNum;}

	/*opernad k might be immediate or from register.*/
	if (FP_Registers[instr.SRC1].busy == FALSE){
		available->Vk = FP_Registers[instr.SRC1].value;
		available->NumOfRightOperands++;	
	}
	else{available->Qk = FP_Registers[instr.SRC1].robNum;}

	/*update destination register*/
	FP_Registers[instr.DST].busy=TRUE;
	FP_Registers[instr.DST].robNum = instr.numRob;;

	available->OPCODE = instr.OPCODE;
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

BOOL FP_InsertToReservationStations_MULL(){

	FpReservationStation_Line *available=NULL,*iter=FpReservationStation_MUL;
	int lengthMUL=Configuration->mul_nr_reservation;
	int i=0;

		while (iter != NULL){
		/*find available node*/
		if (iter->busy == FALSE){
			available = iter;
			break;
		}
		iter = iter->next;
	}

	/*nothing available */
	if (available == NULL)
		return FALSE;

	/*if there is a line available, check if instruction is mine. if so, take it to reseervation station*/
	available->OPCODE = instr.OPCODE;

	/*j is always from register*/
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

	FP_PipelineStage *last=NULL, *prevLast=NULL, *temp=NULL,*NewNode=NULL;
	int i=0,j=0;

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
			/*Wrong OPCODE*/
			break;

		}

		// Preapre Values for CDB struct
		temp_fp_add.numOfRobSupplier = last->numOfSupplier;
		temp_fp_add.result = last->result;
		strcpy(temp_fp_add.name, last->name);

	}
	/*advance ADD pipeline one stage forward*/
	memset(last,0,sizeof(FP_PipelineStage)); //TODO check if WORK!
	last->busy = FALSE;
	last->next = FP_executionPipeline_ADD;
	FP_executionPipeline_ADD = last;
	prevLast->next = NULL;
}



void FP_AdvanceFpPipeline_MUL(){

	FP_PipelineStage *last=NULL, *prevLast=NULL, *temp=NULL,*NewNode=NULL;
	int i=0, j=0;

	/*get last to point to last stage of pipeline*/
	last = FP_executionPipeline_MUL;
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
			/*Wrong OPCODE*/
			break;
		}

	
		// Preapre Values for CDB struct
		temp_fp_mull.numOfRobSupplier = last->numOfSupplier;
		temp_fp_mull.result = last->result;

	}

	/*advance ADD pipeline one stage forward*/
	memset(last,0,sizeof(FP_PipelineStage)); //TODO check if WORK!
	last->busy = FALSE;
	last->next = FP_executionPipeline_MUL;
	FP_executionPipeline_MUL = last;
	prevLast->next = NULL;

	}
// Is the RS full
BOOL isFP_RS_ADD_FULL(){
	return (FP_RS_ADD_Cnt == Configuration->add_nr_reservation);
}
// Is the RS full
BOOL isFP_RS_MULL_FULL(){
	return (FP_RS_MULL_Cnt == Configuration->mul_nr_reservation);
}
/*a simulate clock cycle for the memory unit*/
void simulateClockCycle_FpUnit(){

	FP_AdvanceFpPipeline_ADD();
	FP_AdvanceFpPipeline_MUL();
	FP_ReservationStationToExecution();
	FP_EvictFromReservationStation();
	return;
}
//Is the RS empty
BOOL isFP_RS_ADD_empty(){
	FpReservationStation_Line  *node = FpReservationStation_ADD;
	int lengthADD = Configuration->add_nr_reservation;
	int i = 0;

	for (i = 1; i < lengthADD; i++){
		if (TRUE == node->busy){
			return FALSE;
		}
		node = node->next;
	}
}
		
//Is the RS empty
BOOL isFP_RS_MULL_empty(){
	FpReservationStation_Line *available = NULL, *node = FpReservationStation_MUL;
	int lengthADD = Configuration->mul_nr_reservation;
	int i = 0;
	for (i = 1; i < lengthADD; i++){
		if (TRUE == node->busy){
			return FALSE;
		}
		node = node->next;
	}
}

