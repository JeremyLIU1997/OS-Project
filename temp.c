#include <stdio.h>

int main() {
	int a = 2;
	int* ptr = &a;
	(*ptr)++;
	printf("%d\n", *ptr);
}