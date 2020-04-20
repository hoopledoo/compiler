int globalArray[10];

void callee(int x[], int y[]){
	return;
}

int main(int argc, char** argv){
	int localArray[10];
	int x;

	globalArray[0] = 100;
	x = globalArray[0]; 
	output(x); 

	x = input();
	output(x);

	/*
	localArray[0] = 1;
	output(localArray[0]);
	x = localArray[0];
	*/
	

	/* callee(globalArray, localArray); */
}