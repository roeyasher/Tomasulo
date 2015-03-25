#ifndef CDBFile
#define CDBFile

#include "common.h"

typedef struct IntCDB IntCDB;
struct IntCDB{
	int numOfRobSupplier;
	int result;
	int issued;
	BOOL STLDIns;
	BOOL valid;
};

typedef struct FPCDB FPCDB;
struct FPCDB{

	int numOfRobSupplier;
	float result;
	BOOL valid;
};

typedef struct LoadCDB LoadCDB;
struct LoadCDB{

	int numOfRobSupplier;
	float result;
	BOOL valid;
};

void CDBControlInt(IntCDB *int_to_cdb);
void CDBControlFPADD(FPCDB *fp_to_cdb);
void CDBControlFPMULL(FPCDB *fp_to_cdb);
void CDBControlLoad(LoadCDB *load_to_cdb);
void CDBUpdateRob();
void CDBUpdateRS();


#endif



