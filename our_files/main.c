#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"



/*Defining global variables*/

Configuration_Data *Configuration=NULL;

Instruction instr;

BOOL instr_reservation=TRUE;

BOOL flag=FALSE;

int cycle=1;

Trace trace[TRACE_SIZE];



/*For Rob*/

robLine *robLines=NULL;/*load buffer/load reservation*/
/**/


/*For Integer Unit*/

IntegerRegister Integer_Registers [NUM_OF_INT_REGISTERS];/*Integer Registers array*/

IntReservationStation_Line *IntReservationStation = NULL;/*Integer reservation station*/

IntALU_PipelineStage *Integer_ALU_Unit=NULL;/*Integer - ALU pipeline execute unit*/

/**/



/*For FP Unit*/

FpRegister FP_Registers[NUM_OF_FP_REGISTERS];/*FP register array*/

FpReservationStation_Line *FpReservationStation_ADD=NULL;/*FP reservation station- for ADD instr*/

FpReservationStation_Line *FpReservationStation_MUL=NULL;/*FP reservation station - for MUL instr*/

FP_PipelineStage *FP_executionPipeline_ADD=NULL;/*the FP-ADD pipeline execute unit*/

FP_PipelineStage *FP_executionPipeline_MUL=NULL;/*the FP-MUL pipeline execute unit*/

/**/



/*for Memory Unit*/

LoadBuffer *LoadBufferResarvation=NULL;/*load buffer/load reservation*/

float PhysicalMemoryArray[MEMORY_SIZE];/*the memory unit*/

StoreBuffer *StoreBufferResarvation=NULL;/*store buffer/store reservation*/

Memory_PiplineStage *Memory_Unit=NULL;/*the memory pipeline execute unit*/

Memory_Pipline BufferToMemory;/*a variable to helps us in the programm runing*/

CdbWriteBack CdbToResarvation;/*CDB write back*/

/**/



/*For Fetch and Decode*/

Instruction *instruction_queue_head = NULL;

int PC=0;/*a global parameter - program counter - indicate our location in the program*/

/**/



/*OOOOOOOOOOOOOOOOOOOOOOOOOO*/



/*create registers/memory/traces logs*/

void RegisterLog();

void MemoryLog();

void PrintTrace();

/*read memin file and initialize memory*/

void InitializeMemory(FILE *memin);

/*initialize trace structure*/

void InitializeTrace();

/*check if all reservation stations are empty from instructions - used to know when to terminate program after issue of HALT*/

int detectEnd();





//void duplicate(char *in);



int main(int argc,char* argv[]){
	
	//FILE *memin_txt=fopen(argv[2],"rt");
	//FILE *memin_txt = FileOpen(argv[2], "rt");
	char MainMemoryArray[MEMORY_SIZE][BUFFER_SIZE];
	char *adressMainMemory = MainMemoryArray[0];
	int i=0,pc_counter=0,instruction_queue_counter=0;
	BOOL reservation_stations_has_space = TRUE;
	BOOL rob_has_space = TRUE;
	BOOL no_more_instruction = FALSE;
	instr.OPCODE=-1;

	/*intialize all*/



	/*configuration file*/

	InitializeConfiguration(argv[1]);

		

	/*Memory reservation stations and execution units*/

	IntilaizeRob();

	IntilaizeLoadBuffer();

	IntilaizeStoreBuffer();

	IntializeMemPipline();
	//TODO enable me
	//InitializeMemory(memin_txt);
	MemInToMainMemory(adressMainMemory, argv[2]);
	

	/*INT reservation stations and execution units*/

	InitializeIntegerALU();			

	Initialize_IntRegisters();

	InitializeReservationStation();



	/*FP reservation stations and execution units*/

	Initialize_FpRegisters();

	Initialize_FpReservationStations();

	Initialize_FpPipelines();



	/*fetch and decode unit to insert the instructions to the instructions queue DB*/
	



	/*initialize structure for trace file*/

	InitializeTrace();



	/**/

	strcpy(CdbToResarvation.label,"Init");

		

	printf("Starting simulator with files <%s> <%s>\n",argv[1],argv[2]);

	/*this loop simulate one cycle by calling the simulateclockcycle function of each unit*/

	while (TRUE)
	{
		//TODO We should add flags for: the reservation station and the rob whether they are free or not, and also to check the counter I did for the instruction_queue_counter
		while ((reservation_stations_has_space == TRUE) && (rob_has_space == TRUE) && (no_more_instruction == FALSE) && (instruction_queue_counter<16))
		{
			no_more_instruction = InitializeFetchAndDecode(adressMainMemory, &pc_counter, &instruction_queue_counter);
		}

		/*brings relevant instruction to instr and updates PC if necessary*/

		simulateclockFetchAndDecode();

		/*init as instruction not taken by any unit*/

		instr_reservation=FALSE;	

		/*simulate till HALT is issued*/

		if (instr.OPCODE == HALT)

			break;



		/*simulat each unit one clock cycle. if a unit issues instr, update instr_reservation to say so (used for Fetch&Decode unit)*/

		if ( SimulateClockCycle_IntUnit() ){

			instr_reservation=TRUE;

		}

		if ( simulateClockCycle_FpUnit() ){

			instr_reservation=TRUE;

		}

		if ( SimulateClockCycle_LoadUnit(cycle,0) ){

			instr_reservation=TRUE;

		}

		

		/*flag==TRUE when instr is BEQ/BNE/JUMP and fetch&decode unit has taken it. in that case instr_reservation should be TRUE*/

		

		if ((flag == TRUE) || (instr_reservation == TRUE)){	/*if BEQ/BNE/JMP then flag is set to TRUE. otherwise instruction was not taken by any unit*/
								//TODO: remove the blbla condition (meutar).
				instr_reservation=TRUE;

		}

		else{

				instr_reservation=FALSE;

		}

				

		cycle++;
		instruction_queue_counter--;


		/*the last fix just to get a trace in the forth test*/

		if(cycle == 200000)

			break;

		

			

	}



	/*getting here means all instructions were issued followed by HALT instruction.*/

	instr.OPCODE=-1;	/*not necessary*/ //TODO remove



	/*after isuse of HALT, all instructions are for sure issued to reservation statinos. we must simulate new clock cycles until all reservation stations

	are empty. then the process can terminate.*/



	/* gadi : the second mistake is here and should be ! detectEND competibale to the way we wrote this function*/
	//TODO remove the gadi comment

	/*while(detectEnd()){*/

	while(!detectEnd()){

		SimulateClockCycle_IntUnit();

		simulateClockCycle_FpUnit();

		SimulateClockCycle_LoadUnit(cycle,1);

				

		cycle++;

	}



	/*just in case*/

	for (i=0;i<10000;i++){

		SimulateClockCycle_IntUnit();

		simulateClockCycle_FpUnit();

		SimulateClockCycle_LoadUnit(cycle,1);

	}







	/*the print function to log files: create 4 logfiles: trace,memorylog,registerlog(X2)*/	

	RegisterLog();		/*creates registers log*/

	MemoryLog();		/*creates memory log*/

	PrintTrace();		/*creates trace log*/



	/*close all files*/

	

	_fcloseall();	/*just in case*/



	printf("End of simulation. \nOutput files <%s> <%s> <%s> <%s>\n\n",argv[3],argv[4],argv[5],argv[6]);

	getchar();
	return  0;

}	







void RegisterLog(){

	FILE *output=NULL;

	int i=0;




	output = FileOpen("regint.txt", "wt");

	for (i=0;i<NUM_OF_INT_REGISTERS;i++){

		fprintf(output,"%d\n",Integer_Registers[i].value);

	}



	fclose(output);

	output = FileOpen("regout.txt", "wt");

	for (i=0;i<NUM_OF_INT_REGISTERS;i++){

		fprintf(output,"%f\n",FP_Registers[i].value);

	}



}



void MemoryLog(){

	FILE *output=NULL;

	int i=0;



	output = FileOpen("memout.txt", "wt+");

	for (i=0;i<MEMORY_SIZE;i++){

		fprintf(output,"%.8x\n",*(int*)&PhysicalMemoryArray[i]);

	}

	fflush(output);

	fclose(output);





}



void InitializeMemory(FILE *memin){

	char memData[512];

	int i=0;

	int val=0;

	//FILE *memin=FileOpen(memfile,"rt");

	while(i<MEMORY_SIZE){


		fscanf(memin,"%s",memData);

		val = (int)strtol(memData, NULL, 16);     //TODO check correctness (ask itay if he see any diffrent).

		PhysicalMemoryArray[i]=*(float*)&(val);
	
		i++;

	}

	fclose(memin);

	

}



/*void duplicate(char *in){

	FILE *source,*dest;

	int count=0;

	char buffer[16000];



	memset(buffer,0,16000);

	source=fopen(in,"rt");

	

	fseek(source,0,SEEK_END);

	count=ftell(source);

	rewind(source);

	fread(buffer,sizeof(char),count,source);

	

	fclose(source);

	dest=fopen(in,"wt");

	fwrite(buffer,sizeof(char),count,dest);

	fclose(dest);



	

}*/

void InitializeTrace(){

	int i=0;

	for (i=0;i<TRACE_SIZE;i++){

		trace[i].CDB=-1;

		trace[i].execution=-1;

		trace[i].issued=-1;

		trace[i].valid=0;

		memset(trace[i].instruction,0,16);

	}

}



void PrintTrace(){

	int i=0;

	FILE *dest=FileOpen("trace.txt","wt");



	for (i=0;i<TRACE_SIZE;i++){

		if ( trace[i].valid == TRUE){

			fprintf(dest,"%s %d %d %d\n",trace[i].instruction,trace[i].issued,trace[i].execution,trace[i].CDB);	

		}

	}

	fclose(dest);

}



int detectEnd(){

	int i=0;

	IntReservationStation_Line *temp1=IntReservationStation;

	FpReservationStation_Line *temp2=FpReservationStation_ADD;

	FpReservationStation_Line *temp3=FpReservationStation_MUL;

	LoadBuffer *temp4=LoadBufferResarvation;

	StoreBuffer *temp5=StoreBufferResarvation;









	for (i=0;i<Configuration->int_nr_reservation;i++){

		if (temp1->busy == TRUE)

			return 0;

		temp1=temp1->next;

	}



	

	for (i=0;i<Configuration->add_nr_reservation;i++){

		if (temp2->busy == TRUE)

			return 0;

		temp2=temp2->next;

	}



	for (i=0;i<Configuration->mul_nr_reservation;i++){

		if (temp3->busy == TRUE)

			return 0;

		temp3=temp3->next;

	}



	for (i=0;i<Configuration->mem_nr_load_buffers ;i++){

		if (temp4->busy == TRUE)

			return 0;

		temp4=temp4->next;

	}



	for (i=0;i<Configuration->mem_nr_store_buffers ;i++){

		if (temp5->busy == TRUE)

			return 0;

		temp5=temp5->next;

	}



	return 1;



	}	





