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

	struct Instruction *next;

	char name[16];
};





int CharToInteger (char schar);

BOOL LinkInstQueue(char instruction_line[], int *instruction_queue_counter);

void CheckTheConditionAndReturnPc();

Instruction *SearchTheElementInstByPc(Instruction *instruction_queue_head);

void FillTheFields(Instruction *instr_queue);

Instruction *DeleteTheInstrcutionsDistributor();

int DecodeAndDistributor(Instruction *instruction_queue_head);

BOOL FetchAndDecode(char *memory[], int *pc_conter_to_fetch, int * instruction_queue_counter);

void simulateclockFetchAndDecode();

void EmptyInsturcionQueue(int *counter);

void freeInsturcionQueue();



VOID * GetInstructionFromQUeue(Instruction *result_instruction);



void IntilaizeInstructionQueue();


#endif



