#ifndef CPPUTIL_MEMORY_H
#define CPPUTIL_MEMORY_H

#include <stddef.h>

void * operator new ( size_t size );
void * operator new[] ( size_t size );
void operator delete ( void * p );
void operator delete[] ( void * p );

#endif
