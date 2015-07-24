#include <mm/KVMap.h>

#include <mm/paging/AddressSpace.h>
#include <mm/paging/PageTable.h>

#include <system/func/kprintf.h>

void * mm_kvmap ( void * Physical, uint32_t Length, uint32_t Flags )
{
	
	uint32_t Error;
	void * Virtual;
	
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Alloc ( Length, reinterpret_cast <void **> ( & Virtual ), & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kAlloc_Error_None )
		return NULL;
	
	MM::Paging::PageTable :: SetKernelRegionMapping ( reinterpret_cast <uint32_t> ( Virtual ), reinterpret_cast <uint32_t> ( Physical ) & 0xFFFFF000, Length, Flags | MM::Paging::PageTable :: Flags_Present );
	
	return reinterpret_cast <void *> ( ( reinterpret_cast <uint32_t> ( Virtual ) & 0xFFFFF000 ) | ( reinterpret_cast <uint32_t> ( Physical ) & 0xFFF ) );
	
};

void mm_kvremap ( void * Virtual, void * Physical, uint32_t Flags )
{
	
	uint32_t Error;
	void * Base = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Virtual ) & 0xFFFFF000 );
	uint32_t Length;
	
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> GetAllocationSize ( Base, & Length, & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kGetAllocationSize_Error_None )
		return;
	
	MM::Paging::PageTable :: ClearKernelRegionMapping ( reinterpret_cast <uint32_t> ( Base ), Length );
	MM::Paging::PageTable :: SetKernelRegionMapping ( reinterpret_cast <uint32_t> ( Virtual ) & 0xFFFFF000, reinterpret_cast <uint32_t> ( Physical ) & 0xFFFFF000, Length, Flags | MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Cutsom_KMap );
	
};

void mm_kvunmap ( void * Virtual )
{
	
	uint32_t Error;
	void * Base = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Virtual ) & 0xFFFFF000 );
	uint32_t Length;
	
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> GetAllocationSize ( Base, & Length, & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kGetAllocationSize_Error_None )
		return;
	
	MM::Paging::PageTable :: ClearKernelRegionMapping ( reinterpret_cast <uint32_t> ( Base ), Length );
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Free ( Base, & Error );
	
};
