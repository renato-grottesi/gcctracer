#include <gcctrace.h>
#include <stdio.h>
#include <stdlib.h>

inline unsigned int gcd(unsigned int u, unsigned int v)
{
	if (v != 0)
	{
		unsigned int* res = calloc(0, sizeof(unsigned int));
		unsigned int res_val = 0;
		*res = gcd(v, u%v);
		res_val = *res;
		free(res);
		return res_val;
	}
	return u;
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

