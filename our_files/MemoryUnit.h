#include "ConfigurationFile.h"



#ifndef LOAD_BUFFER_H

#define LOAD_BUFFER_H



#include <stdio.h>

#include <stdlib.h>

#include <string.h>



#define MEMORY_SIZE 1024



typedef struct 

{

	int address;

	int OPCODE;

	BOOL busy;

	BOOL inexecution;

	BOOL done;

	int count;

	int checkforexecute;

	char Label[LABEL_SIZE];

	char name[16];

	int issued;

	struct LoadBuffer *next;

}LoadBuffer;



typedef struct

{

	int address;

	int OPCODE;

	BOOL busy;

	BOOL inexecution;

	BOOL done;

	int count;

	int checkforexecute;

	float vj;

	char Qj[LABEL_SIZE];

	int NumOfRightOperands;

	char Label[LABEL_SIZE];

	char name[16];

	int issued;

	struct StoreBuffer *next;

}StoreBuffer;



typedef struct

{

	int OPCODE;

	int address;

	char labelofsupplier[LABEL_SIZE];

	struct Memory_PiplineStage *next;

	float Data_load;

	float Data_store;

}Memory_PiplineStage;



typedef struct

{

	int OPCODE;

	int address;

	char labelofsupplier[LABEL_SIZE];

	float Data_load;

	float Data_store;

}Memory_Pipline;

typedef struct

{

	float value;

	char label[LABEL_SIZE];

}CdbWriteBack;



void IntilaizeMemoryArray();



static LoadBuffer *CreateNewNode();



static StoreBuffer *CreateNewNodeForStore();



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



void EvictFromLoadAndStoreBuffer();



#endif

