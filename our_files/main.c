#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

/*MainMemory*/
char MainMemoryArray[MEMORY_SIZE][BUFFER_SIZE];
//Decode
int InsType;

/*Defining global variables*/

Configuration_Data *Configuration = NULL;
Instruction instr;
BOOL instr_reservation = TRUE;
BOOL flag = FALSE;
int cycle = 1;
Trace trace[TRACE_SIZE];

/*For CDB*/
IntCDB IntUnitCDB;
FPCDB FPUnitCDBADD;
FPCDB FPUnitCDBMULL;
LoadCDB LoadUnitCDB;

IntCDB temp_int;
FPCDB temp_fp_add;
FPCDB temp_fp_mull;
LoadCDB temp_load;

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
Instruction *my_instruction = NULL;
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

	char *adressMainMemory = MainMemoryArray[0];
	int i = 0, pc_counter_instruction = 0, instruction_queue_counter = 0;
	BOOL reservation_stations_has_space = TRUE, rob_has_space = TRUE, no_more_instruction = FALSE;
	instr.OPCODE = -1;


	//intialize all
	InitializeConfiguration(argv[1]);
	InitBuffers();
	MemInToMainMemory(adressMainMemory, argv[2]);
	InitFus();
	strcpy(CdbToResarvation.label, "Empty");

	//***************************************************************************TODO-delete
	//General Order:
	//***************************************************************************
	//1. Issue
	// Instruction to main memory
	// Updae from CDB to RS's
	// RS
	//2. Execution
	// Run Fus
	//3. CDB
	//4. Commit
	//***************************************************************************
	//***************************************************************************

	// Cycle Simulation
	while (TRUE){

		//***************************************************************************
		//1. Issue
		//***************************************************************************

		// Fetch - Instructions from the main memory ----> to the instruction queue
		while ((no_more_instruction == FALSE) && (instruction_queue_counter < 16)){
			no_more_instruction = Fetch(adressMainMemory, &pc_counter_instruction, &instruction_queue_counter);
			pc_counter_instruction++;
		}

		// Decode and chechk Whether is it the end of the code
		if((no_more_instruction = Decode())) { break; };
		instruction_queue_counter--; //Shoud we put it somewhere else? (Roey)

		// insert to the relevant RS 
		instr_reservation = InsertToRS();;

		// update the system from the CDB
		CDBUpdateRob();
		CDBUpdateRS();


		//***************************************************************************
		//1. Execution
		//***************************************************************************
		
		// Simulate all of the FU's
		SimulateClockCycle_LoadUnit(); /// what about store?!?! (Roey)
		SimulateClockCycle_IntUnit();
		simulateClockCycle_FpUnit();

		// TODO not sure if we need this condition over here (Roey);
		if (TRUE == no_more_instruction) { break; }

		//***************************************************************************
		//1. CDB
		//***************************************************************************

		//TODO pass the CDB function the right values

		CDBControlInt(&temp_int);
		CDBControlFPADD(&temp_fp_add);
		CDBControlFPMULL(&temp_fp_mull);
		CDBControlLoad(&temp_load);

		//***************************************************************************
		//1. Commit
		//***************************************************************************

		//TODO add relevant function
		commitRob();

		//***************************************************************************
		//***************************************************************************

		PC += 4; //Clock
		InsType = -1;

		/*flag==TRUE when instr is BEQ/BNE/JUMP and fetch&decode unit has taken it. in that case instr_reservation should be TRUE*/
		if ((flag == TRUE)){	/*if BEQ/BNE/JMP then flag is set to TRUE. otherwise instruction was not taken by any unit*/
			instr_reservation = TRUE;
		}

		cycle++;
		//instruction_queue_counter--;

		// are we limited to number of cycle?? mybe need to delet this one (Roey);
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







