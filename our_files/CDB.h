#ifndef CDB
#define CDB

#include "common.h"

typedef struct IntCDB IntCDB;
struct IntCDB{

	int OPCODE;
	BOOL busy;
	int operand1, operand2;
	char LabelOfSupplier[LABEL_SIZE];
	struct IntCDB *next;
	int result;
	BOOL valid;
};

typedef struct FPCDB FPCDB;
struct FPCDB{

	int OPCODE;
	BOOL busy;
	int operand1, operand2;
	char LabelOfSupplier[LABEL_SIZE];
	struct FPCDB *next;
	int result;
	BOOL valid;
};

typedef struct LoadCDB LoadCDB;
struct LoadCDB{

	int OPCODE;
	BOOL busy;
	int operand1, operand2;
	char LabelOfSupplier[LABEL_SIZE];
	struct LoadCDB *next;
	int result;
	BOOL valid;
};
void CDBControlInt(IntCDB *int_to_cdb);
void CDBControlFP(FPCDB *fp_to_cdb);
void CDBControlLoad(LoadCDB *load_to_cdb);
#endif



