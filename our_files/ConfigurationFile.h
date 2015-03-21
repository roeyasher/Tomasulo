#ifndef CONFIGURATION_FILE
#define CONFIGURATION_FILE

#include "common.h"

typedef struct Configuration_Data Configuration_Data;

struct Configuration_Data {

	int int_delay;

	int add_delay;

	int mul_delay;

	int mem_delay;
	
	int rob_entries;

	int add_nr_reservation;

	int mul_nr_reservation;

	int int_nr_reservation;

	int mem_nr_load_buffers;

	int mem_nr_store_buffers;
};



void InitializeConfiguration(char *ConfigFile);



#endif