#ifndef INT_UNIT_H
#define INT_UNIT_H
#include "common.h"

/*Integer registers structure*/
typedef struct IntegerRegister IntegerRegister;
struct IntegerRegister{
	int value;
	int robNum;
	BOOL busy;				
	
};
/*Integer Reservation Station*/
typedef struct IntReservationStation_Line IntReservationStation_Line;
struct IntReservationStation_Line{
	char name[BUFFER_SIZE];
	int OPCODE;
	int num;
	int robNum;		
	int Vj,Vk;								
	int NumOfRightOperands;													
	int Qj,Qk;							
	BOOL busy;						
	BOOL done;							
	BOOL inExecution;
	struct IntReservationStation_Line *next;	
	
};
typedef struct IntALU_PipelineStage IntALU_PipelineStage;
struct IntALU_PipelineStage{
	char name[BUFFER_SIZE];
	int OPCODE;
	BOOL busy;
	int num;
	int operand1,operand2;
	int numOfRobSupplier;
	struct IntALU_PipelineStage *next;
	int result;
};
typedef struct Trace Trace;
struct Trace{
	char instruction[16];
	int issued;
	int execution_start;
	int CDB;
	int commit;
};

int Int_RS_Cnt;
void InitializeIntegerALU();
void Initialize_IntRegisters();
void InitializeReservationStation();
BOOL InsertToReservationStation();  
void ReservationStationToALU();
void EvictFromIntReservationStation();
void AdvanceIntPipeline();
void SimulateClockCycle_IntUnit();		
BOOL isINT_RS_empty();

#endif