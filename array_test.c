int globalVar;
int globalArray[10];

int main(int paramVar, int paramArray[]){
	int localVar;
	int b;
	int localArray[10];

	localVar = ((2*2)+4)/(3-2);
	b = 1;

	b = globalVar;
	b = globalArray[1];

	b = paramVar;
	b = paramArray[2];

	/* b = localArray[3]; */

	/*
	localArray[0] = localVar;
	localArray[1] = globalVar;
	globalArray[0] = localVar;
	globalArray[1] = globalVar;

	
	globalVar = globalArray[2];
	localVar = globalArray[3];
	globalVar = localArray[3];

	localArray[4] = globalArray[4];
	globalArray[5] = localArray[5];
	*/
}
