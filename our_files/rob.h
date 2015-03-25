#ifndef ROB_H
#define ROB_H

#include "common.h"

typedef struct robLine robLine;
struct robLine{

	int OPCODE;
	int numRob;				
	int address;
	int intValue;		
	int num;
	float fpValue;
	BOOL busy;								
	BOOL done;								
	BOOL inExecution;
	struct robLine *next;	
};

int robCnt;
// Function Decleration
robLine *CreateRLNewNode();
void updateFPRob(int numRob, float value, int address);
void IntilaizeRob();
BOOL IsRobEmpty();
#endif