#include <stdio.h>

int a=2;

void test(int a){
	printf("in test a=%d", a);
	a = a;
	printf("in test, after assign a=%d", a);
}

int main(){
	printf("in main a=%d",a);
	test(1);
	printf("in main, after call a=%d",a);
	return 0;
}
