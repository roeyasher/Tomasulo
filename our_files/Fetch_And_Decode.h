#ifndef FETCH_AND_DECODE
#define FETCH_AND_DECODE

#include "common.h"

typedef struct Instruction Instruction;
struct Instruction {

	int OPCODE;
	int num;
	float operand1, operand2;
	int numOfSupplier;
	int DST;
	int SRC0;
	int SRC1;
	int IMM;
	int PC;
	int numOfRobSupplier;
	int numRob;
	char name[16];
	BOOL STLDIns;
	BOOL busy;
	float result;
	struct Instruction *next;
};

int CharToInteger (char schar);


void EmptyInsturcionQueue(int *counter);
void freeInsturcionQueue();
void * GetInstructionFromQUeue(int *condition);
void IntilaizeInstructionQueue();
BOOL HaltAndWrongInstruction(Instruction *temp);
BOOL Decode();
BOOL LinkInstQueue(char instruction_line[], int *instruction_queue_counter, int PC);
BOOL Fetch(char *memory[], int *pc_conter_to_fetch, int * instruction_queue_counter);
BOOL InsertToRS();

#endif



