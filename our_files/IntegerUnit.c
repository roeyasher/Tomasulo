#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

/*The function create a new node for a linked list*/
IntReservationStation_Line *CreateNewIRSLNode(){
	IntReservationStation_Line *temp = NULL;
	temp = (IntReservationStation_Line*) malloc(sizeof(IntReservationStation_Line));
	memset(temp, 0, sizeof(IntReservationStation_Line));
	temp->next = NULL;
	return temp;
}
/*The function create a new node for a linked list for the Pip*/
IntALU_PipelineStage *CreateNewIPLSNode(){

	IntALU_PipelineStage *temp = NULL;
	temp = (IntALU_PipelineStage*) malloc(sizeof(IntALU_PipelineStage));
	memset(temp, 0, sizeof(IntALU_PipelineStage));
	temp->next = NULL;
	return temp;
}
/*The function intilaize the INT RS*/
void InitializeReservationStation(){

	int NumberOFReservationStations = Configuration->int_nr_reservation;
	int i = 0;

	IntReservationStation_Line *node = NULL;
	IntReservationStation = CreateNewIRSLNode();
	node = IntReservationStation;

	for (i = 1; i < NumberOFReservationStations; i++){
		node->next = CreateNewIRSLNode();
		node = node->next;
	}

	Int_RS_Cnt = 0;
}
/*this function intilaize the execute memory unit*/
void InitializeIntegerALU(){

	IntALU_PipelineStage *node = NULL;
	int i = 0, length=0;
	length=Configuration->int_delay;

	Integer_ALU_Unit = CreateNewIPLSNode();
	node = Integer_ALU_Unit;

	for (i=1;i<length;i++){
		node->next = CreateNewIPLSNode();
		node = node->next;
	}
}
/*The function insert new inst to the RS*/
BOOL Int_InsertToReservationStation(){

	IntReservationStation_Line *available = NULL, *iter = IntReservationStation;
	int length=Configuration->int_nr_reservation;
	int i = 0;
	
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

	/*j is always from register*/
	if (Integer_Registers[instr.SRC0].busy == FALSE){
		available->Vj = Integer_Registers[instr.SRC0].value;
		available->NumOfRightOperands++;
	}
	else{available->Qj = Integer_Registers[instr.SRC0].robNum;}

	if ((instr.OPCODE == ADD) || (instr.OPCODE == SUB)){
		/*opernad k might be immediate or from register.*/
		if (Integer_Registers[instr.SRC1].busy == FALSE){
			available->Vk = Integer_Registers[instr.SRC1].value;
			available->NumOfRightOperands++;
		}
		/*copy label if value of register is not relevant*/
		else{ available->Qk = Integer_Registers[instr.SRC1].robNum; }
	}
	else{
		/*put immediate if ADDI or SUBI*/
		available->Vk=instr.IMM;
		available->Qk = 0;
		available->NumOfRightOperands++;
	}
	/*Is it LD,ST Inst? calc the address*/
	if (!(instr.OPCODE == ST) && !(instr.OPCODE == LD)){
		Integer_Registers[instr.DST].busy = TRUE;
		Integer_Registers[instr.DST].robNum = instr.numRob;
		available->robNum = instr.numRob;
	}
	/*init the node*/
	available->OPCODE = instr.OPCODE;
	available->busy = TRUE;
	available->done = FALSE;
	available->inExecution=FALSE;
	available->num=instr.num;
	strcpy(available->name, instr.name);

	Int_RS_Cnt++;
	return TRUE;	

}
/* Send the instruction from the buffer to Ex*/
void ReservationStationToALU(){

	IntReservationStation_Line *line=IntReservationStation;

	while (line != NULL){
		/*check if a reservation has it's 2 operands.*/
		if ((line->NumOfRightOperands == 2) && (line->inExecution==FALSE)){

			Integer_ALU_Unit->busy=TRUE;
			Integer_ALU_Unit->OPCODE=line->OPCODE;
			Integer_ALU_Unit->operand1=line->Vj;
			Integer_ALU_Unit->operand2=line->Vk;
			Integer_ALU_Unit->numOfRobSupplier = line->robNum;
			Integer_ALU_Unit->num = line->num;
			strcpy(Integer_ALU_Unit->name,line->name);
			line->done = TRUE;
			line->inExecution=TRUE;
			trace[line->num].execution_start = cycle;
			break;
		}
		line=line->next;
	}
}
/*cleaning the finished fields*/
void EvictFromIntReservationStation(){

	int i=0;
	int length=Configuration->int_nr_reservation;
	IntReservationStation_Line *line=IntReservationStation;

	while (line != NULL){
		if (line->done == TRUE){
			line->busy=FALSE;
			line->done=FALSE;
			line->inExecution=FALSE;
			line->NumOfRightOperands=0;
			line->Qj = 0;
			line->Qk = 0;
			line->Vj=0;
			line->Vk=0;
			line->OPCODE=-1;
			Int_RS_Cnt--;
		}
		line=line->next;
	}
}

void AdvanceIntPipeline(){

	IntALU_PipelineStage *last=Integer_ALU_Unit, *prevLast=NULL, *NewNode=NULL,*temp=NULL;
	int i = 0, j = 0;

	/*get last to point to last stage of pipeline*/
	prevLast = last;
	while (last->next != NULL) {
		prevLast = last;
		last=last->next;
	}

	/*calculate result of operation.*/
	if (last->busy == TRUE) {			
		switch(last->OPCODE){

		case ADD:
			last->result = (last->operand1) + (last->operand2);
			break;

		case ADDI:
			last->result = (last->operand1) + (last->operand2);
			break;

		case SUB:
			last->result = (last->operand1) - (last->operand2);
			break;

		case SUBI:
			last->result = (last->operand1) - (last->operand2);
			break;

		case LD:
			last->result = (last->operand1) + (last->operand2);
			temp_int.STLDIns = TRUE;
			break;

		case ST:
			last->result = (last->operand1) + (last->operand2);
			temp_int.STLDIns = TRUE;
			break;

		default:
			/*Wrong OPCODE*/
			break;
		}

		// Preapre Values for CDB struct
		temp_int.numOfRobSupplier = last->numOfRobSupplier;
		temp_int.result = last->result;
		temp_int.num = last->num;
		strcpy(temp_int.name, last->name);
	}

	/* make the last node in the list the first one.*/
	memset(last,0,sizeof(IntALU_PipelineStage)); //TODO check if WORK!
	last->busy = FALSE;
	last->next = Integer_ALU_Unit;
	Integer_ALU_Unit = last;
	prevLast->next = NULL;

}
// Is the RS full
BOOL isINT_RS_FULL(){
	return (Int_RS_Cnt == Configuration->int_nr_reservation);
}
/*a simulate clock cycle for the memory unit*/
void SimulateClockCycle_IntUnit(){

	AdvanceIntPipeline();				
	ReservationStationToALU();
	EvictFromIntReservationStation();		
	return;
}
//Is the RS empty
BOOL isINT_RS_empty(){
	int Number_of_INTReservation_station = Configuration->int_nr_reservation;
	int i = 0;

	LoadBuffer *node = NULL;
	node = IntReservationStation;
	for (i = 1; i <Number_of_INTReservation_station; i++){
		if (TRUE == node->busy){
			return FALSE;
		}
		node = node->next;
	}
	return TRUE;
}
