#ifndef LOAD_BUFFER_H
#define LOAD_BUFFER_H

#include "common.h"



typedef struct LoadBuffer LoadBuffer;
struct LoadBuffer {
	int address;
	int OPCODE;
	BOOL busy;
	BOOL inExecution;
	BOOL done;
	BOOL addressReady;
	int DST;
	int robNum;
	int issued;
	struct LoadBuffer *next;
};

typedef struct StoreBuffer StoreBuffer;
struct StoreBuffer {
	int address;
	int OPCODE;
	BOOL busy;
	BOOL inExecution;
	BOOL done;
	BOOL addressReady;
	float Vj;
	int Qj; //rob num
	int NumOfRightOperands;
	int robNum;
	int issued;
	struct StoreBuffer *next;
};

typedef struct Memory_PiplineStage Memory_PiplineStage;
struct Memory_PiplineStage {
	int OPCODE;
	int address;
	int issued;
	int DST;
	int SRC1;
	int numOfRobSupplier;
	struct Memory_PiplineStage *next;
	float  Data_load;
	float Data_store;
};

int ST_Buff_Cnt;
int LD_Buff_Cnt;

void IntilaizeMemoryArray();
LoadBuffer *CreateNewLBNode();
StoreBuffer *CreateNewSBNode();
void IntilaizeLoadBuffer();
void IntilaizeStoreBuffer();
void IntializeMemPipline();
BOOL InsertNewLoadInstr();
BOOL InsertNewStoreInstr();
LoadBuffer *FindTheLoadMinumum(int count);
StoreBuffer *FindTheStoreMinumum(int count);
void AddTheInstrToExecute();
BOOL IsNotTheSameAddress(int address, char Label[],int count);
BOOL BufferToMemoryExecution(int counter);
void SimulateClockCycle_LoadUnit();
void ExecuteMemoryAndWriteToCdb();
void CdbReturnValue();
void readLine(FILE *file, char *my_string);
void EvictFromLoadAndStoreBuffer();
void MemInToMainMemory(char *memory[], char *memin_txt);
BOOL isLD_Buff_emepty();
BOOL isST_Buff_empety();
#endif

