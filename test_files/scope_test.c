/* Expected output:
3 3  
4 4 
5 5  
5 4
5 3 */

int main(void){
	int a;
	int b;

	a=3;
	b=3;
	output(a);
	output(b);
	
	{
		int b;

		a=4;
		b=4;
		output(a);
		output(b);

		{
			int b;

			a=5;
			b=5;
			output(a);
			output(b);
		}
		output(a);
		output(b);
	}
	output(a);
	output(b);
}