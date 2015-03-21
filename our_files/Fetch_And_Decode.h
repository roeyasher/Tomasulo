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

Instruction *LinkInstQueue (char instruction_line[],int pc_counter);

void CheckTheConditionAndReturnPc();

Instruction *SearchTheElementInstByPc(Instruction *instruction_queue_head);

void FillTheFields(Instruction *instr_queue);

Instruction *DeleteTheInstrcutionsDistributor();

int DecodeAndDistributor(Instruction *instruction_queue_head);

void InitializeFetchAndDecode(FILE *memin_txt);

void simulateclockFetchAndDecode();



#endif



