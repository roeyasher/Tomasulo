#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"

void InitializeTrace(){

	int i = 0;
	for (i = 0; i < TRACE_SIZE; i++){

		trace[i].CDB = -1;
		trace[i].execution_start = -1;
		trace[i].issued = -1;
		trace[i].commit = -1;
		memset(trace[i].instruction, 0, 16);
	}
}

void PrintTrace(){

	int i = 0;
	FILE *dest = FileOpen("trace.txt", "wt");

	for (i = 0; i < TRACE_SIZE; i++){
		if (trace[i].issued != -1){
			fprintf(dest, "%s %d %d %d %d\n", trace[i].instruction, trace[i].issued, trace[i].execution_start, trace[i].CDB,trace[i].commit);
		}
	}
	fclose(dest);
}