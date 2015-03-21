#define _CRT_SECURE_NO_DEPRECATEs
#include "shared.h"


void InitializeConfiguration(char *ConfigFile){

	char buffer[BUFFER_SIZE];

	int i=0,j=0;

	FILE *file=NULL;

	Configuration_Data *Config_Pointer=(Configuration_Data*)malloc(sizeof(Configuration_Data));



	file = FileOpen(ConfigFile, "rt");



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

FILE * FileOpen(char *input_path_file, const char* flags)
{
	FILE* file_to_open = NULL;
	printf("FileOpen:%s\n", input_path_file);
	file_to_open = fopen(input_path_file, flags); // warning: deprecated 
	if (NULL == file_to_open)
	{
		printf("The function FileOpen failed\n");
	}
	return (file_to_open);
}

