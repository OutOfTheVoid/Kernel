#include <cpputil/Memory.h>

#include <mm/KMalloc.h>

void * operator new ( size_t size )
{
	
	return mm_kmalloc ( size );
	
};

void * operator new[] ( size_t size )
{
	
	return mm_kmalloc ( size );
	
};

void operator delete ( void * p )
{
	
	return mm_kfree ( p );
	
};

void operator delete[] ( void * p )
{
	
	return mm_kfree ( p );
	
};
