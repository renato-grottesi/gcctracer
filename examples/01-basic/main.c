#include <gcctrace.h>
#include <stdio.h>
#include <stdlib.h>

inline unsigned int gcd(unsigned int u, unsigned int v)
{
	return (v != 0)?gcd(v, u%v):u;
}

unsigned int readnum()
{
	return ((unsigned int)rand()) % 4096;
}

int main(void)
{
	unsigned int a = 0;
	unsigned int b = 0;
	unsigned int res = 0;

	srand(1);
	a = readnum();
	b = readnum();
	res = gcd(a, b);
	printf("gcd(%d, %d) = %d\n", a, b, res);

	return 0;
}
