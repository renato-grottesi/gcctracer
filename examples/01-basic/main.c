#include <gcctrace.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int gcd(unsigned int u, unsigned int v)
{
	return (v != 0)?gcd(v, u%v):u;
}

unsigned int readnum()
{
	srand(1);
	return ((unsigned int)rand()) % 4096;
}

int main(void)
{
	unsigned int a = 0;
	unsigned int b = 0;
	unsigned int res = 0;

	a = readnum();
	b = readnum();
	res = gcd(a, b);
	printf("gcd(%d, %d) = %d\n", a, b, res);
}
