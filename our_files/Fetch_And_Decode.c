#include "Fetch_And_Decode.h"

//oooooo a stucture for a data base link,hands the Queue insruction decode and fetch ooo/////



/*add global variables*/

extern Configuration_Data *Configuration;

extern Instruction instr;

extern BOOL instr_reservation;

extern BOOL flag;



/*For Integer Unit*/

extern IntegerRegister Integer_Registers [NUM_OF_INT_REGISTERS];

extern IntReservationStation_Line *IntReservationStation;

extern IntALU_PipelineStage *Integer_ALU_Unit;

/**/



/*For FP Unit*/

extern FpRegister FP_Registers[NUM_OF_FP_REGISTERS];

extern FpReservationStation_Line *FpReservationStation_ADD;

extern FpReservationStation_Line *FpReservationStation_MUL;

extern FP_PipelineStage *FP_executionPipeline_ADD;

extern FP_PipelineStage *FP_executionPipeline_MUL;



/*For fetch and decode*/

extern Instruction *instruction_queue_head;

extern int PC;

/**/



extern int cycle;

extern Trace trace[TRACE_SIZE];















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

			printf("an error, not a valid char, going to return 0\n");

			printf ("bad char is %c\n",schar);

			return 0;

	}

}



Instruction *LinkInstQueue (char instruction_line[],int pc_counter)

{

	int opcode,dst,src0,src1;

	int imm;

	/*the function that creates the instruction queue*/

	Instruction *new_node = (Instruction *)malloc(1*sizeof(Instruction));

	if (new_node != NULL)

	{



		opcode = CharToInteger(instruction_line[0]);

		dst = CharToInteger(instruction_line[1]);

		src0 = CharToInteger(instruction_line[2]);

		src1= CharToInteger(instruction_line[3]);

		if ((imm = CharToInteger(instruction_line[4])) >=8)

		{

			imm-=16;

		}

	

		imm=imm*16*16*16;

		imm=imm + CharToInteger(instruction_line[5])*16*16 + CharToInteger(instruction_line[6])*16 +  CharToInteger(instruction_line[7]);	/*decoding immediate*/

		new_node->OPCODE = opcode;

		new_node->DST = dst;

		new_node->SRC0 = src0;

		new_node->SRC1 = src1;

		if ((opcode==BEQ) || (opcode==BNE) || (opcode==JUMP)){

		new_node->IMM = imm*4;

		}else{

			new_node->IMM = imm;

		}

		new_node->PC = pc_counter;

		new_node->next = NULL;

		strcpy(new_node->name,instruction_line);

		return new_node;

	}



	return NULL;

}

void CheckTheConditionAndReturnPc(Instruction *temp)

{

	

	switch(instr.OPCODE)

	{

	case BEQ:

		if((Integer_Registers[instr.SRC0].busy == FALSE)&&(Integer_Registers[instr.SRC1].busy == FALSE)){

			if(Integer_Registers[instr.SRC0].value == Integer_Registers[instr.SRC1].value)

			{

				PC = PC + instr.IMM;

				

				flag=TRUE;

			//	FillTheFields(temp);

				trace[cycle].valid=TRUE;

				trace[cycle].issued=cycle;

				trace[cycle].execution=cycle;

				strcpy(trace[cycle].instruction,instr.name);



			}

			else

			{

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

	temp = SearchTheElementInstByPc(instruction_queue_head,PC);

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

			case BEQ:			

				CheckTheConditionAndReturnPc(temp);	

				

				break;

			case BNE:			

				CheckTheConditionAndReturnPc(temp);

				

				break;

			case JUMP:			

				CheckTheConditionAndReturnPc(temp);

				

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



void InitializeFetchAndDecode (FILE *memin_txt)

{

	char instruction_line[SIZE_OF_CHAR];

	Instruction *node = NULL; 

	int pc_conter=0;

	int i=0;

	//printf("roie\n");

	memset(instruction_line,0,SIZE_OF_CHAR);

	//while(fread(instruction_line,sizeof(char),8,memin_txt) != EOF)

		while ((fscanf(memin_txt,"%s",instruction_line)) != EOF)

	{

	/*	for(i=0;i<8;i++)

		{

			instruction_line[i]=instruction_line[i+1];

		}*/

		//printf("roie\n");

		node = instruction_queue_head;

		if (instruction_queue_head	== NULL)

		{

			instruction_queue_head = LinkInstQueue(instruction_line,pc_conter);

		}

		else 

		{

			while(node->next !=NULL)

			{

				node =(Instruction*) node->next;

			}

			node->next = LinkInstQueue(instruction_line,pc_conter);

		}

		pc_conter+=4;

	//	printf("%c\n",instruction_line[0]);

		if (instruction_line[0] == 'c'){

			rewind(memin_txt);

			break;

		}

		memset(instruction_line,0,SIZE_OF_CHAR);

		//fseek(memin_txt,1,SEEK_CUR);

		

	}

	node = instruction_queue_head;

	while(node !=NULL)

	{

		//printf("%d %d %d %d %d %d\n",node->OPCODE,node->DST,node->SRC0,node->SRC1,node->IMM,node->PC);

		node= node->next;

	}

	

}



void simulateclockFetchAndDecode()

{

	/*A simluate clock cycle for fetch and decode unit*/

	DecodeAndDistributor(instruction_queue_head);



}



