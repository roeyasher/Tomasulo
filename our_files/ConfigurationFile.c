#include "shared.h"


void InitializeConfiguration(char *ConfigFile){

	char buffer[BUFFER_SIZE];

	int i=0,j=0;

	FILE *file=NULL;

	Configuration_Data *Config_Pointer=(Configuration_Data*)malloc(sizeof(Configuration_Data));



	file=fopen(ConfigFile,"rt");



	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->int_delay=atoi(buffer)+1;

	



	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->add_delay=atoi(buffer)+1;



	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->mul_delay=atoi(buffer)+1;



	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->mem_delay=atoi(buffer);

	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->rob_entries=atoi(buffer);


	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->add_nr_reservation=atoi(buffer);

	

	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->mul_nr_reservation=atoi(buffer);

	

	

	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->int_nr_reservation=atoi(buffer);

	



	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->mem_nr_load_buffers=atoi(buffer);



	for (i=0;i<3;i++){

			memset(buffer,0,BUFFER_SIZE);

			fscanf(file,"%s",buffer);

		}

	Config_Pointer->mem_nr_store_buffers=atoi(buffer);//fix after debug

	

	



	Configuration=Config_Pointer;

	fclose(file);



}



