#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Windows.h>


/*possible opcodes of ISA*/
#define LD 0
#define ST 1
#define JUMP 2
#define BEQ 3
#define BNE 4
#define ADD 5
#define ADDI 6
#define SUB 7
#define SUBI 8
#define ADDS 9
#define SUBS 10
#define MULTS 11 
#define HALT 12

#define Memory_INS 0
#define INT_INS 1
#define FP_INS 2


#define SIZE_OF_CHAR 512
#define TRUE 1
#define FALSE 0
#define NUM_OF_INT_REGISTERS 16
#define NUM_OF_FP_REGISTERS 16
#define InstructionQueueSize 16
#define LABEL_SIZE 10
#define BUFFER_SIZE 512
#define TRACE_SIZE 1000000
#define MEMORY_SIZE 1024

typedef int BOOL;