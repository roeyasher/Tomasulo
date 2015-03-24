#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"


void CDBControlInt(IntCDB *int_to_cdb)
{
	IntUnitCDB.result				= int_to_cdb->result;
	LoadUnitCDB.numOfRobSupplier	= int_to_cdb->numOfRobSupplier;
	IntUnitCDB.valid				= TRUE;
	
	return;
}

void CDBControlFP(FPCDB *fp_to_cdb)
{
	FPUnitCDB.result				= fp_to_cdb->result;
	LoadUnitCDB.numOfRobSupplier	= fp_to_cdb->numOfRobSupplier;
	FPUnitCDB.valid					= TRUE;
	
	return;
}

void CDBControlLoad(LoadCDB *load_to_cdb)
{
	LoadUnitCDB.result				= load_to_cdb->result;
	LoadUnitCDB.valid				= TRUE;
	LoadUnitCDB.numOfRobSupplier	= load_to_cdb->numOfRobSupplier;
	return;
}