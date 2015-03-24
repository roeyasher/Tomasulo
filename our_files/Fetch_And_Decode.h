#ifndef FETCH_AND_DECODE
#define FETCH_AND_DECODE

#include "common.h"

typedef struct Instruction Instruction;
struct Instruction {

	int OPCODE;
	int DST;
	int SRC0;
	int SRC1;
	int IMM;
	int PC;
	int numRob;
	char name[16];
	struct Instruction *next;
};

int CharToInteger (char schar);

void BranchAndUpdatePC();
void EmptyInsturcionQueue(int *counter);
void freeInsturcionQueue();
void * GetInstructionFromQUeue();
void IntilaizeInstructionQueue();
void FillTheFields(Instruction *instr_queue);
BOOL HaltAndWrongInstruction(Instruction *temp);
BOOL Decode();
BOOL LinkInstQueue(char instruction_line[], int *instruction_queue_counter, int PC);
BOOL Fetch(char *memory[], int *pc_conter_to_fetch, int * instruction_queue_counter);
Instruction *SearchTheElementInstByPc(Instruction *instruction_queue_head);
Instruction *DeleteTheInstrcutionsDistributor();
BOOL InsertToRS();
#endif



