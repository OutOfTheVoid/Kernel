#include <hw/acpi/XSDT.h>

#include <util/string/String.h>

#include <mm/paging/AddressSpace.h>
#include <mm/paging/PageTable.h>

bool HW::ACPI::XSDT :: Validated = false;
uint32_t HW::ACPI::XSDT :: TableCount = 0;

HW::ACPI::XSDT :: XSDTable * HW::ACPI::XSDT :: Table = 0;

void HW::ACPI::XSDT :: Init ( void * XSDTAddress )
{
	
	if ( Validated )
		return;
	
	uint32_t Error;
	
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Alloc ( 0x1000, reinterpret_cast <void **> ( & Table ), & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kAlloc_Error_None )
		return;
	
	MM::Paging::PageTable :: SetKernelMapping ( reinterpret_cast <uint32_t> ( Table ), reinterpret_cast <uint32_t> ( XSDTAddress ), MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
	
	if ( ! ACPITable :: VerifyTable ( & Table -> Header ) )
	{
		
		MM::Paging::PageTable :: ClearKernelMapping ( reinterpret_cast <uint32_t> ( Table ) );
		MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Free ( reinterpret_cast <void *> ( Table ), & Error );
		
		return;
		
	}
	
	Validated = true;
	
	TableCount = ( Table -> Header.Length - sizeof ( HW::ACPI::ACPITable :: ACPITableHeader ) ) / sizeof ( uint64_t );
	
};

void HW::ACPI::XSDT :: Discard ()
{
	
	uint32_t Error;
	
	if ( ! Validated )
		return;
	
	MM::Paging::PageTable :: ClearKernelMapping ( reinterpret_cast <uint32_t> ( Table ) );
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Free ( reinterpret_cast <void *> ( Table ), & Error );
	
	Table = NULL;
	
};

bool HW::ACPI::XSDT :: Valid ()
{
	
	return Validated;
	
};

void * HW::ACPI::XSDT :: FindTable ( const char * Name )
{
	
	if ( ! Validated )
		return NULL;
	
	for ( uint32_t I = 0; I < TableCount; I ++ )
	{
		
		HW::ACPI::ACPITable :: ACPITableHeader * SearchTable = reinterpret_cast <HW::ACPI::ACPITable :: ACPITableHeader *> ( ( & Table -> SDTableBase ) [ I ] );
		
		if ( strncmp ( SearchTable -> Signature, Name, 4 ) == 0 )
			return reinterpret_cast <void *> ( SearchTable );
		
	}
	
	return NULL;
	
};
