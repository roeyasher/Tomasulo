#include "common.h"
#include "ConfigurationFile.h"
#include "Fetch_And_Decode.h"
#include "FpUnit.h"
#include "IntegerUnit.h"
#include "MemoryUnit.h"
#include "rob.h"  
#include "common.h"
#include "CDBFile.h"

/*Memory*/
extern char MainMemoryArray[MEMORY_SIZE][BUFFER_SIZE];

/*add global variables*/

extern Configuration_Data *Configuration;

extern Instruction instr;

extern BOOL instr_reservation;

extern BOOL flag;


/*For Rob*/

extern robLine *robLines;

/**/


/*For Integer Unit*/

extern IntegerRegister Integer_Registers [NUM_OF_INT_REGISTERS];

extern IntReservationStation_Line *IntReservationStation;

extern IntALU_PipelineStage *Integer_ALU_Unit;

/**/



/*For FP Unit*/

extern FpRegister FP_Registers[NUM_OF_FP_REGISTERS];

extern FpReservationStation_Line *FpReservationStation_ADD;

extern FpReservationStation_Line *FpReservationStation_MUL;

extern FP_PipelineStage *FP_executionPipeline_ADD;

extern FP_PipelineStage *FP_executionPipeline_MUL;



/*For fetch and decode*/

extern Instruction *instruction_queue_head;

extern int PC;

/**/


extern Memory_PiplineStage *Memory_Unit;

extern LoadBuffer *LoadBufferResarvation;

extern StoreBuffer *StoreBufferResarvation;



extern int cycle;

extern Trace trace[TRACE_SIZE];

extern float PhysicalMemoryArray[];


/*For CDB*/

extern IntCDB IntUnitCDB;
extern FPCDB FPUnitCDB;
extern LoadCDB LoadUnitCDB;

extern IntCDB temp_int;
extern IntCDB temp_fp;
extern IntCDB temp_load;



