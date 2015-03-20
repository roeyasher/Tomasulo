

typedef struct{

	int OPCODE;

	char label[LABEL_SIZE];					/*this is the label of the line. e.g 'ADD1'*/

	int Vj,Vk;								/*those are the values to operate on*/

	int NumOfRightOperands;					/*we need 2 operands to operate. this must be initialized to 0 and incremented by 1 when

											getting Vj,Vk so that 2 indicates that all operands are ready and intruction shall be executed*/

	char Qj[LABEL_SIZE],Qk[LABEL_SIZE];		/*labels in case of for result from another instruction*/

	BOOL busy;								/*is this line in use*/

	BOOL done;								/*did this line finish execution and can be evicted from Reservation Station*/

	BOOL inExecution;

	struct robLine *next;	/*next line in reservation station*/

	int issued;

}robLine;