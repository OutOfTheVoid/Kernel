#include <mm/KMalloc.h>
#include <mm/LibAlloc.h>
#include <mm/PMalloc.h>

#include <system/func/kprintf.h>

#include <mt/synchronization/Spinlock.h>

#include <cpputil/Unused.h>

#include <stddef.h>
#include <stdint.h>

MT::Synchronization::Spinlock :: Spinlock_t liballoc_spinlock = MT::Synchronization::Spinlock :: Initializer ();

int liballoc_lock ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & liballoc_spinlock );
	
	return 0;
	
};

int liballoc_unlock ()
{
	
	MT::Synchronization::Spinlock :: Release ( & liballoc_spinlock );
	
	return 0;
	
};

void * liballoc_alloc ( size_t Pages )
{
	
	void * Addr = mm_pmalloc ( static_cast <uint32_t> ( Pages ) );
	
	return Addr;
	
};

int liballoc_free ( void * Address, size_t Length )
{
	
	UNUSED ( Length );
	
	mm_pfree ( Address );
	
	return 0;
	
};

#include "LibAlloc.c"
