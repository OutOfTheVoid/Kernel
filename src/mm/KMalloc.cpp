#include <mm/KMalloc.h>
#include <mm/LibAlloc.h>
#include <system/func/kprintf.h>
#include <system/func/pmalloc.h>

#include <cpputil/Unused.h>

#include <stddef.h>
#include <stdint.h>

int liballoc_lock ()
{
	
	return 0;
	
};

int liballoc_unlock ()
{
	
	return 0;
	
};

void * liballoc_alloc ( size_t Pages )
{
	
	void * Addr = system_func_pmalloc ( static_cast <uint32_t> ( Pages ) );
	
	return Addr;
	
};

int liballoc_free ( void * Address, size_t Length )
{
	
	UNUSED ( Length );
	
	system_func_pfree ( Address );
	
	return 0;
	
};


#include "LibAlloc.c"
