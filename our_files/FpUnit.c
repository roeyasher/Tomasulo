#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"


void Initialize_FpRegisters(){

	int i=0;

	for (i=0;i<NUM_OF_FP_REGISTERS;i++){
		FP_Registers[i].value=(float)i;
		FP_Registers[i].busy=FALSE;
		memset(FP_Registers[i].label,0,LABEL_SIZE);
	}

}

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
	sprintf(FpReservationStation_ADD->label,"FPADD%d",i+1);
	node = FpReservationStation_ADD;

	for (i=1;i<lengthAdd;i++){
		node->next = CreateNewFPRSNode();
		node = node->next;
		sprintf(node->label,"FPADD%d",i+1);
	}


	/*Create Reservation station for MUL unit*/
	i=0;
	FpReservationStation_MUL=CreateNewFPRSNode();
	sprintf(FpReservationStation_MUL->label,"FPMUL%d",i+1);
	node = FpReservationStation_MUL;

	for (i=1;i<lengthMUL;i++){
		node->next = CreateNewFPRSNode();
		node = node->next;
		sprintf(node->label,"FPMUL%d",i+1);
	}
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
	while (line->next != NULL){
		if (line->done == TRUE){
			line->busy=FALSE;
			line->done=FALSE;
			line->inExecution=FALSE;
			line->NumOfRightOperands=0;
			memset(line->Qj,0,LABEL_SIZE);
			memset(line->Qk,0,LABEL_SIZE);
		}
		line=line->next;
	}

	/*evict for MUL reservation station*/
	line=FpReservationStation_MUL;

	while (line->next != NULL){
		if (line->done == TRUE){
			line->busy=FALSE;
			line->done=FALSE;
			line->inExecution=FALSE;
			line->NumOfRightOperands=0;
			memset(line->Qj,0,LABEL_SIZE);
			memset(line->Qk,0,LABEL_SIZE);
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
	while (line->next != NULL){

		if ((line->NumOfRightOperands == 2) && (line->inExecution==FALSE)){
			FP_executionPipeline_ADD->busy=TRUE;
			FP_executionPipeline_ADD->OPCODE=line->OPCODE;
			FP_executionPipeline_ADD->operand1=line->Vj;
			FP_executionPipeline_ADD->operand2=line->Vk;
			strcpy(FP_executionPipeline_ADD->LabelOfSupplier,line->label);
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
		while (line->next != NULL){

			if ((line->NumOfRightOperands == 2) && (line->inExecution==FALSE)){
				FP_executionPipeline_MUL->busy=TRUE;
				FP_executionPipeline_MUL->OPCODE=line->OPCODE;
				FP_executionPipeline_MUL->operand1=line->Vj;
				FP_executionPipeline_MUL->operand2=line->Vk;
				strcpy(FP_executionPipeline_MUL->LabelOfSupplier,line->label);
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
	while (iter->next != NULL){

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
	if(instr.OPCODE==9 || instr.OPCODE==10)
		available->OPCODE = instr.OPCODE;
	else
		return FALSE;

	/*operand j*/
	if (FP_Registers[instr.SRC0].busy == FALSE){
		available->Vj = FP_Registers[instr.SRC0].value;
		available->NumOfRightOperands++;			/*operand j is ready*/
	}
	else{
		strcpy(available->Qj,FP_Registers[instr.SRC0].label);	/*copy label of supplier if register is busy*/
	}

	/*operand k. in FP ops there are no immediate*/
	if (FP_Registers[instr.SRC1].busy == FALSE){
		available->Vk = FP_Registers[instr.SRC1].value;
		available->NumOfRightOperands++;			/*operand j is ready*/
	}
	else{
		strcpy(available->Qk,FP_Registers[instr.SRC1].label);	/*copy label of supplier if register is busy*/
	}

	/*update destination register*/
	FP_Registers[instr.DST].busy=TRUE;
	memset(FP_Registers[instr.DST].label,0,LABEL_SIZE);
	strcpy(FP_Registers[instr.DST].label,available->label);

	available->busy=TRUE;
	available->done=FALSE;
	available->inExecution=FALSE;

	available->issued=cycle;
	trace[cycle].issued=cycle;
	trace[cycle].valid=TRUE;
	strcpy(trace[cycle].instruction,instr.name);

	return TRUE;
}

BOOL FP_InsertToReservationStations_MUL(){


	FpReservationStation_Line *available=NULL,*iter=FpReservationStation_MUL;

	int lengthMUL=Configuration->mul_nr_reservation;

	int i=0;

	



	/*find an available line in reseravation station*/

	

	for (i=0;i<lengthMUL;i++){

		if (iter->busy==FALSE){

			available=iter;

			break;

		}

		iter=iter->next;

	}



	/*case no available lines in reservation station*/

	if (available == NULL){

		return FALSE;

	}



	/*if there is a line available, check if instruction is mine. if so, take it to reseervation station*/

	

	switch (instr.OPCODE){

	case MULTS:

		available->OPCODE=MULTS;

		break;

	default:

		return FALSE;				/*not my responsability*/

		break;

	}



	/*operand j*/

	if (FP_Registers[instr.SRC0].busy == FALSE){

		available->Vj = FP_Registers[instr.SRC0].value;

		available->NumOfRightOperands++;			/*operand j is ready*/

		

	}

	else{

		strcpy(available->Qj,FP_Registers[instr.SRC0].label);	/*copy label of supplier if register is busy*/

	}



	/*operand k. in FP ops there are no immediate*/

	if (FP_Registers[instr.SRC1].busy == FALSE){

		available->Vk = FP_Registers[instr.SRC1].value;

		available->NumOfRightOperands++;			/*operand j is ready*/

	}

	else{

		strcpy(available->Qk,FP_Registers[instr.SRC1].label);	/*copy label of supplier if register is busy*/

	}



	/*update destination register*/

	FP_Registers[instr.DST].busy=TRUE;

	memset(FP_Registers[instr.DST].label,0,LABEL_SIZE);

	strcpy(FP_Registers[instr.DST].label,available->label);



	available->busy=TRUE;

	available->done=FALSE;

	available->inExecution=FALSE;



	available->issued=cycle;

	trace[cycle].issued=cycle;

	trace[cycle].valid=TRUE;

	strcpy(trace[cycle].instruction,instr.name);



	return TRUE;



}

/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/





void FP_AdvanceFpPipeline_ADD(){

	int j;

	int lengthADD=Configuration->add_delay;

	FP_PipelineStage *last=NULL,*temp=NULL,*NewNode=NULL;

	FpReservationStation_Line *Line=NULL;

	StoreBuffer *Line2=NULL;

	int i=0;



	/*start with ADD unit*/

	/*get last to point to last stage of pipeline*/

	last=FP_executionPipeline_ADD;

	for (i=1;i<lengthADD;i++){

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

			//printf("Error: wrong Opcode in FP_ADD execution unit\n");

			break;

		}



		/*operate as CDB and update reservation station lines waiting for result and FP register file*/

		/*update reservation stations. first ADD*/

		Line=FpReservationStation_ADD;

		for (i=0;i<Configuration->add_nr_reservation;i++){

			/*update ADD reservation station*/

			if (!strcmp(Line->Qj,last->LabelOfSupplier)){

				Line->Vj=last->result;

				memset((void*)Line->Qj,0,LABEL_SIZE);

				Line->NumOfRightOperands++;

			}



			if (!strcmp(Line->Qk,last->LabelOfSupplier)){

				Line->Vk=last->result;

				memset((void*)Line->Qk,0,LABEL_SIZE);

				Line->NumOfRightOperands++;

			}



			/*update instruction in reservation station as done*/

			if (!strcmp(last->LabelOfSupplier,Line->label)){

				Line->done=TRUE;

					for (j=0;j<TRACE_SIZE;j++){

				if (trace[j].issued == Line->issued)

					break;

			}

				trace[j].CDB=cycle-1;

			}

			

			Line=Line->next;

		}



		/*update MUL reservation station*/

		Line=FpReservationStation_MUL;

		for (i=0;i<Configuration->mul_nr_reservation;i++){

			/*update ADD reservation station*/

			if (!strcmp(Line->Qj,last->LabelOfSupplier)){

				Line->Vj=last->result;

				memset((void*)Line->Qj,0,LABEL_SIZE);

				Line->NumOfRightOperands++;

			}



			if (!strcmp(Line->Qk,last->LabelOfSupplier)){

				Line->Vk=last->result;

				memset((void*)Line->Qk,0,LABEL_SIZE);

				Line->NumOfRightOperands++;

			}



			/*impossible for instruction to come from MUL as it's in ADD piepline*/		

			Line=Line->next;

		}



			/*update store buffers*/

			Line2=StoreBufferResarvation;

			for (i=0;i<Configuration->mem_nr_store_buffers;i++){

				if (!strcmp(Line2->Qj,last->LabelOfSupplier)){

					Line2->vj=last->result;

					memset((void*)Line2->Qj,0,LABEL_SIZE);

					Line2->NumOfRightOperands++;

				}

				Line2=Line2->next;

			}



		/*update registers*/

		for (i=0;i<NUM_OF_FP_REGISTERS;i++){

			if ((FP_Registers[i].busy==TRUE) && (!(strcmp(FP_Registers[i].label,last->LabelOfSupplier)))){

				FP_Registers[i].value=last->result;

				FP_Registers[i].busy=FALSE;

				memset(FP_Registers[i].label,0,LABEL_SIZE);

			}}

	}

			

		/*advance ADD pipeline one stage forward*/	

			

			if (lengthADD==1){

				memset(last,0,sizeof(FP_PipelineStage));

				last->busy = FALSE;

				last->next=NULL;

			}

			else{

				/*free last stage. create new one and point to start*/

				free(last);

				NewNode=(FP_PipelineStage*)malloc(sizeof(FP_PipelineStage));

				memset(NewNode,0,sizeof(FP_PipelineStage));

				NewNode->busy=FALSE;

				NewNode->next=FP_executionPipeline_ADD;

				FP_executionPipeline_ADD=NewNode;

				

			}



			/*make last (new) stage point to NULL*/

			temp=FP_executionPipeline_ADD;

			for (i=1;i<lengthADD;i++){

				temp=temp->next;

			}

			temp->next=NULL;

	

	

	

}



void FP_AdvanceFpPipeline_MUL(){

	int j;

	int lengthMUL=Configuration->mul_delay;

	FP_PipelineStage *last=NULL,*temp=NULL,*NewNode=NULL;

	FpReservationStation_Line *Line=NULL;

	StoreBuffer *Line2=NULL;

	int i=0;



		/*Now advance MUL pipeline and update waiting instructions in reservation stations and registers*/

		/*get last to point to last stage of pipeline*/

		last=FP_executionPipeline_MUL;

		for (i=1;i<lengthMUL;i++){

			last=last->next;

		}

	

		/*calculate result value*/

		if (last->busy == TRUE){

			switch(last->OPCODE){

			case MULTS:

				last->result=(last->operand1)*(last->operand2);

				break;

			default:

				//printf("Error: wrong Opcode in FP_MUL execution unit\n");

				break;

			}



			/*operate as CDB and update reservation station lines waiting for result and FP register file*/

			/*update reservation stations. first ADD then MUL*/

			Line=FpReservationStation_ADD;

			for (i=0;i<Configuration->add_nr_reservation;i++){

				if (!strcmp(Line->Qj,last->LabelOfSupplier)){

					Line->Vj=last->result;

					memset((void*)Line->Qj,0,LABEL_SIZE);

					Line->NumOfRightOperands++;

				}



				if (!strcmp(Line->Qk,last->LabelOfSupplier)){

					Line->Vk=last->result;

					memset((void*)Line->Qk,0,LABEL_SIZE);

					Line->NumOfRightOperands++;

				}



				/*impossible for instruction to come from ADD reservation station as it's in MUL pipeline*/			

				Line=Line->next;

			}



			/*update MUL reservation station*/

			Line=FpReservationStation_MUL;

			for (i=0;i<Configuration->mul_nr_reservation;i++){

				if (!strcmp(Line->Qj,last->LabelOfSupplier)){

					Line->Vj=last->result;

					memset((void*)Line->Qj,0,LABEL_SIZE);

					Line->NumOfRightOperands++;

				}



				if (!strcmp(Line->Qk,last->LabelOfSupplier)){

					Line->Vk=last->result;

					memset((void*)Line->Qk,0,LABEL_SIZE);

					Line->NumOfRightOperands++;

				}



				/*update instruction in reservation station as done*/

				if (!strcmp(last->LabelOfSupplier,Line->label)){

					Line->done=TRUE;

					for (j=0;j<TRACE_SIZE;j++){

						if (trace[j].issued == Line->issued)

							break;

						}

						trace[j].CDB=cycle-1;

				}	

				Line=Line->next;

			}



			/*update store buffers*/

			Line2=StoreBufferResarvation;

			for (i=0;i<Configuration->mem_nr_store_buffers;i++){

				if (!strcmp(Line2->Qj,last->LabelOfSupplier)){

					Line2->vj=last->result;

					memset((void*)Line2->Qj,0,LABEL_SIZE);

					Line2->NumOfRightOperands++;

				}

				Line2=Line2->next;

			}



			/*update registers*/

			for (i=0;i<NUM_OF_FP_REGISTERS;i++){

				if ((FP_Registers[i].busy==TRUE) && (!(strcmp(FP_Registers[i].label,last->LabelOfSupplier)))){

					FP_Registers[i].value=last->result;

					FP_Registers[i].busy=FALSE;

					memset(FP_Registers[i].label,0,LABEL_SIZE);

				}

			}}



		/*Now advance MUL pipeline as well*/

			

			if (lengthMUL==1){

				memset(last,0,sizeof(FP_PipelineStage));

				last->busy = FALSE;

				last->next=NULL;

			}

			else{

				/*free last stage. create new one and point to start*/

				free(last);

				NewNode=(FP_PipelineStage*)malloc(sizeof(FP_PipelineStage));

				memset(NewNode,0,sizeof(FP_PipelineStage));

				NewNode->busy=FALSE;

				NewNode->next=FP_executionPipeline_MUL;

				FP_executionPipeline_MUL=NewNode;

			}



			/*make last (new) stage point to NULL*/

			temp=FP_executionPipeline_MUL;

			for (i=1;i<lengthMUL;i++){

				temp=temp->next;

			}

			temp->next=NULL;

	}



/*OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo*/



BOOL simulateClockCycle_FpUnit(){

	BOOL isInstructionTakenByUnit=FALSE;



	FP_AdvanceFpPipeline_ADD();

	FP_AdvanceFpPipeline_MUL();



	FP_ReservationStationToExecution();

	FP_EvictFromReservationStation();



	if (FP_InsertToReservationStations_ADD()){

		isInstructionTakenByUnit=TRUE;

	}

	if(FP_InsertToReservationStations_MUL()){

		isInstructionTakenByUnit=TRUE;

	}



	return isInstructionTakenByUnit;



}





