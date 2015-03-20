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