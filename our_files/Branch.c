#define _CRT_SECURE_NO_DEPRECATEs
#include "shared.h"
void BuildBranchNode(Instruction *new_branch_node)
{
	new_branch_node = (Instruction *)malloc(1 * sizeof(Instruction));
	new_branch_node->DST = instr.DST;
	new_branch_node->IMM = instr.IMM;
	new_branch_node->next = instr.next;
	new_branch_node->numRob = instr.numRob;
	new_branch_node->OPCODE = instr.OPCODE;
	new_branch_node->PC = instr.PC;
	new_branch_node->SRC0 = instr.SRC0;
	new_branch_node->SRC1 = instr.SRC1;
	strcpy(new_branch_node->name, instr.name);
}
void AppendBranchToList()
{
	Instruction *node;
	if (TRUE == is_branch_list_empty()){
		BuildBranchNode(Branch_List);
	}
	else{
		node = Branch_List;
		while (NULL != node){
			node->next;
		}
		BuildBranchNode(Branch_List);
	}
}

BOOL CalculateBranch()
{
	switch (instr.OPCODE)
	{
	case BEQ:
		if ((Integer_Registers[instr.SRC0].busy == FALSE) && (Integer_Registers[instr.SRC1].busy == FALSE)){
			if (Integer_Registers[instr.SRC0].value == Integer_Registers[instr.SRC1].value){
				PC = PC + instr.IMM;
				flag = TRUE;
				trace[cycle].valid = TRUE;
				trace[cycle].issued = cycle;
				trace[cycle].execution = cycle;
				strcpy(trace[cycle].instruction, instr.name);
				return TRUE;
			}
			else{
				PC = PC + 4;
				flag = TRUE;
				trace[cycle].valid = TRUE;
				trace[cycle].issued = cycle;
				trace[cycle].execution = cycle;
				strcpy(trace[cycle].instruction, instr.name);
				return TRUE;
			}
		}
		else
		{
			return FALSE;
			//memset(&instr,0,sizeof(Instruction));
			//instr.OPCODE = -1;
			//PC=instruction_queue->PC;
		}

		break;
	case BNE:
		if ((Integer_Registers[instr.SRC0].busy == FALSE) && (Integer_Registers[instr.SRC1].busy == FALSE)){
			if (Integer_Registers[instr.SRC0].value != Integer_Registers[instr.SRC1].value){
				PC = PC + instr.IMM;
				flag = TRUE;
				trace[cycle].valid = TRUE;
				trace[cycle].issued = cycle;
				trace[cycle].execution = cycle;
				strcpy(trace[cycle].instruction, instr.name);
				return TRUE;
			}
			else{
				PC = PC + 4;
				flag = TRUE;
				trace[cycle].valid = TRUE;
				trace[cycle].issued = cycle;
				trace[cycle].execution = cycle;
				strcpy(trace[cycle].instruction, instr.name);
				return TRUE;
			}
		}
		else {
			return FALSE;
			//instr.OPCODE = -1;
			//PC=instruction_queue->PC;
		}
		break;
	case JUMP:
		PC = PC + instr.IMM;
		flag = TRUE;
		trace[cycle].valid = TRUE;
		trace[cycle].issued = cycle;
		trace[cycle].execution = cycle;
		strcpy(trace[cycle].instruction, instr.name);
		break;
		return TRUE;
	default:
		flag = FALSE;
		return FALSE;
		break;
	}
}
BOOL EmptyInstructionQueue()
{
	Instruction *node = NULL;
	node = instruction_queue_head;
	node = (Instruction*)node->next;
	while ((node != NULL) && (0!=(strcmp(node->name, "00000000"))))
	{
		
		node->OPCODE = 00000000;
		strcpy(node->name, "00000000");
		node = (Instruction*)node->next;
	}

}
void deletebranchfromlist(){
	Instruction *node = NULL;
	if (NULL != Branch_List)
	{
		node = Branch_List;
		if (NULL != Branch_List->next){
			Branch_List = (Instruction*)Branch_List->next;
		}
		free(node);
	}
}
BOOL ExecuteBranch (){
	BOOL executed = FALSE;
	//tring to execute and calculate
	if (CalculateBranch() == TRUE)
	{
		EmptyInstructionQueue();
		deletebranchfromlist();
		return TRUE;
	}
	else{
		return FALSE; // didn't execute
	}

}
BOOL is_branch_list_empty()
{
	if (NULL == Branch_List)
		return TRUE;
	else//not empty
		return FALSE;
}
BOOL ThereIsBranchWaiting(){
	BOOL execution_condition = FALSE;
	if(TRUE==is_branch_list_empty()) // list is empty
		return FALSE;
	else{// list is not empty
		execution_condition = ExecuteBranch();
		if (FALSE == execution_condition)
			return TRUE;
		else
			return (!is_branch_list_empty());
	}
}

//exceptions for JUMP;