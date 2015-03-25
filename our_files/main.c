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

Instruction temp_int;
Instruction temp_fp_add;
Instruction temp_fp_mull;
Instruction temp_load;
/*For Branch*/
extern Instruction *Branch_List= NULL;
/*For Rob*/
robLine *robLines = NULL;
/**/

/*For Integer Unit*/
IntegerRegister Integer_Registers[NUM_OF_INT_REGISTERS];
IntReservationStation_Line *IntReservationStation = NULL;
IntALU_PipelineStage *Integer_ALU_Unit = NULL;
/**/

/*For FP Unit*/
FpRegister FP_Registers[NUM_OF_FP_REGISTERS];
FpReservationStation_Line *FpReservationStation_ADD = NULL;
FpReservationStation_Line *FpReservationStation_MUL = NULL;
FP_PipelineStage *FP_executionPipeline_ADD = NULL;
FP_PipelineStage *FP_executionPipeline_MUL = NULL;
/**/

/*for Memory Unit*/
Instruction *my_instruction = NULL;
LoadBuffer *LoadBufferResarvation = NULL;
StoreBuffer *StoreBufferResarvation = NULL;
Memory_PiplineStage *Memory_Unit = NULL;
/**/

/*For Fetch and Decode*/
Instruction *instruction_queue_head = NULL;
int PC = 0;
/**/

/*create registers/memory/traces logs*/
void RegisterLog();
void MemoryLog();
void PrintTrace();

/*read memin file and initialize memory*/
void InitializeMemory(FILE *memin);

/*initialize trace structure*/
void InitializeTrace();

int detectEnd();

BOOL halt_flag = FALSE;
int main(int argc, char* argv[]){

	char *adressMainMemory = MainMemoryArray[0];
	int i = 0,j=0, pc_counter_instruction = 0, instruction_queue_counter = 0;
	BOOL reservation_stations_has_space = TRUE, rob_has_space = TRUE, more_instruction = TRUE,decode_value=TRUE,stop_decode=FALSE;
	instr.OPCODE = -1;


	//intialize all
	InitializeConfiguration(argv[1]);
	InitBuffers();
	MemInToMainMemory(adressMainMemory, argv[2]);
	InitFus();
	instr.num = 0;

	// Cycle Simulation
	while (TRUE){

		//***************************************************************************
		//  Issue
		//***************************************************************************
		if (TRUE != halt_flag){
			// Fetch - Instructions from the main memory ----> to the instruction queue
			while ((more_instruction == TRUE) && (instruction_queue_counter < 16)){
				more_instruction = Fetch(adressMainMemory, &pc_counter_instruction, &instruction_queue_counter);
				pc_counter_instruction++;
			}

			// Decode and check Whether is it the end of the code
			if (FALSE == stop_decode){
				decode_value = Decode(&stop_decode);
			}
			more_instruction = (((int)more_instruction) &((int)decode_value));
			if (TRUE == flag)
			{
				instruction_queue_counter = 0;
				pc_counter_instruction = (PC / 4) + 1;
			}
			else{
				instruction_queue_counter--; 
			}
		}
		if (TRUE != flag)
		{
		

			//***************************************************************************
			// Commit
			//***************************************************************************
			
			CDBUpdateRob();
			CDBUpdateRS();
			commitRob();
			if (TRUE != flag)
			{

			//***************************************************************************
			// Execution
			//***************************************************************************

			//cycle++;
			// Simulate all of the FU's
			SimulateClockCycle_LoadUnit();
			SimulateClockCycle_IntUnit();
			simulateClockCycle_FpUnit();

			//***************************************************************************
			//  CDB
			//***************************************************************************

			CDBControlInt(&temp_int);
			CDBControlFPADD(&temp_fp_add);
			CDBControlFPMULL(&temp_fp_mull);
			CDBControlLoad(&temp_load);

			}
			//***************************************************************************
			//  RS
			//***************************************************************************
			if (!stop_decode)
				// insert to the relevant RS 
				instr_reservation = InsertToRS();
		}
			
			cycle++;
			
		//***************************************************************************
		//***************************************************************************
		
		
		PC += 4; //Clock
		
		InsType = -1;

		if ((flag == TRUE)){	
			instr_reservation = TRUE;
		}

		if (cycle == 200000)
			break;
	
	}

	/*the print function to log files: create 4 logfiles: trace,memorylog,registerlog(X2)*/
	RegisterLog();		
	MemoryLog();		
	PrintTrace();		

	/*close all files*/
	_fcloseall();	/*just in case*/

	freeInsturcionQueue();
	printf("\nEnd of simulation.\n");

	getchar();
	return  0;
}
