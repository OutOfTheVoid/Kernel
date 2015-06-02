#include <hw/acpi/RSDT.h>

#include <util/string/String.h>

#include <mm/paging/AddressSpace.h>
#include <mm/paging/PageTable.h>

bool HW::ACPI::RSDT :: Validated = false;
uint32_t HW::ACPI::RSDT :: TableCount = 0;

HW::ACPI::RSDT :: RSDTable * HW::ACPI::RSDT :: Table = 0;

void HW::ACPI::RSDT :: Init ( void * RSDTAddress )
{
	
	if ( Validated )
		return;
	
	uint32_t Error;
	
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Alloc ( 0x1000, reinterpret_cast <void **> ( & Table ), & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kAlloc_Error_None )
		return;
	
	MM::Paging::PageTable :: SetKernelMapping ( reinterpret_cast <uint32_t> ( Table ), reinterpret_cast <uint32_t> ( RSDTAddress ), MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
	
	if ( ! ACPITable :: VerifyTable ( & Table -> Header ) )
	{
		
		MM::Paging::PageTable :: ClearKernelMapping ( reinterpret_cast <uint32_t> ( Table ) );
		MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Free ( reinterpret_cast <void *> ( Table ), & Error );
		
		return;
		
	}
	
	Validated = true;
	
	TableCount = ( Table -> Header.Length - sizeof ( HW::ACPI::ACPITable :: ACPITableHeader ) ) / sizeof ( uint32_t );
	
};

void HW::ACPI::RSDT :: Discard ()
{
	
	uint32_t Error;
	
	if ( ! Validated )
		return;
	
	MM::Paging::PageTable :: ClearKernelMapping ( reinterpret_cast <uint32_t> ( Table ) );
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Free ( reinterpret_cast <void *> ( Table ), & Error );
	
	Table = NULL;
	
};

bool HW::ACPI::RSDT :: Valid ()
{
	
	return Validated;
	
};

void * HW::ACPI::RSDT :: FindTable ( const char * Name )
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
