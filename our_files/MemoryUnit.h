#ifndef LOAD_BUFFER_H
#define LOAD_BUFFER_H

#include "common.h"



typedef struct LoadBuffer LoadBuffer;
struct LoadBuffer {
	int address;
	int OPCODE;
	BOOL busy;
	BOOL inexecution;
	BOOL done;
	int count;
	int checkforexecute;
	int buffNum;
	char name[16];
	int issued;
	struct LoadBuffer *next;
};

typedef struct StoreBuffer StoreBuffer;
struct StoreBuffer {
	int address;
	int OPCODE;
	BOOL busy;
	BOOL inexecution;
	BOOL done;
	int count;
	int checkforexecute;
	float vj;
	int numRob; //Qj
	int NumOfRightOperands;
	int buffNum;
	char name[16];
	int issued;
	struct StoreBuffer *next;
};

typedef struct Memory_PiplineStage Memory_PiplineStage;
struct Memory_PiplineStage {
	int OPCODE;
	int address;
	int numOfRobSupplier;
	struct Memory_PiplineStage *next;
	float Data_load;
	float Data_store;
};

typedef struct Memory_Pipline Memory_Pipline;
struct Memory_Pipline {
	int OPCODE;
	int address;
	int numOfRobSupplier;
	float Data_load;
	float Data_store;
};

typedef struct CdbWriteBack CdbWriteBack;
struct CdbWriteBack {
	float value;
	char label[LABEL_SIZE];
};

int ST_Buff_Cnt;
int LD_Buff_Cnt;

void IntilaizeMemoryArray();
LoadBuffer *CreateNewLBNode();
StoreBuffer *CreateNewSBNode();
void IntilaizeLoadBuffer();
void IntilaizeStoreBuffer();
void IntializeMemPipline();
BOOL InsertNewLoadInstr(int count);
BOOL InsertNewStoreInstr(int count);
LoadBuffer *FindTheLoadMinumum(int count);
StoreBuffer *FindTheStoreMinumum(int count);
void AddTheInstrToExecute();
BOOL IsNotTheSameAddress(int address, char Label[],int count);
BOOL BufferToMemoryExecution(int counter);
BOOL SimulateClockCycle_LoadUnit(int count,int flag);
void ExecuteMemoryAndWriteToCdb();
void CdbReturnValue();
void readLine(FILE *file, char *my_string);
void EvictFromLoadAndStoreBuffer();
void MemInToMainMemory(char *memory[], char *memin_txt);
#endif

