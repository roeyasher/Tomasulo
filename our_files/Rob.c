#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"


robLine *CreateRLNewNode(){

	/*this function create a new node for a linked list, for the load buffer*/
	robLine *temp = NULL;
	temp = (robLine*) malloc(sizeof(robLine));
	memset(temp, 0, sizeof(robLine));
	temp->next = NULL;
	return temp;		/*NULL is returned if failure occured*/

}


void IntilaizeRob() {
	/*this function intilaize the load buffer/reservation station*/

	int Number_of_Rob_Lines = Configuration->rob_entries;
	int i=0;
	robCnt = 0;

	robLine *node = NULL;
	robLines = CreateRLNewNode();
	sprintf(robLines->label,"ROB%d",i+1);
	node = robLines;

	for (i=1;i<Number_of_Rob_Lines;i++)	{

		node->next = CreateRLNewNode();
		node = node->next;
		sprintf(node->label,"ROB%d",i+1);
		node->busy = FALSE;
	}
}

void emptyRob(){

	robLine *succ = robLines, *prev = robLines;
	char label[LABEL_SIZE];

	while(succ != NULL) {
		prev = succ;
		succ = succ->next;
		strncpy(label, prev->label, LABEL_SIZE);
		memset(prev, 0, sizeof(robLine));
		strncpy(prev->label, label, LABEL_SIZE);
	} 
	robCnt = 0;
}

// Try to inster the global instruction to the rob line.
BOOL insertRob(){

	robLine *iter = robLines, *availableRobLine = NULL;
	
	while (iter != NULL){

		if (iter->busy == FALSE){
			availableRobLine = iter;
			break;
		}
		iter = iter->next;
	}

	if (availableRobLine == NULL)
		return FALSE;

	availableRobLine->OPCODE = instr.OPCODE;
	availableRobLine->dest = instr.DST;
	availableRobLine->inExecution = FALSE;
	availableRobLine->done = FALSE;
	availableRobLine->busy = TRUE;
	availableRobLine->state = 2; // issue
	robCnt++;

}

// commit if the first insruction that done
BOOL commit(){
	
	robLine *node = robLines, *last = robLines;
	char label[LABEL_SIZE];

	if (node->done == TRUE){

		/// TODO write to regs!.

		robLines = node->next;
		strncpy(label, node->label, LABEL_SIZE);
		memset(node, 0, sizeof(robLine));
		strncpy(node->label, label, LABEL_SIZE);
	}
	else{
		return FALSE;
	}

	// get the last node
	while (last->next != NULL)
		last = last->next;

	last->next = node;
	node->next = NULL;
	robCnt--;
}

BOOL isRobFull(){
	return (robCnt == Configuration->rob_entries);
}

