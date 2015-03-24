#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"


robLine *CreateRLNewNode(int index){

	/*this function create a new node for a linked list, for the load buffer*/
	robLine *temp = NULL;
	temp = (robLine*) malloc(sizeof(robLine));
	memset(temp, 0, sizeof(robLine));
	temp->numRob = index;
	temp->next = NULL;
	return temp;		/*NULL is returned if failure occured*/

}


void IntilaizeRob() {
	/*this function intilaize the load buffer/reservation station*/

	int Number_of_Rob_Lines = Configuration->rob_entries;
	int i=0;
	robCnt = 0;

	robLine *node = NULL;
	robLines = CreateRLNewNode(1);
	node = robLines;

	for (i=2;i<=Number_of_Rob_Lines;i++)	{

		node->next = CreateRLNewNode(i);
		node = node->next;
		node->busy = FALSE;
	}
}

void emptyRob(){

	robLine *succ = robLines, *prev = robLines;
	int tmp;

	while(succ != NULL) {
		prev = succ;
		succ = succ->next;
		tmp = prev->numRob;
		memset(prev, 0, sizeof(robLine));
		prev->numRob = tmp;
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
	instr.numRob = availableRobLine->numRob;
	robCnt++;

}

// commit if the first insruction that done
BOOL commit(){
	
	robLine *node = robLines, *last = robLines;
	char label[LABEL_SIZE];
	int i = 0;
	int tmp;

	if (node->done == TRUE){

		/// write to regs!.

		/*update registers*/
		for (i = 0; i<NUM_OF_INT_REGISTERS; i++){

			if ((Integer_Registers[i].busy == TRUE) && Integer_Registers[i].robNum == node->numRob){

				Integer_Registers[i].value = node->intValue;			/*update value*/
				Integer_Registers[i].busy = FALSE;					/*no more busy*/
				Integer_Registers[i].robNum = -1;	/*reset label though it is not necessary*/
			}
		}

		/*update registers*/
		for (i = 0; i<NUM_OF_FP_REGISTERS; i++){
			if ((FP_Registers[i].busy == TRUE) && Integer_Registers[i].robNum == node->numRob){
				FP_Registers[i].value = last->fpValue;
				FP_Registers[i].busy = FALSE;
				Integer_Registers[i].robNum = -1;
			}
		}

		robLines = node->next;
		tmp = node->numRob;
		memset(node, 0, sizeof(robLine));
		node->numRob = tmp;
		robCnt--;
		return TRUE;
	}
	else{
		return FALSE;
	}

}

BOOL isRobFull(){
	return (robCnt == Configuration->rob_entries);
}

