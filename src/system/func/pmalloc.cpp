#include <system/func/pmalloc.h>

#include <mm/paging/AddressSpace.h>
#include <mm/paging/PFA.h>
#include <mm/paging/PageTable.h>

void * system_func_pmalloc ( uint32_t Pages, uint32_t Hint )
{
	
	MM::Paging::AddressSpace :: AddressSpace * KernelAddressSpace = MM::Paging::AddressSpace :: ReteriveKernelSpace ();
	
	
	
};
