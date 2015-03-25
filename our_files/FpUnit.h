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

/*FP Reservation station line.*/
typedef struct FpReservationStation_Line FpReservationStation_Line;
struct FpReservationStation_Line{
	char name[BUFFER_SIZE];
	int OPCODE;
	int robNum;
	float Vj,Vk;
	int Qj,Qk;
	int NumOfRightOperands;
	BOOL busy;
	BOOL done;
	BOOL inExecution;
	struct FpReservationStation_Line *next;
	int num;
};

/*Pipeline stages of FP Unit*/
typedef struct FP_PipelineStage FP_PipelineStage;
struct FP_PipelineStage{
	char name[BUFFER_SIZE];
	int OPCODE;
	int num;
	BOOL busy;
	float operand1,operand2;
	float result;
	int numOfSupplier;
	struct FP_PipelineStage *next;
};

int FP_RS_ADD_Cnt;
int FP_RS_MULL_Cnt;


void Initialize_FpRegisters();
void Initialize_FpReservationStations();
void Initialize_FpPipelines();
BOOL FP_InsertToReservationStations_ADD();
BOOL FP_InsertToReservationStations_MUL();
void FP_ReservationStationToExecution();
void FP_EvictFromReservationStation();
void FP_AdvanceFpPipeline_ADD();
void FP_AdvanceFpPipeline_MUL();
void simulateClockCycle_FpUnit();
BOOL UpdateResultInRS();
BOOL isFP_RS_ADD_empty();
BOOL isFP_RS_MULL_empty();
#endif