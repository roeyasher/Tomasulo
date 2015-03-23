#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

void Initialize_IntRegisters(){

	int i = 0;

	for (i = 0; i<NUM_OF_INT_REGISTERS; i++){
		Integer_Registers[i].value = 0;
		Integer_Registers[i].busy = FALSE;
		Integer_Registers[i].robNum = -1;
	}
}

void Initialize_FpRegisters(){

	int i = 0;

	for (i = 0; i<NUM_OF_FP_REGISTERS; i++){
		FP_Registers[i].value = (float)i;
		FP_Registers[i].busy = FALSE;
		Integer_Registers[i].robNum = -1;
	}
}

void RegisterLog(){

	FILE *output = NULL;
	int i = 0;
	output = FileOpen("regint.txt", "wt");

	for (i = 0; i < NUM_OF_INT_REGISTERS; i++)
		fprintf(output, "%d\n", Integer_Registers[i].value);

	fclose(output);
	output = FileOpen("regout.txt", "wt");

	for (i = 0; i < NUM_OF_INT_REGISTERS; i++)
		fprintf(output, "%f\n", FP_Registers[i].value);
}