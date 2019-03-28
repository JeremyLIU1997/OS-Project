#include <stdio.h>
#include "test.h"

int ar[10];
int main() {
	
	test_func();
	for (int i = 0; i < 10; ++i)
	{
		printf("%d\n", ar[i]);
	}
}