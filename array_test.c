int globalVar;
int globalArray[10];

void function(int paramVar, int paramArray[]){
	int localVar;
	int localInt;
	int localArray[10];

	localVar = ((2*2)+4)/(3-2);
	localInt= 1;

	localInt= globalVar;
	localInt= globalArray[1];

	localInt= paramVar;
	localInt= paramArray[2]; /* work because pointers */
	localInt= localArray[3];

	globalVar = globalArray[2];
	localVar = globalArray[3];
	globalVar = localArray[3];

	localArray[0] = 1;
	paramArray[0] = 2;
	globalArray[0] = 3;

	/*
	localArray[0] = localVar;
	localArray[1] = globalVar;
	globalArray[0] = localVar;
	globalArray[1] = globalVar;
	*/

	/*
	localArray[4] = globalArray[4];
	globalArray[5] = localArray[5];
	*/
}
