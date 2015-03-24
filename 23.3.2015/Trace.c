#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

void InitializeTrace(){

	int i = 0;
	for (i = 0; i < TRACE_SIZE; i++){

		trace[i].CDB = -1;
		trace[i].execution = -1;
		trace[i].issued = -1;
		trace[i].valid = 0;
		memset(trace[i].instruction, 0, 16);
	}
}

void PrintTrace(){

	int i = 0;
	FILE *dest = FileOpen("trace.txt", "wt");

	for (i = 0; i < TRACE_SIZE; i++){
		if (trace[i].valid == TRUE){
			fprintf(dest, "%s %d %d %d\n", trace[i].instruction, trace[i].issued, trace[i].execution, trace[i].CDB);
		}
	}
	fclose(dest);
}