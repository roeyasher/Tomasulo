#ifndef CDBFile
#define CDBFile

#include "common.h"

typedef struct IntCDB IntCDB;
struct IntCDB{
	int numOfRobSupplier;
	int result;
	int num;
	BOOL STLDIns;
	BOOL valid;
};

typedef struct FPCDB FPCDB;
struct FPCDB{

	int numOfRobSupplier;
	float result;
	BOOL valid;
	int num;
};

typedef struct LoadCDB LoadCDB;
struct LoadCDB{

	int numOfRobSupplier;
	float result;
	BOOL valid;
	int num;
};

void CDBControlInt(Instruction *int_to_cdb);
void CDBControlFPADD(Instruction *fp_to_cdb);
void CDBControlFPMULL(Instruction *fp_to_cdb);
void CDBControlLoad(Instruction *load_to_cdb);
void CDBUpdateRob();
void CDBUpdateRS();


#endif



