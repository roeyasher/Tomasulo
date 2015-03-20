#include "Fetch_And_Decode.h"

#include "ConfigurationFile.h"



#ifndef INT_UNIT_H

#define INT_UNIT_H

#include "common.h"



#define SIZE_OF_CHAR 512

#define TRUE 1

#define FALSE 0

#define NUM_OF_INT_REGISTERS 16

#define LABEL_SIZE 10

#define BUFFER_SIZE 512

#define TRACE_SIZE 10000




typedef int BOOL;





/*Integer registers structure*/

typedef struct{

	int value;

	BOOL busy;						/*indicating if value is relevant */

	char label[LABEL_SIZE];			/*Which Reservation station is in charge of value in case busy is TRUE*/

}IntegerRegister;



/*Integer Reservation Station is a linked list of size 'add_nr_reservation' where each node is an instruction station before being executed*/



typedef struct{

	int OPCODE;

	char label[LABEL_SIZE];					/*this is the label of the line. e.g 'ADD1'*/

	int Vj,Vk;								/*those are the values to operate on*/

	int NumOfRightOperands;					/*we need 2 operands to operate. this must be initialized to 0 and incremented by 1 when

											getting Vj,Vk so that 2 indicates that all operands are ready and intruction shall be executed*/

	char Qj[LABEL_SIZE],Qk[LABEL_SIZE];		/*labels in case of for result from another instruction*/

	BOOL busy;								/*is this line in use*/

	BOOL done;								/*did this line finish execution and can be evicted from Reservation Station*/

	BOOL inExecution;

	struct IntReservationStation_Line *next;	/*next line in reservation station*/

	int issued;

}IntReservationStation_Line;





typedef struct{

	int OPCODE;

	BOOL busy;

	int operand1,operand2;

	char LabelOfSupplier[LABEL_SIZE];

	struct IntALU_PipelineStage *next;

	int result;

}IntALU_PipelineStage;



typedef struct{

	char instruction[16];

	int issued;

	int execution;

	int CDB;

	int valid;

}Trace;





void InitializeIntegerALU();			//Allocation of integer ALU unit 



/*Initialize integer registers to 0 */

void Initialize_IntRegisters();



/*Create the Reservation Station for Int instrucions*/

void InitializeReservationStation();



/*Insert a new instruction. TRUE returned when there is an empty label. FALSE indicates that BUFFER is full and fetch must wait*/

BOOL InsertToReservationStation();  



/*Checks if there is a line ready to be executed and place it in first pipeline stage of execution unit*/

void ReservationStationToALU();



/*removes instruction which are done from reservation station and broadcasts result on CDB for other instruction with RAW hazard*/

void EvictFromIntReservationStation();



/*this function advances the pipeline one stage forward, pops last stage out and update everyone waiting (registers and reservation station)*/

void AdvanceIntPipeline();



/*pipeline advances one stage thus last stage writes if executing an inst and first stage gets a new instr if there is one available	

in reservation stations. moreover, reservation station is updated - done instrs are evicted and new one is placed if exists*/

BOOL SimulateClockCycle_IntUnit();		







#endif