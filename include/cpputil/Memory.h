#ifndef CPPUTIL_MEMORY_H
#define CPPUTIL_MEMORY_H

#include <stddef.h>

// General C++ memory operators. Allows for calling new/delete without placement syntax.
void * operator new ( size_t size );
void * operator new[] ( size_t size );
void operator delete ( void * p );
void operator delete[] ( void * p );

inline void * operator new ( size_t, void * p ) throw () { return p; };
inline void * operator new[] ( size_t, void * p ) throw () { return p; };
inline void operator delete  ( void *, void * ) throw () { };
inline void operator delete[] ( void *, void * ) throw () { };

#endif
