#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"



//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo//
void CDBControlInt(IntCDB *int_to_cdb)
{
	IntUnitCDB.busy					= int_to_cdb->busy;
	IntUnitCDB.next					= int_to_cdb->next;
	IntUnitCDB.OPCODE				= int_to_cdb->OPCODE;
	IntUnitCDB.operand1				= int_to_cdb->operand1;
	IntUnitCDB.operand2				= int_to_cdb->operand2;
	IntUnitCDB.result				= int_to_cdb->result;
	strcpy(IntUnitCDB.LabelOfSupplier, int_to_cdb->LabelOfSupplier);
	IntUnitCDB.valid = TRUE;
	return;
}

void CDBControlFP(FPCDB *fp_to_cdb)
{

	FPUnitCDB.busy = fp_to_cdb->busy;
	FPUnitCDB.next = fp_to_cdb->next;
	FPUnitCDB.OPCODE = fp_to_cdb->OPCODE;
	FPUnitCDB.operand1 = fp_to_cdb->operand1;
	FPUnitCDB.operand2 = fp_to_cdb->operand2;
	FPUnitCDB.result = fp_to_cdb->result;
	strcpy(FPUnitCDB.LabelOfSupplier, fp_to_cdb->LabelOfSupplier);
	FPUnitCDB.valid = TRUE;
	return;
}

void CDBControlLoad(LoadCDB *load_to_cdb)
{
	LoadUnitCDB.busy = load_to_cdb->busy;
	LoadUnitCDB.next = load_to_cdb->next;
	LoadUnitCDB.OPCODE = load_to_cdb->OPCODE;
	LoadUnitCDB.operand1 = load_to_cdb->operand1;
	LoadUnitCDB.operand2 = load_to_cdb->operand2;
	LoadUnitCDB.result = load_to_cdb->result;
	strcpy(LoadUnitCDB.LabelOfSupplier, load_to_cdb->LabelOfSupplier);
	LoadUnitCDB.valid = TRUE;
	return;
}