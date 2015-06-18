#include <system/func/PMalloc.h>
#include <system/func/KPrintf.h>

#include <mm/paging/AddressSpace.h>
#include <mm/paging/PFA.h>
#include <mm/paging/PageTable.h>

#include <math/bitmath.h>

#include <stdint.h>

void __system_func_pmallocFailure ( void * Virtual, uint32_t Pages );

void * system_func_pmalloc ( uint32_t Pages, uint32_t PTFlags, uint32_t Hint )
{
	
	void * Virtual = NULL;
	
	void * Physical;
	
	uint32_t PhysSize = 0;
	
	uint32_t Error;
	
	if ( ( MM::Paging::PFA :: GetFreeKB () >> 2 ) < Pages )
		return NULL;
	
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Alloc ( Pages * 0x1000, & Virtual, & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kAlloc_Error_None )
		return NULL;
	
	if ( Hint == PMALLOC_PHSCHEME_DEFAULT )
		Hint = PMALLOC_PHSCHEME_BINARY;
	
	void * VCopy = Virtual;
	uint32_t PagesCopy = Pages;
	uint32_t i;
	
	switch ( Hint )
	{
		
	case PMALLOC_PHSCHEME_BLOCK:
	
		PhysSize = math_bitmath_nextPowerOfTwo ( Pages );
		
		if ( MM::Paging::PFA :: Alloc ( PhysSize << 12, & Physical ) )
		{
			
			MM::Paging::PageTable :: SetKernelRegionMapping ( reinterpret_cast <uint32_t> ( Virtual ), reinterpret_cast <uint32_t> ( Physical ), Pages << 12, PTFlags | MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
			
			return Virtual;
			
		}
		
	case PMALLOC_PHSCHEME_BINARY:
		
		i = 0;
		
		{
			
			PhysSize = Pages;
			
			while ( PhysSize )
			{
				
				if ( ( PhysSize & 0x01 ) != 0 )
				{
					
					if ( ! MM::Paging::PFA :: Alloc ( 0x1000 << i, & Physical ) )
						break;
					
					MM::Paging::PageTable :: SetKernelRegionMapping ( reinterpret_cast <uint32_t> ( Virtual ), reinterpret_cast <uint32_t> ( Physical ), 0x1000 << i, PTFlags | MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
					
					Virtual = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Virtual ) + ( 0x1000 << i ) );
					Pages -= ( 1 << i );
					
				}
				
				i ++;
				PhysSize >>= 1;
				
			}
			
			if ( PhysSize == 0 )
				return VCopy;
			
		}
		
	case PMALLOC_PHSCHEME_DISTRIBUTE:
	default:
		
		if ( Hint != PMALLOC_PHSCHEME_BINARY )
		{
			
			PhysSize = Pages;
			PagesCopy = Pages;
			VCopy = Virtual;
			
		}
		
		for ( i = 0; i < Pages; i ++ )
		{
			
			if ( ! MM::Paging::PFA :: Alloc ( 0x1000, & Physical ) )
			{
				
				__system_func_pmallocFailure ( VCopy, PagesCopy );
				
				return NULL;
				
			}
			
			MM::Paging::PageTable :: SetKernelMapping ( reinterpret_cast <uint32_t> ( Virtual ), reinterpret_cast <uint32_t> ( Physical ), PTFlags | MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
			Virtual = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Virtual ) + 0x1000 );
			
		}
		
		return VCopy;
		
	}
	
	return NULL;
	
};

void __system_func_pmallocFailure ( void * Virtual, uint32_t Pages )
{
	
	uint32_t Offset = 0;
	uint32_t DummyError = 0;
	
	while ( Pages != 0 )
	{
		
		uint32_t Phys = MM::Paging::PageTable :: KernelVirtualToPhysical ( reinterpret_cast <uint32_t> ( Virtual ) + Offset );
		uint32_t PhysSize = MM::Paging::PFA :: GetAllocationSize ( reinterpret_cast <void *> ( Phys ) );
		
		MM::Paging::PFA :: Free ( reinterpret_cast <void *> ( Phys ) );
		MM::Paging::PageTable :: ClearKernelRegionMapping ( reinterpret_cast <uint32_t> ( Virtual ) + Offset, PhysSize );
		
		Pages -= ( PhysSize >> 12 );
		Offset += PhysSize;
		
		}
	
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Free ( Virtual, & DummyError );
	
	};

void system_func_pfree ( void * Base )
{
	
	MM::Paging::AddressSpace * KernelAddressSpace = MM::Paging::AddressSpace :: RetrieveKernelAddressSpace ();
	
	uint32_t Error = 0;
	uint32_t Length = 0;
	uint32_t Offset = 0;
	
	KernelAddressSpace -> GetAllocationSize ( Base, & Length, & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kGetAllocationSize_Error_None )
		return;
	
	while ( Offset != Length )
	{
		
		uint32_t Phys = MM::Paging::PageTable :: KernelVirtualToPhysical ( reinterpret_cast <uint32_t> ( Base ) + Offset );
		uint32_t PhysSize = MM::Paging::PFA :: GetAllocationSize ( reinterpret_cast <void *> ( Phys ) );
		
		MM::Paging::PFA :: Free ( reinterpret_cast <void *> ( Phys ) );
		MM::Paging::PageTable :: ClearKernelRegionMapping ( reinterpret_cast <uint32_t> ( Base ) + Offset, PhysSize );
		
		Offset += PhysSize;
		
	}
	
	KernelAddressSpace -> Free ( Base, & Error );
	
};

uint32_t system_func_psize ( void * Base )
{
	
	MM::Paging::AddressSpace * KernelAddressSpace = MM::Paging::AddressSpace :: RetrieveKernelAddressSpace ();
	
	uint32_t Error = 0;
	uint32_t Length = 0;
	
	KernelAddressSpace -> GetAllocationSize ( Base, & Length, & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kGetAllocationSize_Error_None )
		return 0;
	
	return Length;
	
};

