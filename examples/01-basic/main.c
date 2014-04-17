/****************************************************************************
 * Copyright (C) 2014 by Renato Grottesi                                    *
 *                                                                          *
 * This file is part of gcctrace.                                           *
 *                                                                          *
 *   gcctrace is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   gcctrace is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with gcctrace.                                           *
 *   If not, see <http://www.gnu.org/licenses/>.                            *
 ****************************************************************************/

/**
 * @file main.c
 * @author Renato Grottesi
 * @date 7 Apr 2014
 * @brief Main file to include gcctrace's functionalities
 */

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

unsigned int * readnum()
{
	unsigned int *res = malloc(sizeof(unsigned int));
	*res = ((unsigned int)rand()) % 4096;
	return res;
}

void freenum(unsigned int *num)
{
	free(num);
}

int main(void)
{
	unsigned int *a;
	unsigned int *b;
	unsigned int res = 0;

	srand(1);
	a = readnum();
	b = readnum();
	res = gcd(*a, *b);
	printf("gcd(%d, %d) = %d\n", *a, *b, res);

	freenum(a);
	freenum(b);

	return 0;
}

