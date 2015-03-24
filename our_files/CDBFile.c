#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"


void CDBControlInt(IntCDB *int_to_cdb)
{
	if (int_to_cdb->numOfRobSupplier){
		IntUnitCDB.result = int_to_cdb->result;
		IntUnitCDB.numOfRobSupplier = int_to_cdb->numOfRobSupplier;
		IntUnitCDB.valid = TRUE;
	}
	else
		FPUnitCDBADD.valid = FALSE;
	
	return;
}

void CDBControlFPADD(FPCDB *fp_to_cdb)
{
	if (fp_to_cdb->numOfRobSupplier){
		FPUnitCDBADD.result = fp_to_cdb->result;
		FPUnitCDBADD.numOfRobSupplier = fp_to_cdb->numOfRobSupplier;
		FPUnitCDBADD.valid = TRUE;
	}
	else
		FPUnitCDBADD.valid = FALSE;

	return;
}

void CDBControlFPMULL(FPCDB *fp_to_cdb)
{
	if (fp_to_cdb->numOfRobSupplier){
		FPUnitCDBMULL.result = fp_to_cdb->result;
		FPUnitCDBMULL.numOfRobSupplier = fp_to_cdb->numOfRobSupplier;
		FPUnitCDBMULL.valid = TRUE;
	}
	else
		FPUnitCDBADD.valid = FALSE;
	return;
}
void CDBControlLoad(LoadCDB *load_to_cdb)
{
	if (load_to_cdb->numOfRobSupplier){
		LoadUnitCDB.result = load_to_cdb->result;
		LoadUnitCDB.valid = TRUE;
		LoadUnitCDB.numOfRobSupplier = load_to_cdb->numOfRobSupplier;
	}
	else
		LoadUnitCDB.valid = FALSE;
	return;
}

void CDBUpdateRob(){
	
	robLine *iter = robLines;
	int flage = ((LoadUnitCDB.numOfRobSupplier) | (IntUnitCDB.numOfRobSupplier) | (FPUnitCDBADD.numOfRobSupplier) | (FPUnitCDBMULL.numOfRobSupplier));

	while (iter != NULL && flage){
		
		if (iter->numRob == LoadUnitCDB.numOfRobSupplier){
			iter->done = TRUE;
			iter->intValue = LoadUnitCDB.result;
		}

		if (iter->numRob == IntUnitCDB.numOfRobSupplier){
			iter->done = TRUE;
			iter->intValue = IntUnitCDB.result;
		}

		if (iter->numRob == FPUnitCDBADD.numOfRobSupplier){
			iter->done = TRUE;
			iter->intValue = FPUnitCDBADD.result;
		}

		if (iter->numRob == FPUnitCDBMULL.numOfRobSupplier){
			iter->done = TRUE;
			iter->fpValue = FPUnitCDBMULL.result;
		}

		iter = iter->next;
	}
}

void CDBUpdateRS(){

	IntReservationStation_Line *IntRsLine = NULL;
	FpReservationStation_Line  *FPRsLine = NULL;

	if (IntUnitCDB.valid == TRUE) {

		IntRsLine = IntReservationStation;
		while (IntRsLine != NULL && IntRsLine->NumOfRightOperands<2){

			if (IntRsLine->Qj == IntUnitCDB.numOfRobSupplier){
				IntRsLine->Vj = IntUnitCDB.result;
				IntRsLine->NumOfRightOperands++;
			}
			if (IntRsLine->Qk == IntUnitCDB.numOfRobSupplier){
				IntRsLine->Vk = IntUnitCDB.result;
				IntRsLine->NumOfRightOperands++;
			}

			IntRsLine = IntRsLine->next;
		}
	}


	if (FPUnitCDBMULL.valid == TRUE) {

		FPRsLine = FpReservationStation_MUL;
		while (FPRsLine != NULL && FPRsLine->NumOfRightOperands<2){

			if (FPRsLine->Qj == FPUnitCDBMULL.numOfRobSupplier){
				FPRsLine->Vj = FPUnitCDBMULL.result;
				FPRsLine->NumOfRightOperands++;
			}
			if (FPRsLine->Qk == FPUnitCDBMULL.numOfRobSupplier){
				FPRsLine->Vk = FPUnitCDBMULL.result;
				FPRsLine->NumOfRightOperands++;
			}
		}
	}


	if (FPUnitCDBADD.valid == TRUE) {

		FPRsLine = FpReservationStation_ADD;
		while (FPRsLine != NULL && FPRsLine->NumOfRightOperands<2){

			if (FPRsLine->Qj == FPUnitCDBADD.numOfRobSupplier){
				FPRsLine->Vj = FPUnitCDBADD.result;
				FPRsLine->NumOfRightOperands++;
			}
			if (FPRsLine->Qk == FPUnitCDBADD.numOfRobSupplier){
				FPRsLine->Vk = FPUnitCDBADD.result;
				FPRsLine->NumOfRightOperands++;
			}
		}
	}

}

