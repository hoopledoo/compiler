int globalArray[10];

void callee(int x[]){
	return;
}

int main(void){
	int x;

	globalArray[0] = 1;
	x = globalArray[0];

	callee(globalArray);
}