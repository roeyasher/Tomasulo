#ifndef FETCH_AND_DECODE

#define FETCH_AND_DECODE



#include "common.h"

#include "ConfigurationFile.h"

#include "IntegerUnit.h"

#include "FpUnit.h"







#define SIZE_OF_CHAR 512

#define TRUE 1

#define FALSE 0

#define NUM_OF_INT_REGISTERS 16

#define LABEL_SIZE 10

#define BUFFER_SIZE 512



typedef int BOOL;




typedef struct Instruction {

	int OPCODE;

	int DST;

	int SRC0;

	int SRC1;

	int IMM;

	int PC;

	struct Instruction *next;

	char name[16];

}Instruction;





int CharToInteger (char schar);

Instruction *LinkInstQueue (char instruction_line[]);

void CheckTheConditionAndReturnPc();

Instruction *SearchTheElementInstByPc(Instruction *instr_queue,int pc);

void FillTheFields(Instruction *instr_queue);

Instruction *DeleteTheInstrcutionsDistributor();

int DecodeAndDistributor(Instruction *instruction_queue_head);

void InitializeFetchAndDecode(FILE *memin_txt);

void simulateclockFetchAndDecode();



#endif



