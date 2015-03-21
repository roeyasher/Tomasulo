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


void IntilaizeRob()

{

	/*this function intilaize the load buffer/reservation station*/

	int Number_of_Rob_Lines = Configuration->rob_entries;
	int i=0;

	robLine *node = NULL;
	robLines = CreateRLNewNode();
	sprintf(robLines->label,"ROB%d",i+1);
	node = robLines;

	for (i=1;i<Number_of_Rob_Lines;i++)	{

		node->next = CreateRLNewNode();
		node = node->next;
		sprintf(node->label,"ROB%d",i+1);
	}

}