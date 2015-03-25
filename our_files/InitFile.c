#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

void InitBuffers(){
	//IntilaizeInstructionQueue();
	IntilaizeInstructionQueue();

	/*Memory reservation stations and execution units*/
	IntilaizeRob();
	IntilaizeLoadBuffer();
	IntilaizeStoreBuffer();
	IntializeMemPipline();
	//TODO enable me
	IntilaizeMemoryArray();
}
void InitFus(){
	/*INT reservation stations and execution units*/
	InitializeIntegerALU();
	Initialize_IntRegisters();
	InitializeReservationStation();

	/*FP reservation stations and execution units*/
	Initialize_FpRegisters();
	Initialize_FpReservationStations();
	Initialize_FpPipelines();

	/*fetch and decode unit to insert the instructions to the instructions queue DB*/

	/*initialize structure for trace file*/
	InitializeTrace();
	/**/
}


// TODO Myabe to Move from here
void MemoryLog(){

	FILE *output = NULL;
	int i = 0;
	output = FileOpen("memout.txt", "wt+");

	for (i = 0; i < MEMORY_SIZE; i++)
		fprintf(output, "%.8x\n", *(int*)&MainMemoryArray[i]);

	fflush(output);
	fclose(output);
}


int detectEnd(){

	int i = 0;
	IntReservationStation_Line *temp1 = IntReservationStation;
	FpReservationStation_Line *temp2 = FpReservationStation_ADD;
	FpReservationStation_Line *temp3 = FpReservationStation_MUL;
	LoadBuffer *temp4 = LoadBufferResarvation;
	StoreBuffer *temp5 = StoreBufferResarvation;

	for (i = 0; i < Configuration->int_nr_reservation; i++){
		if (temp1->busy == TRUE)
			return 0;
		temp1 = temp1->next;
	}

	for (i = 0; i < Configuration->add_nr_reservation; i++){
		if (temp2->busy == TRUE)
			return 0;
		temp2 = temp2->next;
	}

	for (i = 0; i < Configuration->mul_nr_reservation; i++){
		if (temp3->busy == TRUE)
			return 0;
		temp3 = temp3->next;
	}

	for (i = 0; i < Configuration->mem_nr_load_buffers; i++){
		if (temp4->busy == TRUE)
			return 0;
		temp4 = temp4->next;
	}

	for (i = 0; i < Configuration->mem_nr_store_buffers; i++){
		if (temp5->busy == TRUE)
			return 0;
		temp5 = temp5->next;
	}
	return 1;
}
BOOL DoesRobAndRSEmpty(){
	if ((FALSE == isINT_RS_empty()) || (FALSE == isFP_RS_ADD_empty()) || (FALSE == isFP_RS_MULL_empty()) || (FALSE == IsRobEmpty()) || (FALSE == isLD_Buff_emepty()) || (FALSE == isST_Buff_empety()))
	{
		return FALSE;
	}
	return TRUE;
}