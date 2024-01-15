#include "mymalloc.c"

#ifndef MYMALLOC_H   // If MYMALLOC_H is not defined,
#define MYMALLOC_H   // define MYMALLOC_H, and include the following content.

// Content of the header file

#endif // Ends the conditional inclusion started by #ifndef.


#define malloc(X) mymalloc(X, __FILE__, __LINE__)
#define free(X) myfree(X, __FILE__, __LINE__)


void myfree(void* ptr, const char* file, int line);
void *mymalloc(int bytes, const char* file, int line);
void connector();
int memCleared();