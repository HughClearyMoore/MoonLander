#include <stdio.h>

#include <DynArray.h>

static void IntPrinter(int* i, int* count)
{
	printf("Array['%d'] = %d\n", *i, (*count)++);
}

int main(int argc, char** argv)
{
	DynArray arr = DynArrayCreate(sizeof(int), 16, NULL);

	for (int i = 0; i < 10; ++i)
	{
		DynArrayPush(&arr, &i);
	}

	int count = 0;
	DynArrayForEach(&arr, &IntPrinter, &count);

	DynArrayDestroy(&arr);

	return 0x0;
}