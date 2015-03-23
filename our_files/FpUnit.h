#ifndef FP_UNIT_H
#define FP_UNIT_H

#include "common.h"

/*FP Registers structure*/

typedef struct FpRegister FpRegister;
struct FpRegister {

	float value;
	BOOL busy;
	int robNum;
};

/*FP Reservation station line. the reservation station will be a linked list of size according to configuration file*/
typedef struct FpReservationStation_Line FpReservationStation_Line;
struct FpReservationStation_Line{
	int OPCODE;
	int robNum;
	float Vj,Vk;
	char Qj[LABEL_SIZE],Qk[LABEL_SIZE];
	int NumOfRightOperands;
	BOOL busy;
	BOOL done;
	BOOL inExecution;
	struct FpReservationStation_Line *next;
	int issued;
};

/*Pipeline stages of FP Unit*/
typedef struct FP_PipelineStage FP_PipelineStage;
struct FP_PipelineStage{

	int OPCODE;
	BOOL busy;
	float operand1,operand2;
	float result;
	int numOfSupplier;
	struct FP_PipelineStage *next;
};

int FP_RS_ADD_Cnt;
int FP_RS_MULL_Cnt;

/*Initialize FP registers*/
void Initialize_FpRegisters();

/*Create Reservation stations for MUL and ADD/SUB of FP*/
void Initialize_FpReservationStations();

/*Create Pipeline for executing FP operations. there are 2 pipelines: one for ADDS/SUBS and a second for MULTS*/
void Initialize_FpPipelines();

/*Insert next instruction to one of two reservation stations of FP if it's ADDS/SUBS/MULTS*/
BOOL FP_InsertToReservationStations_ADD();
BOOL FP_InsertToReservationStations_MUL();

/*send instructions from both FP reservation stations to execution units*/
void FP_ReservationStationToExecution();

/*Remove Instructions from reservation stations after they finished executing and wrote result to CDB*/
void FP_EvictFromReservationStation();

/*advance pipelines one stage forward each clock cycle. last stage writes to CDB if needed*/
void FP_AdvanceFpPipeline_ADD();
void FP_AdvanceFpPipeline_MUL();

/*simulate one clock cycle: advance pipeline, write to CDB if needed, evict done instruction from reservation station and
put new ones if possible*/
BOOL simulateClockCycle_FpUnit();
BOOL UpdateResultInRS();
#endif