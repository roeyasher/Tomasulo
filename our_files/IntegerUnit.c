#include "IntegerUnit.h"



extern IntegerRegister Integer_Registers[NUM_OF_INT_REGISTERS];

extern Configuration_Data *Configuration;

extern IntReservationStation_Line *IntReservationStation;

extern IntALU_PipelineStage *Integer_ALU_Unit;

extern Instruction instr;



extern int cycle;

extern Trace trace[TRACE_SIZE];



void Initialize_IntRegisters(){

	int i=0;

	for (i=0;i<NUM_OF_INT_REGISTERS;i++){

		Integer_Registers[i].value=0;

		Integer_Registers[i].busy=FALSE;

		memset(Integer_Registers[i].label,0,LABEL_SIZE);

	}

}



/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/

/*used to create new reservation station line*/

static IntReservationStation_Line *CreateNewNode(){

	IntReservationStation_Line *temp = NULL;

	temp = (IntReservationStation_Line*) malloc(sizeof(IntReservationStation_Line));

	memset(temp, 0, sizeof(IntReservationStation_Line));

	temp->next = NULL;

	

	return temp;		/*NULL is returned if failure occured*/

}



/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/



void InitializeReservationStation(){

	int NumberOFReservationStations = Configuration->int_nr_reservation;		/*how many nodes to create*/

	int i = 0;

	IntReservationStation_Line *NewNode = NULL, *head = NULL;





	/*Create first Line and label it as ADD1*/

	head = CreateNewNode();

	sprintf(head->label, "ADD%d", i + 1);

	IntReservationStation = head;



	/*Create other Lines and label them ADD2,ADD3 and so on*/



	for (i = 1; i < NumberOFReservationStations; i++){

		head->next = CreateNewNode();

		head = head->next;

		sprintf(head->label, "ADD%d", i + 1);

	}

}



/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/





/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/





void InitializeIntegerALU(){

	/*How long is the pipeline*/

	int length=Configuration->int_delay;

	int i=0;

	IntALU_PipelineStage *NewNode=NULL,*head=NULL;



	head=(IntALU_PipelineStage*)malloc(sizeof(IntALU_PipelineStage));		/*first pipeline stage*/

	memset(head,0,sizeof(IntALU_PipelineStage));

	head->next=NULL;

	Integer_ALU_Unit=head;



	for (i=1;i<length;i++){

		head=Integer_ALU_Unit;

		while (head->next != NULL){

			head=head->next;

		}



		NewNode=(IntALU_PipelineStage*)malloc(sizeof(IntALU_PipelineStage));

		memset(NewNode,0,sizeof(IntALU_PipelineStage));

		

		NewNode->next=NULL;

		head->next=NewNode;

		

	}

		

}





/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/





BOOL InsertToReservationStation(){

	IntReservationStation_Line *available = NULL, *iter = IntReservationStation;

	int length=Configuration->int_nr_reservation;

	int i = 0;



	/*find available Line in Reservation station*/

	for (i = 0; i < length; i++){

		/*if not busy then assign to available*/

		if (iter->busy == FALSE){

			available = iter;

			break;

		}

		iter = iter->next;

	}



	/*if available is still NULL then no available station thus return FALSE*/

	if (available == NULL){

		return FALSE;

	}



	/*check if instruction is mine and update instruction type. otherwise report nothing happend by retuning FALSE*/

	

	switch(instr.OPCODE){

	case ADD:

		available->OPCODE=ADD;

		break;

	case ADDI:

		available->OPCODE=ADDI;

		break;

	case SUB:

		available->OPCODE=SUB;

		break;

	case SUBI:

		available->OPCODE=SUBI;

		break;

	default:

		return FALSE;

		break;

	}



	

	



	/*operand j is always from register*/

	if (Integer_Registers[instr.SRC0].busy == FALSE){

		available->Vj = Integer_Registers[instr.SRC0].value;

		available->NumOfRightOperands++;				/*operand is ready*/

		

	}

	else{

		strcpy(available->Qj,Integer_Registers[instr.SRC0].label);		/*copy label if value of register is not relevant*/

	}

	

	/*opernad k might be immediate or from register. check if ADD or SUB and register is not busy*/

	if ((Integer_Registers[instr.SRC1].busy == FALSE) && ((instr.OPCODE == ADD) || (instr.OPCODE == SUB))){

		available->Vk = Integer_Registers[instr.SRC1].value;

		available->NumOfRightOperands++;				/*operand is ready*/

		

		

	}

	else if ((instr.OPCODE == ADD) || (instr.OPCODE == SUB)){

		strcpy(available->Qk, Integer_Registers[instr.SRC1].label);		/*copy label if value of register is not relevant*/

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

	memset(Integer_Registers[instr.DST].label,0,LABEL_SIZE);

	strcpy(Integer_Registers[instr.DST].label,available->label);



	available->busy = TRUE;

	available->done = FALSE;

	available->inExecution=FALSE;

	available->issued=cycle;

	trace[cycle].issued=cycle;

	trace[cycle].valid=TRUE;

	strcpy(trace[cycle].instruction,instr.name);

	

	return TRUE;								 /*means the instruction has been written to reservation station*/

}





/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/



void ReservationStationToALU(){

	int i=0,j=0;

	int length=Configuration->int_nr_reservation;		/*how many Lines in reservation station*/

	IntReservationStation_Line *Line=IntReservationStation;



	for (i=0;i<length;i++){

		/*check if a reservation has it's 2 operands. if so pass to ALU*/

		if ((Line->NumOfRightOperands == 2) && (Line->inExecution==FALSE)){

			

			Integer_ALU_Unit->busy=TRUE;

			Integer_ALU_Unit->OPCODE=Line->OPCODE;

			Integer_ALU_Unit->operand1=Line->Vj;

			Integer_ALU_Unit->operand2=Line->Vk;

			strcpy(Integer_ALU_Unit->LabelOfSupplier,Line->label);

			Line->inExecution=TRUE;			/*so it's not sent again to ALU*/



			for (j=0;j<TRACE_SIZE;j++){

				if (trace[j].issued == Line->issued){

					

					trace[j].execution=cycle;

					break;

				}

			}

			

			

			

			break;

		}

		Line=Line->next;

	}

	

}



/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/



void EvictFromIntReservationStation(){

	int i=0;

	int length=Configuration->int_nr_reservation;

	IntReservationStation_Line *Line=IntReservationStation;



	for (i=0;i<length;i++){

		if (Line->done == TRUE){

			/*set Line as available and zero fields if needed (e.g  Qj,Qk so that argumnets won't be passed accidently*/

			Line->busy=FALSE;

			Line->done=FALSE;

			Line->inExecution=FALSE;

			Line->NumOfRightOperands=0;

			memset(Line->Qj,0,LABEL_SIZE);

			memset(Line->Qk,0,LABEL_SIZE);

			Line->Vj=0;

			Line->Vk=0;

			

			

			Line->OPCODE=-1;

			

			

		}

		Line=Line->next;

	}



}



/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/



void AdvanceIntPipeline(){

	int i=0,j=0;

	int length=Configuration->int_delay;		/*how long is the pipeline*/

	IntALU_PipelineStage *last=Integer_ALU_Unit,*NewNode=NULL,*temp=NULL;

	IntReservationStation_Line *Line=IntReservationStation;



	/*get last to point to last stage of pipeline*/

	for (i=1;i<length;i++){

		last=last->next;

	}



		if (last->busy == TRUE){			/*check if stage has an instruction or a bubble that went on*/

		

		/*calculate result of operation. in reality it's done in stages but here it's done in one stage and we decide to do it int last stage*/

		switch(last->OPCODE){

		case ADD:

			last->result=(last->operand1)+(last->operand2);

			break;

		case ADDI:

			last->result=(last->operand1)+(last->operand2);

			break;

		case SUB:

			last->result=(last->operand1)-(last->operand2);

			break;

		case SUBI:

			last->result=(last->operand1)-(last->operand2);

			break;

		default:

			//printf("Error: wrong Opcode in ALU unit\n");

			break;

		}



		/*opearate as CDB and update waiting stations and registers*/



		/*update Integer Reservation satation*/

		for (i=0;i<Configuration->int_nr_reservation;i++){

			if (!strcmp(Line->Qj,last->LabelOfSupplier)){

				Line->Vj=last->result;			/*update waiting value*/

				memset((void*)Line->Qj,0,LABEL_SIZE);	/*reset label so no more unexpected updates occur*/

				Line->NumOfRightOperands++;		/*update number of ready operands*/

			}



			if (!strcmp(Line->Qk,last->LabelOfSupplier)){

				Line->Vk=last->result;			/*update waiting value*/

				memset((void*)Line->Qk,0,LABEL_SIZE);	/*reset label so no more unexpected updates occur*/

				Line->NumOfRightOperands++;		/*update number of ready operands*/

			}



				/*we set reservation station state as done for this instruction*/

			if ( (!strcmp(last->LabelOfSupplier,Line->label)) && (Line->inExecution == TRUE) ){

				Line->done = TRUE;

				for (j=0;j<TRACE_SIZE;j++){

					if (trace[j].issued == Line->issued){

						trace[j].CDB=cycle-1;

						break;

					}

				}

				

			}



			Line=Line->next;

		}



			/*update registers*/

		for (i=0;i<NUM_OF_INT_REGISTERS;i++){

			if ((Integer_Registers[i].busy == TRUE) && (!(strcmp(Integer_Registers[i].label,last->LabelOfSupplier)))){

				

				Integer_Registers[i].value = last->result;			/*update value*/

				Integer_Registers[i].busy = FALSE;					/*no more busy*/

				

				memset(Integer_Registers[i].label,0,LABEL_SIZE);	/*reset label though it is not necessary*/

			}}



		}

	

	/*waiting stations and registers were updated if not a bubble in last stage. Now must advance pipeline one stage foreward*/

	

		

	/*if piepeline length is one then we erase content, set it as not busy and we are done*/

	if (length == 1){

		memset(last,0,sizeof(IntALU_PipelineStage));

		last->busy = FALSE;

		last->next=NULL;

	}

	

	/*if length is greater than 1 then we free last node, create a new one and make it first so that it's the same as advancing each node*/

	

	else {

		free(last);				/*last stage finished executing and updated everybody thus we release it*/

		NewNode=(IntALU_PipelineStage*)malloc(sizeof(IntALU_PipelineStage));

		

		

		memset(NewNode,0,sizeof(IntALU_PipelineStage));

		NewNode->busy=FALSE;			/*first stage not in use right now*/

		NewNode->next=Integer_ALU_Unit;			/*point first node to old first stage of pipeline*/

				

		/*now begining of ALU is the NewNode*/

		

		Integer_ALU_Unit=NewNode;

		

		

	}

			/*we want last stage of pipeline to point to NULL as next stage*/

	temp=Integer_ALU_Unit;				/*point to begining*/

	/*advance till pointing to new last stage (as old last was released) */

	for (i=1;i<length;i++){

		temp=temp->next;		

	}

	temp->next=NULL;



	

}



	



/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/



BOOL SimulateClockCycle_IntUnit(){

	BOOL isInstructionTakenByUnit=FALSE;



	AdvanceIntPipeline();					/*advance piepline*/

	ReservationStationToALU();				/*send new instruction for execution*/

	EvictFromIntReservationStation();		/*evict done instructions from reservation station*/

	if ( InsertToReservationStation() ){			/*insert new instruction to reservation station*/

		isInstructionTakenByUnit=TRUE;

		

	}



	return isInstructionTakenByUnit;

		

}