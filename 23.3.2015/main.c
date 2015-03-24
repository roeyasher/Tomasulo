#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

/*MainMemory*/
char MainMemoryArray[MEMORY_SIZE][BUFFER_SIZE];

/*Defining global variables*/

Configuration_Data *Configuration = NULL;
Instruction instr;
BOOL instr_reservation = TRUE;
BOOL flag = FALSE;
int cycle = 1;
Trace trace[TRACE_SIZE];

/*For CDB*/
IntCDB IntUnitCDB;
FPCDB FPUnitCDB;
LoadCDB LoadUnitCDB;

IntCDB temp_int;
IntCDB temp_fp;
IntCDB temp_load;

/*For Rob*/
robLine *robLines = NULL;/*load buffer/load reservation*/
/**/

/*For Integer Unit*/
IntegerRegister Integer_Registers[NUM_OF_INT_REGISTERS];/*Integer Registers array*/
IntReservationStation_Line *IntReservationStation = NULL;/*Integer reservation station*/
IntALU_PipelineStage *Integer_ALU_Unit = NULL;/*Integer - ALU pipeline execute unit*/
/**/

/*For FP Unit*/
FpRegister FP_Registers[NUM_OF_FP_REGISTERS];/*FP register array*/
FpReservationStation_Line *FpReservationStation_ADD = NULL;/*FP reservation station- for ADD instr*/
FpReservationStation_Line *FpReservationStation_MUL = NULL;/*FP reservation station - for MUL instr*/
FP_PipelineStage *FP_executionPipeline_ADD = NULL;/*the FP-ADD pipeline execute unit*/
FP_PipelineStage *FP_executionPipeline_MUL = NULL;/*the FP-MUL pipeline execute unit*/
/**/

/*for Memory Unit*/
LoadBuffer *LoadBufferResarvation = NULL;/*load buffer/load reservation*/
float PhysicalMemoryArray[MEMORY_SIZE];/*the memory unit*/
StoreBuffer *StoreBufferResarvation = NULL;/*store buffer/store reservation*/
Memory_PiplineStage *Memory_Unit = NULL;/*the memory pipeline execute unit*/
Memory_Pipline BufferToMemory;/*a variable to helps us in the programm runing*/
CdbWriteBack CdbToResarvation;/*CDB write back*/
/**/

/*For Fetch and Decode*/
Instruction *instruction_queue_head = NULL;
int PC = 0;/*a global parameter - program counter - indicate our location in the program*/
/**/

/*create registers/memory/traces logs*/
void RegisterLog();
void MemoryLog();
void PrintTrace();

/*read memin file and initialize memory*/
void InitializeMemory(FILE *memin);

/*initialize trace structure*/
void InitializeTrace();

/*check if all reservation stations are empty from instructions - used to know when to terminate program after issue of HALT*/
int detectEnd();


int main(int argc, char* argv[]){

	//FILE *memin_txt=fopen(argv[2],"rt");
	//FILE *memin_txt = FileOpen(argv[2], "rt");
	
	char *adressMainMemory = MainMemoryArray[0];
	int i = 0, pc_counter_instruction = 0, instruction_queue_counter = 0;
	BOOL reservation_stations_has_space = TRUE;
	BOOL rob_has_space = TRUE;
	BOOL no_more_instruction = FALSE;
	instr.OPCODE = -1;




	/*intialize all*/

	/*configuration file*/
	InitializeConfiguration(argv[1]);

	//IntilaizeInstructionQueue();
	IntilaizeInstructionQueue();

	/*Memory reservation stations and execution units*/
	IntilaizeRob();
	IntilaizeLoadBuffer();
	IntilaizeStoreBuffer();
	IntializeMemPipline();
	//TODO enable me
	IntilaizeMemoryArray();
	MemInToMainMemory(adressMainMemory, argv[2]);

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
	strcpy(CdbToResarvation.label, "Empty");

	printf("Starting simulator with files <%s> <%s>\n", argv[1], argv[2]);

	/*this loop simulate one cycle by calling the simulateclockcycle function of each unit*/

	while (TRUE)
	{
		// Bring instruction from the main memory to the instruction queue
		//TODO We should add flags for: the reservation station and the rob whether they are free or not, and also to check the counter I did for the instruction_queue_counter
		while ((no_more_instruction == FALSE) && (instruction_queue_counter < 16))
		{
			no_more_instruction = FetchAndDecode(adressMainMemory, &pc_counter_instruction, &instruction_queue_counter);
			pc_counter_instruction++;
		}

		/*brings relevant instruction to instr and updates PC if necessary*/
		/*A simluate clock cycle for fetch and decode unit*/
		DecodeAndDistributor(instruction_queue_head);

		/*init as instruction not taken by any unit*/
		instr_reservation = FALSE;



		// check if the rob have free line -> check if relevant RS have free line ->  interst to rob and the relevat RS  

		/*simulat each unit one clock cycle. if a unit issues instr, update instr_reservation to say so (used for Fetch&Decode unit)*/
		instr_reservation = SimulateClockCycle_IntUnit();
		instr_reservation = simulateClockCycle_FpUnit();
		instr_reservation = SimulateClockCycle_LoadUnit(cycle, 0);

		/*flag==TRUE when instr is BEQ/BNE/JUMP and fetch&decode unit has taken it. in that case instr_reservation should be TRUE*/
		if ((flag == TRUE)){	/*if BEQ/BNE/JMP then flag is set to TRUE. otherwise instruction was not taken by any unit*/
			instr_reservation = TRUE;
		}

		/*simulate till HALT is issued*/
		if (instr.OPCODE == HALT)
			break;


		cycle++;
		//instruction_queue_counter--;

		/*the last fix just to get a trace in the forth test*/
		if (cycle == 200000)
			break;
	}



	/*getting here means all instructions were issued followed by HALT instruction.*/

	instr.OPCODE = -1;	/*not necessary*/ //TODO remove



	/*after isuse of HALT, all instructions are for sure issued to reservation statinos. we must simulate new clock cycles until all reservation stations

	are empty. then the process can terminate.*/



	/* gadi : the second mistake is here and should be ! detectEND competibale to the way we wrote this function*/
	//TODO remove the gadi comment

	/*while(detectEnd()){*/

	while (!detectEnd()){
		SimulateClockCycle_IntUnit();
		simulateClockCycle_FpUnit();
		SimulateClockCycle_LoadUnit(cycle, 1);
		cycle++;
	}

	/*just in case*/
	for (i = 0; i < 10000; i++){
		SimulateClockCycle_IntUnit();
		simulateClockCycle_FpUnit();
		SimulateClockCycle_LoadUnit(cycle, 1);
	}

	/*the print function to log files: create 4 logfiles: trace,memorylog,registerlog(X2)*/
	RegisterLog();		/*creates registers log*/
	MemoryLog();		/*creates memory log*/
	PrintTrace();		/*creates trace log*/

	/*close all files*/
	_fcloseall();	/*just in case*/


		printf("End of simulation. \nOutput files <%s> <%s> <%s> <%s>\n\n", argv[3], argv[4], argv[5], argv[6]);

		freeInsturcionQueue();
	printf("End of simulation. \nOutput files <%s> <%s> <%s> <%s>\n\n", argv[3], argv[4], argv[5], argv[6]);

		getchar();
	return  0;
}

void MemoryLog(){

	FILE *output = NULL;
	int i = 0;
	output = FileOpen("memout.txt", "wt+");

	for (i = 0; i < MEMORY_SIZE; i++)
		fprintf(output, "%.8x\n", *(int*)&PhysicalMemoryArray[i]);

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




