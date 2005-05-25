#include <stdlib.h>
#include <stdio.h>

#define MAX_E 1024
#define MD "MEMORY_DEBUG: "

#undef malloc
#undef realloc
#undef free

struct {
	void *addr;
	int size;
} l[MAX_E];

void *__malloc(int size, char *fil, char * func, int line)
{
	int i;
	void *p;
	
	for (i=0; i<MAX_E;i++)
		if (l[i].addr == 0)
			break;

	p = malloc(size);
	l[i].addr = p;
	l[i].size = size;
	printf(MD "[%s:%s:%d] New chunk (%d) of %d bytes at %p\n", fil, func, line, i, size, p);
	return p;
}

void *__realloc(unsigned char *p, int size, char *fil, char * func, int line)
{
	int i;
	unsigned char *p1;
	
	for (i=0; i<MAX_E;i++)
		if (l[i].addr == p)
			break;
	
	p1 = realloc(p, size);
	printf(MD "[%s:%s:%d] resized chunk (%d) of %d bytes to %d bytes at %p was %p\n", fil, func, line, i, l[i].size, size, p1, p);
	
	l[i].size = size;
	l[i].addr = p1;
	return p1;
}

void __free(void *p, char *fil, char * func, int line)
{
	int i,j, sum;
	for (i=0; i<MAX_E;i++)
		if (l[i].addr == p)
			break;
	
	if(i==1024) {
	//:	printf(MD "[%s:%s:%d] Unbalanced free()\n", fil, func, line);
		return;
	}
	l[i].addr = 0;
	for (j=0, sum=0; j<MAX_E;j++)
		if (l[j].addr != 0)
			sum++;

	printf(MD "[%s:%s:%d] Freed chunk (%d) of %d bytes at %p, %d remainig\n", fil, func, line, i, l[i].size, p, sum);
	l[i].size = 0;
	free(p);
}

void list_table()
{
	int i;
	for (i=0; i<MAX_E;i++) {
		if (l[i].addr == 0) continue;
		printf(MD "Chunk (%d) of %d bytes at %p\n", i, l[i].size, l[i].addr);
	}
}
