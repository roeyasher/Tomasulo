#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

/*used to create new reservation station line*/
IntReservationStation_Line *CreateNewIRSLNode(int index){

	IntReservationStation_Line *temp = NULL;
	temp = (IntReservationStation_Line*) malloc(sizeof(IntReservationStation_Line));
	memset(temp, 0, sizeof(IntReservationStation_Line));
	temp->next = NULL;
	temp->robNum = index;
	return temp;		/*NULL is returned if failure occured*/
}

IntALU_PipelineStage *CreateNewIPLSNode(){

	IntALU_PipelineStage *temp = NULL;
	temp = (IntALU_PipelineStage*) malloc(sizeof(IntALU_PipelineStage));
	memset(temp, 0, sizeof(IntALU_PipelineStage));
	temp->next = NULL;
	return temp;		/*NULL is returned if failure occured*/
}

void InitializeReservationStation(){

	int NumberOFReservationStations = Configuration->int_nr_reservation;		/*how many nodes to create*/
	int i = 0;

	IntReservationStation_Line *node = NULL;
	IntReservationStation = CreateNewIRSLNode(0);  /*Create first Line and label it as ADD1*/
	node = IntReservationStation;

	/*Create other Lines and label them ADD2,ADD3 and so on*/
	for (i = 1; i < NumberOFReservationStations; i++){
		node->next = CreateNewIRSLNode(i);
		node = node->next;
	}

	Int_RS_Cnt = 0;
}

void InitializeIntegerALU(){

	/*How long is the pipeline*/
	IntALU_PipelineStage *node = NULL;
	int i = 0, length=0;
	length=Configuration->int_delay;

	Integer_ALU_Unit = CreateNewIPLSNode(); /*first pipeline stage*/
	node = Integer_ALU_Unit;

	for (i=1;i<length;i++){
		node->next = CreateNewIPLSNode();
		node = node->next;
	}

}

BOOL InsertToReservationStation(){

	IntReservationStation_Line *available = NULL, *iter = IntReservationStation;
	int length=Configuration->int_nr_reservation;
	int i = 0;

	/*find available Line in Reservation station*/
	while (iter != NULL){

		/*if not busy then assign to available*/
		if (iter->busy == FALSE){
			available = iter;
			break;
		}
		iter = iter->next;
	}

	/*if available is still NULL then no available station thus return FALSE*/
	if (available == NULL)
		return FALSE;

	/*check if instruction is mine and update instruction type. otherwise report nothing happend by retuning FALSE*/
	if(instr.OPCODE>4 && instr.OPCODE<9) //Integer ALU instruction
		available->OPCODE=instr.OPCODE;
	else
		return FALSE;

	/*operand j is always from register*/
	if (Integer_Registers[instr.SRC0].busy == FALSE){

		available->Vj = Integer_Registers[instr.SRC0].value;
		available->NumOfRightOperands++;				/*operand is ready*/
	}
	else{
		available->Qj = Integer_Registers[instr.SRC0].robNum;
	}

	/*opernad k might be immediate or from register. check if ADD or SUB and register is not busy*/
	if ((Integer_Registers[instr.SRC1].busy == FALSE) && ((instr.OPCODE == ADD) || (instr.OPCODE == SUB))){

		available->Vk = Integer_Registers[instr.SRC1].value;
		available->NumOfRightOperands++;				/*operand is ready*/	
	}

	else if ((instr.OPCODE == ADD) || (instr.OPCODE == SUB)){
	/*copy label if value of register is not relevant*/
		available->Qk = Integer_Registers[instr.SRC1].robNum;
	}

	/*put immediate if ADDI or SUBI*/
	if((instr.OPCODE==ADDI) || (instr.OPCODE==SUBI)){

		available->Vk=instr.IMM;
		memset(available->Qk,0,LABEL_SIZE);
		available->NumOfRightOperands++;				/*operand is ready*/
	}

	/*it's ADD or SUB and register is busy*/

	/*update destination register to being busy and update label to know from whom result is given*/
	Integer_Registers[instr.DST].busy=TRUE;
	Integer_Registers[instr.DST].robNum = available->robNum;

	available->busy = TRUE;
	available->done = FALSE;
	available->inExecution=FALSE;
	available->issued=cycle;
	trace[cycle].issued=cycle;
	trace[cycle].valid=TRUE;
	strcpy(trace[cycle].instruction,instr.name);

	Int_RS_Cnt++;
	return TRUE;								 /*means the instruction has been written to reservation station*/

}

void ReservationStationToALU(){

	int i=0,j=0;
	int length=Configuration->int_nr_reservation;		/*how many Lines in reservation station*/
	IntReservationStation_Line *line=IntReservationStation;

	while (line != NULL){

		/*check if a reservation has it's 2 operands. if so pass to ALU*/
		if ((line->NumOfRightOperands == 2) && (line->inExecution==FALSE)){

			Integer_ALU_Unit->busy=TRUE;
			Integer_ALU_Unit->OPCODE=line->OPCODE;
			Integer_ALU_Unit->operand1=line->Vj;
			Integer_ALU_Unit->operand2=line->Vk;
			Integer_ALU_Unit->numOfRobSupplier = line->robNum;
			line->inExecution=TRUE;			/*so it's not sent again to ALU*/

			for (j=0;j<TRACE_SIZE;j++){

				if (trace[j].issued == line->issued){

					trace[j].execution=cycle;
					break;
				}
			}		
			break;
		}
		line=line->next;
	}
}

void EvictFromIntReservationStation(){

	int i=0;
	int length=Configuration->int_nr_reservation;
	IntReservationStation_Line *line=IntReservationStation;

	while (line != NULL){

		if (line->done == TRUE){

			/*set Line as available and zero fields if needed (e.g  Qj,Qk so that argumnets won't be passed accidently*/
			line->busy=FALSE;
			line->done=FALSE;
			line->inExecution=FALSE;
			line->NumOfRightOperands=0;
			memset(line->Qj,0,LABEL_SIZE);
			memset(line->Qk,0,LABEL_SIZE);
			line->Vj=0;
			line->Vk=0;
			line->OPCODE=-1;
			Int_RS_Cnt--;
		}
		line=line->next;
	}
}

void AdvanceIntPipeline(){

	int i=0,j=0;
	int length=Configuration->int_delay;		/*how long is the pipeline*/

	IntALU_PipelineStage *last=Integer_ALU_Unit, *prevLast=NULL, *NewNode=NULL,*temp=NULL;
	IntReservationStation_Line *line = IntReservationStation;


	/*get last to point to last stage of pipeline*/
	prevLast = last;
	while (last->next != NULL) {
		prevLast = last;
		last=last->next;
	}

	if (last->busy == TRUE) {			/*check if stage has an instruction or a bubble that went on*/
		/*calculate result of operation. in reality it's done in stages but here it's done in one stage and we decide to do it int last stage*/
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

		default:
			// Opcode not in ALU unit
			break;
		}

		/*opearate as CDB and update waiting stations and registers*/
		/*update Integer Reservation satation*/

		while (line != NULL){

			if (line->Qj == last->numOfRobSupplier){
				line->Vj = last->result;			/*update waiting value*/
				memset((void*)line->Qj,0,LABEL_SIZE);	/*reset label so no more unexpected updates occur*/
				line->NumOfRightOperands++;		/*update number of ready operands*/
			}

			if (line->Qk == last->numOfRobSupplier){
				line->Vk=last->result;			/*update waiting value*/
				memset((void*)line->Qk,0,LABEL_SIZE);	/*reset label so no more unexpected updates occur*/
				line->NumOfRightOperands++;		/*update number of ready operands*/
			}

			/*we set reservation station state as done for this instruction*/
			if ( (last->numOfRobSupplier == line->robNum) && (line->inExecution == TRUE) ){
				line->done = TRUE;
				for (j=0;j<TRACE_SIZE;j++){
					if (trace[j].issued == line->issued){
						trace[j].CDB=cycle-1;
						break;
					}
				}
			}
			line=line->next;
		}

		//DELETE!!!!!
		/*update registers*/
		for (i=0;i<NUM_OF_INT_REGISTERS;i++){

			if ((Integer_Registers[i].busy == TRUE) && (Integer_Registers[i].robNum == last->numOfRobSupplier)){

				Integer_Registers[i].value = last->result;			/*update value*/
				Integer_Registers[i].busy = FALSE;					/*no more busy*/
				Integer_Registers[i].robNum = -1;
			}
		}
	}

	/*waiting stations and registers were updated if not a bubble in last stage. Now must advance pipeline one stage foreward*/
	/*if piepeline length is one then we erase content, set it as not busy and we are done*/
	memset(last,0,sizeof(IntALU_PipelineStage)); //TODO check if WORK!
	last->busy = FALSE;
	last->next = Integer_ALU_Unit;
	Integer_ALU_Unit = last;
	prevLast->next = NULL;

}

BOOL isINT_RS_FULL(){
	return (Int_RS_Cnt == Configuration->int_nr_reservation);
}

BOOL SimulateClockCycle_IntUnit(){

	BOOL isInstructionTakenByUnit = FALSE;

	AdvanceIntPipeline();					/*advance piepline*/
	ReservationStationToALU();				/*send new instruction for execution*/
	EvictFromIntReservationStation();		/*evict done instructions from reservation station*/

	if (!isINT_RS_FULL && !isRobFull()) {
		if (InsertToReservationStation())			/*insert new instruction to reservation station*/
			isInstructionTakenByUnit = TRUE;
	}

	return isInstructionTakenByUnit;

}