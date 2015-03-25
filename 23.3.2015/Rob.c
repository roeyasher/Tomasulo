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
	availableRobLine->done = FALSE;
	availableRobLine->busy = TRUE;
	availableRobLine->state = 2; // issue
	instr.numRob = availableRobLine->numRob;
	robCnt++;

}
void updateIntRob(int numRob, int value){

	robLine *iter = robLines;

	while (iter != NULL){
		if (iter->numRob == numRob){
			iter->intValue = value;
			break;
		}
		iter = iter->next;
	}
	return;
}

void updateFPRob(int numRob, float value){

	robLine *iter = robLines;

	while (iter != NULL){
		if (iter->numRob == numRob){
			iter->fpValue = value;
			break;
		}
		iter = iter->next;
	}
	return;
}

// commit if the first insruction that done
BOOL commitRob(){
	
	robLine *node = robLines, *last = robLines;
	int i = 0;
	int tmp;

	if (node->done == TRUE){

		/*update registers*/
		for (i = 0; i<NUM_OF_INT_REGISTERS; i++){

			if ((Integer_Registers[i].busy == TRUE) && Integer_Registers[i].robNum == node->numRob && node->done == TRUE){

				Integer_Registers[i].value = node->intValue;			/*update value*/
				Integer_Registers[i].busy = FALSE;					/*no more busy*/
				Integer_Registers[i].robNum = 0;	/*reset label though it is not necessary*/
			}
		}

		/*update registers*/
		for (i = 0; i<NUM_OF_FP_REGISTERS; i++){
			if ((FP_Registers[i].busy == TRUE) && FP_Registers[i].robNum == node->numRob  && node->done == TRUE){
				FP_Registers[i].value = last->fpValue;
				FP_Registers[i].busy = FALSE;
				Integer_Registers[i].robNum = 0;
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

BOOL IsRobEmpty(){
	int Number_of_Rob_Lines = Configuration->rob_entries,i=0;
	robLine *node = NULL;
	node = robLines;
	for (i = 2; i <= Number_of_Rob_Lines; i++)	{
		if (TRUE == node->busy){
			return FALSE;
		}
		node = node->next;
	}
}
