

#include "ConfigurationFile.h"



#ifndef FP_UNIT_H

#define FP_UNIT_H



#include <stdio.h>

#include <stdlib.h>

#include <string.h>



#define SIZE_OF_CHAR 512

#define TRUE 1

#define FALSE 0

#define NUM_OF_FP_REGISTERS 16

#define LABEL_SIZE 10

#define BUFFER_SIZE 512



#define LD 0

#define ST 1

#define JUMP 2

#define BEQ 3

#define BNE 4

#define ADD 5

#define ADDI 6

#define SUB 7

#define SUBI 8

#define ADDS 9

#define SUBS 10

#define MULTS 11 

#define HALT 12



typedef int BOOL;



/*FP Registers structure*/

typedef struct{

	float value;

	BOOL busy;

	char label[LABEL_SIZE];

}FpRegister;



/*FP Reservation station line. the reservation station will be a linked list of size according to configuration file*/

typedef struct{

	int OPCODE;

	char label[LABEL_SIZE];

	float Vj,Vk;

	char Qj[LABEL_SIZE],Qk[LABEL_SIZE];



	int NumOfRightOperands;

	BOOL busy;

	BOOL done;

	BOOL inExecution;

	struct FpReservationStation_Line *next;

	int issued;

}FpReservationStation_Line;



/*Pipeline stages of FP Unit*/

typedef struct{

	int OPCODE;

	BOOL busy;

	float operand1,operand2;

	float result;

	char LabelOfSupplier[LABEL_SIZE];

	struct FP_PipelineStage *next;

	

}FP_PipelineStage;



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





#endif