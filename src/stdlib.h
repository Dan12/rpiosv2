#include <stdint.h>

#if !defined(_STDLIB)
#define _STDLIB

#define MIN(x,y) ((x < y ? x : y))
#define MAX(x,y) ((x < y ? y : x))

// convert an int into a string
char * itoa(int i, int base);
// convert a string into an int
int atoi(char * num);

void mcpy(void* dest, const void* src, int bytes);

void mzero(void* dest, int bytes);

#endif // _STDLIB
