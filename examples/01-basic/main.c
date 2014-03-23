#include <gcctrace.h>
#include <stdio.h>
#include <stdlib.h>

int gcd(int u, int v)
{
	return (v != 0)?gcd(v, u%v):u;
}

int readnum()
{
	int a;
	printf("Insert a number = ");
	scanf("%8d", &a);
	return a;
}

int main(void)
{
	int a = 0;
	int b = 0;
	int res = 0;

	a = readnum();
	b = readnum();
	res = gcd(a, b);
	printf("gcd(%d, %d) = %d\n", a, b, res);
}
