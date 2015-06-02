#include <hw/acpi/RSDT.h>

#include <util/string/String.h>

#include <mm/paging/PageTable.h>
#include <mm/KVMap.h>

bool HW::ACPI::RSDT :: Validated = false;
uint32_t HW::ACPI::RSDT :: TableCount = 0;

HW::ACPI::RSDT :: RSDTable * HW::ACPI::RSDT :: Table = 0;

void HW::ACPI::RSDT :: Init ( void * RSDTAddress )
{
	
	if ( Validated )
		return;
	
	Table = reinterpret_cast <RSDTable *> ( mm_kvmap ( RSDTAddress, 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( Table == NULL )
		return;
	
	uint32_t TableLength = ( reinterpret_cast <uint32_t> ( RSDTAddress ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( RSDTAddress ) & 0xFFFFF000 );
	
	if ( TableLength > 0x2000 )
	{
		
		mm_kvunmap ( Table );
		Table = reinterpret_cast <RSDTable *> ( mm_kvmap ( RSDTAddress, TableLength, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( Table == NULL )
			return;
		
	}
	
	if ( ! ACPITable :: VerifyTable ( & Table -> Header ) )
	{
		
		mm_kvunmap ( Table );
		
		return;
		
	}
	
	Validated = true;
	
	TableCount = ( Table -> Header.Length - sizeof ( HW::ACPI::ACPITable :: ACPITableHeader ) ) / 4;
	
};

void HW::ACPI::RSDT :: Discard ()
{
	
	if ( ! Validated )
		return;
	
	mm_kvunmap ( Table );
	
	Validated = false;
	
	Table = NULL;
	
};

bool HW::ACPI::RSDT :: Valid ()
{
	
	return Validated;
	
};

void * HW::ACPI::RSDT :: FindTable ( const char * Name )
{
	
	HW::ACPI::ACPITable :: ACPITableHeader * SearchTable;
	uint32_t SearchTableLength;
	
	for ( uint32_t I = 0; I < TableCount; I ++ )
	{
		
		SearchTable = reinterpret_cast <HW::ACPI::ACPITable :: ACPITableHeader *> ( mm_kvmap ( reinterpret_cast <void *> ( Table -> SDTableBase [ 0 ] ), 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
		SearchTableLength = ( reinterpret_cast <uint32_t> ( SearchTable ) + SearchTable -> Length ) - ( reinterpret_cast <uint32_t> ( SearchTable ) & 0xFFFFF000 );
		
		if ( SearchTableLength > 0x2000 )
		{
			
			mm_kvunmap ( SearchTable );
			
			SearchTable = reinterpret_cast <HW::ACPI::ACPITable :: ACPITableHeader *> ( mm_kvmap ( reinterpret_cast <void *> ( Table -> SDTableBase [ 0 ] ), SearchTableLength, MM::Paging::PageTable :: Flags_Writeable ) );
			
			if ( SearchTable == NULL )
				return NULL;
			
		}
		
		
		
		if ( strncmp ( SearchTable -> Signature, Name, 4 ) == 0 )
		{
			
			mm_kvunmap ( SearchTable );
			
			return SearchTable;
			
		}
		
		mm_kvunmap ( SearchTable );
		
	}
	
	return NULL;
	
};