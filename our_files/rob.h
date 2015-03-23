#ifndef ROB_H
#define ROB_H

#include "common.h"

typedef struct robLine robLine;
struct robLine{

	int OPCODE;
	char label[LABEL_SIZE];					/*this is the label of the line. e.g 'ADD1'*/
	int state;
	int dest;
	int value;			//TODO check if its ok!
	BOOL busy;								/*is this line in use*/
	BOOL done;								/*did this line finish execution and can be evicted from Reservation Station*/
	BOOL inExecution;

	struct robLine *next;	/*next line in reservation station*/
};

int robCnt;

// Function Decleration
robLine *CreateRLNewNode();
void IntilaizeRob();

#endif