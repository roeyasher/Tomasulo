#ifndef ROB_H
#define ROB_H

#include "common.h"

typedef struct robLine robLine;
struct robLine{

	int OPCODE;
	int numRob;					/*this is the label of the line.  start form 0... -1 indicate without rob num*/
	int state;		
	int dest;
	int intValue;			//TODO check if its ok!
	int fpValue;
	BOOL busy;								/*is this line in use*/
	BOOL done;								/*did this line finish execution and can be evicted from Reservation Station*/
	BOOL inExecution;

	struct robLine *next;	/*next line in reservation station*/
};

int robCnt;

// Function Decleration
robLine *CreateRLNewNode();
void IntilaizeRob();
BOOL IsRobEmpty();
#endif