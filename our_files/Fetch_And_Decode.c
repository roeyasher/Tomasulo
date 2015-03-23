#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

//oooooo a stucture for a data base link,hands the Queue insruction decode and fetch ooo/////

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo//

int CharToInteger (char schar)
{ 
	switch (schar)
	{
		case '0':
			return 0;
			break;
		case '1':
			return 1;
			break;
		case '2':
			return 2;
			break;
		case '3':
			return 3;
			break;
		case '4':
			return 4;
			break;
		case '5':
			return 5;
			break;
		case '6':
			return 6;
			break;
		case '7':
			return 7;
			break;
		case '8':
			return 8;
			break;
		case '9':
			return 9;
			break;
		case 'a':
			return 10;
			break;
		case 'b':
			return 11;
			break;
		case 'c':
			return 12;
			break;
		case 'd':
			return 13;
			break;
		case 'e':
			return 14;
			break;
		case 'f':
			return 15;
			break;
		default:
			printf("bad char, so return 0, you gave me= %c\n", schar);
			return 0;
	}
}




void CheckTheConditionAndReturnPc(Instruction *temp)

{
	switch(instr.OPCODE)
	{
	case BEQ:
		if((Integer_Registers[instr.SRC0].busy == FALSE)&&(Integer_Registers[instr.SRC1].busy == FALSE)){
			if(Integer_Registers[instr.SRC0].value == Integer_Registers[instr.SRC1].value){
				PC = PC + instr.IMM;
				flag=TRUE;
			//	FillTheFields(temp);
				trace[cycle].valid=TRUE;
				trace[cycle].issued=cycle;
				trace[cycle].execution=cycle;
				strcpy(trace[cycle].instruction,instr.name);
			}
			else{
				PC=PC+4;
				flag=TRUE;
				//FillTheFields(temp);
				trace[cycle].valid=TRUE;
				trace[cycle].issued=cycle;
				trace[cycle].execution=cycle;
				strcpy(trace[cycle].instruction,instr.name);
			}
		}
		else
		{
			//memset(&instr,0,sizeof(Instruction));
			//instr.OPCODE = -1;
			//PC=instruction_queue->PC;
		}
	
		break;
	case BNE:
		if((Integer_Registers[instr.SRC0].busy == FALSE)&&(Integer_Registers[instr.SRC1].busy == FALSE)){
			if(Integer_Registers[instr.SRC0].value != Integer_Registers[instr.SRC1].value){
				PC = PC + instr.IMM;
				flag=TRUE;
			//	FillTheFields(temp);
				trace[cycle].valid=TRUE;
				trace[cycle].issued=cycle;
				trace[cycle].execution=cycle;
				strcpy(trace[cycle].instruction,instr.name);
			}
			else{
				PC = PC + 4;
				flag=TRUE;
			//	FillTheFields(temp);
				trace[cycle].valid=TRUE;
				trace[cycle].issued=cycle;
				trace[cycle].execution=cycle;
				strcpy(trace[cycle].instruction,instr.name);
			}
		}
		else {
			//instr.OPCODE = -1;
			//PC=instruction_queue->PC;
		}
		break;
	case JUMP:
			PC = PC + instr.IMM;
			flag=TRUE;
		//	FillTheFields(temp);
			trace[cycle].valid=TRUE;
			trace[cycle].issued=cycle;
			trace[cycle].execution=cycle;
			strcpy(trace[cycle].instruction,instr.name);
			break;
	default:
		flag=FALSE;
		break;
	}
}

Instruction *SearchTheElementInstByPc(Instruction *instruction_queue_head)
{

	/*a help function the do the fetch and decode main function*/

	Instruction *instr_queue=instruction_queue_head;
	while(instr_queue != NULL)
	{
		if(instr_queue->PC == PC)
			return instr_queue;
		instr_queue =(Instruction*) instr_queue->next;
	}
	return instr_queue;
}

void FillTheFields(Instruction *instr_queue)
{
		/*this function fill the fields to a global parameter*/
		instr.OPCODE = instr_queue->OPCODE;
		instr.DST = instr_queue->DST;
		instr.SRC0 = instr_queue->SRC0;
		instr.SRC1 = instr_queue->SRC1;
		instr.IMM = instr_queue->IMM;
		instr.PC = instr_queue->PC;
		instr.next = NULL;
		strcpy(instr.name,instr_queue->name);
}

Instruction *DeleteTheInstrcutionsDistributor()
{
	return NULL;
}


int DecodeAndDistributor(Instruction *instruction_queue_head)

{	
	/*the main function that handle with branches and jump and do the fetch and decode*/
	/*the function checks the next instr can be sent to the units and for branches and jump the function*/
	/* make a 1 clock cycle delay and brings the next instr depends the branch condition*/
	//int pc_counter= instruction_queue_head->PC;
	Instruction *temp = instruction_queue_head;
	//instr_reservation = TRUE;
	temp = SearchTheElementInstByPc(instruction_queue_head);
	//printf("instruction adress is %p\n",temp);
		if(temp== NULL)
	{	
	//	printf("well well finish all the instruction queue\n");
		return FALSE;
	}
		/*if instr_reservation is FALSE then do nothing otherwise old instruction is erased*/
		if ((instr_reservation == TRUE)/*||(temp->OPCODE==BEQ)||(temp->OPCODE==BNE)||(temp->OPCODE==JUMP)*/){
			FillTheFields(temp);
		}
		switch (temp->OPCODE)
		{
			case HALT:
				// simulate more clock cycles
				if (instr_reservation==TRUE){
					FillTheFields(temp);
					trace[cycle].issued=cycle;
					trace[cycle].valid=1;
					trace[cycle].execution=cycle;
					strcpy(trace[cycle].instruction,instr.name);
				}	
				break;
			default:		/*other*/
				CheckTheConditionAndReturnPc(temp);
				if(instr_reservation == FALSE)// need to check what to do with this.
				{	
					//printf("all the resarvation stations are busy\n");
					//Sleep(1000);					
					break;
				}	
				else
				{
				PC=PC+4;
				break;		
				}
	}
	return TRUE;
}
BOOL LinkInstQueue(char instruction_line[], int *instruction_queue_counter) {
	/* Fethch new instruction from memory to queue*/
	int opcode, dst, src0, src1;
	int imm;
	Instruction *node = NULL;
	node = instruction_queue_head;
	// check whether we have more instruction in main memory
	if (strcmp(instruction_line,"00000000")==0)
	{
		return TRUE;
	}
	//find place in instruction queue	
	while ((strcmp(node->name, "00000000")!=0 ) && (NULL != node))
	{
		node = (Instruction*)node->next;
	}
			if (NULL != node)
			{
				(*instruction_queue_counter)++;
				strcpy(node->name, instruction_line);
				node->OPCODE = CharToInteger(instruction_line[0]);
				node->DST = CharToInteger(instruction_line[1]);
				node->SRC0 = CharToInteger(instruction_line[2]);
				node->SRC1 = CharToInteger(instruction_line[3]);
				if ((imm = CharToInteger(instruction_line[4])) >= 8)
				{
					imm -= 16;
				}
				imm = imm * 16 * 16 * 16;
				imm = imm + CharToInteger(instruction_line[5]) * 16 * 16 + CharToInteger(instruction_line[6]) * 16 + CharToInteger(instruction_line[7]);	/*decoding immediate*/
				if ((node->OPCODE == BEQ) || (node->OPCODE == BNE) || (node->OPCODE == JUMP)){
					node->IMM = imm * 4;
				}
				else{
					node->IMM = imm;
				}
			}
			return FALSE;
		}

BOOL FetchAndDecode(char *memory[], int *pc_conter_to_fetch, int * instruction_queue_counter)
{
	char instruction_line[SIZE_OF_CHAR];
	Instruction *node = NULL; 
	int i = 0, counterdivide = 0, place_in_array=0,j=0;
	BOOL is_it_end_of_instruction_in_memory = FALSE;

	place_in_array = ((*pc_conter_to_fetch)*(512 / 4));
	node = instruction_queue_head;
	is_it_end_of_instruction_in_memory = LinkInstQueue((memory + place_in_array), instruction_queue_counter);
	if (TRUE == is_it_end_of_instruction_in_memory)
	{
		return TRUE;
	}
	return FALSE;
}

VOID * GetInstructionFromQUeue(Instruction *result_instruction)
{
	
	Instruction *hold_head = instruction_queue_head;
	Instruction *node = instruction_queue_head;
	result_instruction->DST = instruction_queue_head->DST;
	result_instruction->IMM = instruction_queue_head->IMM ;
	strcpy(result_instruction->name,instruction_queue_head->name);
	result_instruction->next = instruction_queue_head->next;
	result_instruction->OPCODE = instruction_queue_head->OPCODE;
	result_instruction->PC = instruction_queue_head->PC;
	result_instruction->SRC0 = instruction_queue_head->SRC0;
	result_instruction->SRC1 = instruction_queue_head->SRC1;

	while (NULL != node->next)
	{
		node = (Instruction*)node->next;
	}
	instruction_queue_head = (Instruction*)instruction_queue_head->next;
	node->next = (Instruction*)hold_head;
	node->next->next = NULL;
	
	strcpy(node->next->name, "00000000");
	return;
}
void simulateclockFetchAndDecode()

{
	/*A simluate clock cycle for fetch and decode unit*/
	DecodeAndDistributor(instruction_queue_head);
}

void EmptyInsturcionQueue(int *counter)

{
	Instruction *node = NULL;
	Instruction *node_last = NULL;
	node = instruction_queue_head;
	while (node->next != NULL)
	{
		strcpy(node->name, "00000000");
		node = (Instruction*)node->next;
	}
	(*counter) -= 16;
	strcpy(node->name, "00000000");
	
}

void freeInsturcionQueue()

{
	Instruction *node = NULL;
	Instruction *node_last = NULL;
	node = instruction_queue_head;
	node_last = instruction_queue_head;
	while (node->next != NULL)
	{
		node = (Instruction*)node->next;
		free(node_last);
		node_last = (Instruction*)node;
	}
	
	instruction_queue_head=NULL;
}



void IntilaizeInstructionQueue() {
	/*this function intilaize the load buffer/reservation station*/

	int i = 0;
	Instruction *node = NULL;
	instruction_queue_head = (Instruction *)malloc(1 * sizeof(Instruction));
	instruction_queue_head->OPCODE = 00000000;
	strcpy(instruction_queue_head->name, "00000000");
	node = instruction_queue_head;
	for (i = 1; i<InstructionQueueSize; i++)	{
		node->next = (Instruction *)malloc(1 * sizeof(Instruction));
		node->next->OPCODE=00000000;
		strcpy(node->next->name , "00000000");
		node = (Instruction*)node->next;
	}
	node->next = NULL;
}