
#ifndef CONFIGURATION_FILE

#define CONFIGURATION_FILE



#include <stdio.h>

#include <stdlib.h>

#include <string.h>



#include "Fetch_And_Decode.h"

#include "IntegerUnit.h"





#define SIZE_OF_CHAR 512

#define TRUE 1

#define FALSE 0

#define NUM_OF_INT_REGISTERS 16

#define LABEL_SIZE 10

#define BUFFER_SIZE 512



typedef int BOOL;





typedef struct{

	int int_delay;

	int add_delay;

	int mul_delay;

	int mem_delay;

	int add_nr_reservation;

	int mul_nr_reservation;

	int int_nr_reservation;

	int mem_nr_load_buffers;

	int mem_nr_store_buffers;

}Configuration_Data;



void InitializeConfiguration(char *ConfigFile);



#endif