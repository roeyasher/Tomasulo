#define _CRT_SECURE_NO_DEPRECATE
#include "shared.h"


void CDBControlInt(Instruction *int_to_cdb)
{
	int j = 0;
	if (int_to_cdb->numOfRobSupplier || int_to_cdb->STLDIns){
		IntUnitCDB.result = int_to_cdb->result;
		IntUnitCDB.numOfRobSupplier = int_to_cdb->numOfRobSupplier;
		IntUnitCDB.STLDIns = int_to_cdb->STLDIns;
		IntUnitCDB.num = int_to_cdb->num;
		IntUnitCDB.valid = TRUE;
		trace[IntUnitCDB.num].CDB = cycle;
		memset(int_to_cdb, 0, sizeof(Instruction));
	}
	else
		FPUnitCDBADD.valid = FALSE;
	
	return;
}

void CDBControlFPADD(Instruction *fp_to_cdb)
{
	int j = 0;
	if (fp_to_cdb->numOfRobSupplier){
		FPUnitCDBADD.result = fp_to_cdb->result;
		FPUnitCDBADD.numOfRobSupplier = fp_to_cdb->numOfRobSupplier;
		FPUnitCDBADD.valid = TRUE;
		FPUnitCDBADD.num = fp_to_cdb->num;
		trace[FPUnitCDBADD.num].CDB = cycle;
		memset(fp_to_cdb, 0, sizeof(Instruction));
	}
	else
		FPUnitCDBADD.valid = FALSE;

	return;
}

void CDBControlFPMULL(Instruction *fp_to_cdb)
{
	int j = 0;
	if (fp_to_cdb->numOfRobSupplier){
		FPUnitCDBMULL.result = fp_to_cdb->result;
		FPUnitCDBMULL.numOfRobSupplier = fp_to_cdb->numOfRobSupplier;
		FPUnitCDBMULL.valid = TRUE;
		FPUnitCDBMULL.num = fp_to_cdb->num;
		trace[FPUnitCDBMULL.num].CDB = cycle;
		memset(fp_to_cdb, 0, sizeof(Instruction));
	}
	else
		FPUnitCDBADD.valid = FALSE;
	return;
}
void CDBControlLoad(Instruction *load_to_cdb)
{
	int j = 0;
	if (load_to_cdb->numOfRobSupplier){
		LoadUnitCDB.result = load_to_cdb->result;
		LoadUnitCDB.valid = TRUE;
		LoadUnitCDB.numOfRobSupplier = load_to_cdb->numOfRobSupplier;
		LoadUnitCDB.num = load_to_cdb->num;
		trace[LoadUnitCDB.num].CDB = cycle;
		memset(load_to_cdb, 0, sizeof(Instruction));
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
			iter->fpValue = LoadUnitCDB.result;
		}

		if (iter->numRob == IntUnitCDB.numOfRobSupplier){
			iter->done = TRUE;
			iter->intValue = IntUnitCDB.result;
		}

		if (iter->numRob == FPUnitCDBADD.numOfRobSupplier){
			iter->done = TRUE;
			iter->fpValue = FPUnitCDBADD.result;
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
	LoadBuffer  *LDBuffLine = NULL;
	StoreBuffer  *STBuffLine = NULL;



	// Update result from load - ST, FPMULL, FPADD
	if (LoadUnitCDB.valid == TRUE) {

		STBuffLine = StoreBufferResarvation;
		while ((STBuffLine) != NULL){

			if (STBuffLine->Qj == LoadUnitCDB.numOfRobSupplier && (STBuffLine->NumOfRightOperands < 1)){
				STBuffLine->Vj = LoadUnitCDB.result;
				STBuffLine->NumOfRightOperands++;
			}
			STBuffLine = STBuffLine->next;
		}

		FPRsLine = FpReservationStation_MUL;
		while (FPRsLine != NULL && FPRsLine->NumOfRightOperands<2){

			if (FPRsLine->Qj == LoadUnitCDB.numOfRobSupplier){
				FPRsLine->Vj = LoadUnitCDB.result;
				FPRsLine->NumOfRightOperands++;
			}
			if (FPRsLine->Qk == LoadUnitCDB.numOfRobSupplier){
				FPRsLine->Vk = LoadUnitCDB.result;
				FPRsLine->NumOfRightOperands++;
			}
			FPRsLine = FPRsLine->next;
		}

		FPRsLine = FpReservationStation_ADD;
		while (FPRsLine != NULL && FPRsLine->NumOfRightOperands<2){

			if (FPRsLine->Qj == LoadUnitCDB.numOfRobSupplier){
				FPRsLine->Vj = LoadUnitCDB.result;
				FPRsLine->NumOfRightOperands++;
			}
			if (FPRsLine->Qk == LoadUnitCDB.numOfRobSupplier){
				FPRsLine->Vk = LoadUnitCDB.result;
				FPRsLine->NumOfRightOperands++;
			}
			FPRsLine = FPRsLine->next;
		}
	}


	// Update result from INTALU - INTALU
	if (IntUnitCDB.valid == TRUE) {

		if (IntUnitCDB.STLDIns == TRUE){

			STBuffLine = StoreBufferResarvation;
			while ((STBuffLine) != NULL){

				if ((STBuffLine->addressReady == FALSE) && IntUnitCDB.num == STBuffLine->num){
					STBuffLine->address = IntUnitCDB.result;
					STBuffLine->addressReady = TRUE;
				}
				STBuffLine = STBuffLine->next;
			}
			
			LDBuffLine = LoadBufferResarvation;
			while ((LDBuffLine) != NULL){

				if ((LDBuffLine->addressReady == FALSE) && IntUnitCDB.num == LDBuffLine->num){
					LDBuffLine->address = IntUnitCDB.result;
					LDBuffLine->addressReady = TRUE;
				}
				LDBuffLine = LDBuffLine->next;
			}


		}
		else{
			IntRsLine = IntReservationStation;
			while (IntRsLine != NULL){

				if (IntRsLine->Qj == IntUnitCDB.numOfRobSupplier  && IntRsLine->NumOfRightOperands < 2){
					IntRsLine->Vj = IntUnitCDB.result;
					IntRsLine->NumOfRightOperands++;
				}
				if (IntRsLine->Qk == IntUnitCDB.numOfRobSupplier  && IntRsLine->NumOfRightOperands < 2){
					IntRsLine->Vk = IntUnitCDB.result;
					IntRsLine->NumOfRightOperands++;
				}
				IntRsLine = IntRsLine->next;
			}
		}
	}

	// Update from FPMULL - FPMULL, FPADD, LD
	if (FPUnitCDBMULL.valid == TRUE) {

		STBuffLine = StoreBufferResarvation;
		while (STBuffLine != NULL){

		if (STBuffLine->Qj == FPUnitCDBMULL.numOfRobSupplier && (STBuffLine->NumOfRightOperands < 1)){
				STBuffLine->Vj = FPUnitCDBMULL.result;
				STBuffLine->NumOfRightOperands++;
			}
			STBuffLine = STBuffLine->next;
		}

		FPRsLine = FpReservationStation_MUL;
		while (FPRsLine != NULL){

			if (FPRsLine->Qj == FPUnitCDBMULL.numOfRobSupplier  && FPRsLine->NumOfRightOperands<2){
				FPRsLine->Vj = FPUnitCDBMULL.result;
				FPRsLine->NumOfRightOperands++;
			}
			if (FPRsLine->Qk == FPUnitCDBMULL.numOfRobSupplier  && FPRsLine->NumOfRightOperands<2){
				FPRsLine->Vk = FPUnitCDBMULL.result;
				FPRsLine->NumOfRightOperands++;
			}
			FPRsLine = FPRsLine->next;
		}

		FPRsLine = FpReservationStation_ADD;
		while (FPRsLine != NULL){

			if (FPRsLine->Qj == FPUnitCDBMULL.numOfRobSupplier && FPRsLine->NumOfRightOperands<2){
				FPRsLine->Vj = FPUnitCDBMULL.result;
				FPRsLine->NumOfRightOperands++;
			}
			if (FPRsLine->Qk == FPUnitCDBMULL.numOfRobSupplier && FPRsLine->NumOfRightOperands<2){
				FPRsLine->Vk = FPUnitCDBMULL.result;
				FPRsLine->NumOfRightOperands++;
			}
			FPRsLine = FPRsLine->next;
		}
	}

	// Update from FPADD - FPMULL, FPADD, LD
	if (FPUnitCDBADD.valid == TRUE) {

		STBuffLine = IntReservationStation;
		while ((STBuffLine) != NULL){

			if (STBuffLine->Qj == FPUnitCDBADD.numOfRobSupplier && (STBuffLine->NumOfRightOperands < 1)){
				STBuffLine->Vj = FPUnitCDBADD.result;
				STBuffLine->NumOfRightOperands++;
			}
			STBuffLine = STBuffLine->next;
		}

		FPRsLine = FpReservationStation_MUL;
		while (FPRsLine != NULL){

			if (FPRsLine->Qj == FPUnitCDBADD.numOfRobSupplier  && FPRsLine->NumOfRightOperands<2){
				FPRsLine->Vj = FPUnitCDBADD.result;
				FPRsLine->NumOfRightOperands++;
			}
			if (FPRsLine->Qk == FPUnitCDBADD.numOfRobSupplier  && FPRsLine->NumOfRightOperands<2){
				FPRsLine->Vk = FPUnitCDBADD.result;
				FPRsLine->NumOfRightOperands++;
			}
			FPRsLine = FPRsLine->next;
		}

		FPRsLine = FpReservationStation_ADD;
		while (FPRsLine != NULL ){

			if (FPRsLine->Qj == FPUnitCDBADD.numOfRobSupplier && FPRsLine->NumOfRightOperands<2){
				FPRsLine->Vj = FPUnitCDBADD.result;
				FPRsLine->NumOfRightOperands++;
			}
			if (FPRsLine->Qk == FPUnitCDBADD.numOfRobSupplier && FPRsLine->NumOfRightOperands<2){
				FPRsLine->Vk = FPUnitCDBADD.result;
				FPRsLine->NumOfRightOperands++;
			}
			FPRsLine = FPRsLine->next;
		}
	}

}

