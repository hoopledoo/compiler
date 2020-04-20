int globalArray[10];

void callee(int x[], int y[]){
	return;
}

int main(void){
	int localArray[10];
	int x;

	globalArray[0] = 1;
	x = globalArray[0];

	callee(globalArray, localArray);
}