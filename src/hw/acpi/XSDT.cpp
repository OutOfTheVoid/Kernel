#include <hw/acpi/XSDT.h>

#include <util/string/String.h>

#include <mm/KVMap.h>
#include <mm/paging/PageTable.h>

#include <system/func/KPrintf.h>

bool HW::ACPI::XSDT :: Validated = false;
uint32_t HW::ACPI::XSDT :: TableCount = 0;

HW::ACPI::XSDT :: XSDTable * HW::ACPI::XSDT :: Table = 0;
uint32_t HW::ACPI::XSDT :: TableLength = 0;

void HW::ACPI::XSDT :: Init ( void * XSDTAddress )
{
	
	if ( Validated )
		return;
	
	Table = reinterpret_cast <XSDTable *> ( mm_kvmap ( XSDTAddress, 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( Table == NULL )
		return;
	
	TableLength = ( reinterpret_cast <uint32_t> ( XSDTAddress ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( XSDTAddress ) & 0xFFFFF000 );
	
	if ( TableLength > 0x2000 )
	{
		
		mm_kvunmap ( Table );
		
		Table = reinterpret_cast <XSDTable *> ( mm_kvmap ( XSDTAddress, TableLength, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( Table == NULL )
			return;
		
	}
	
	if ( ! ACPITable :: VerifyTable ( & Table -> Header ) )
	{
		
		mm_kvunmap ( Table );
		
		return;
		
	}
	
	TableCount = ( Table -> Header.Length - sizeof ( HW::ACPI::ACPITable :: ACPITableHeader ) ) / 8;
	
	Validated = true;
	
};

void HW::ACPI::XSDT :: Discard ()
{
	
	if ( ! Validated )
		return;
	
	mm_kvunmap ( Table );
	
	Validated = false;
	
	Table = NULL;
	
};

bool HW::ACPI::XSDT :: Valid ()
{
	
	return Validated;
	
};

void * HW::ACPI::XSDT :: FindTable ( const char * Name )
{
	
	HW::ACPI::ACPITable :: ACPITableHeader * SearchTable;
	uint32_t SearchTableLength;
	
	for ( uint32_t I = 0; I < TableCount; I ++ )
	{
		
		if ( Table -> SDTableBase [ I ] > 0xFFFFFFFF )
			continue;
		
		SearchTable = reinterpret_cast <HW::ACPI::ACPITable :: ACPITableHeader *> ( mm_kvmap ( reinterpret_cast <void *> ( Table -> SDTableBase [ I ] ), 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
		SearchTableLength = ( reinterpret_cast <uint32_t> ( SearchTable ) + SearchTable -> Length ) - ( reinterpret_cast <uint32_t> ( SearchTable ) & 0xFFFFF000 );
		
		if ( SearchTableLength > 0x2000 )
		{
			
			mm_kvunmap ( SearchTable );
			
			SearchTable = reinterpret_cast <HW::ACPI::ACPITable :: ACPITableHeader *> ( mm_kvmap ( reinterpret_cast <void *> ( Table -> SDTableBase [ I ] ), SearchTableLength, MM::Paging::PageTable :: Flags_Writeable ) );
			
			if ( SearchTable == NULL )
				return NULL;
			
		}
		
		if ( ( strncmp ( SearchTable -> Signature, Name, 4 ) == 0 ) && ( SearchTable -> Length > sizeof ( HW::ACPI::ACPITable :: ACPITableHeader ) ) )
		{
			
			mm_kvunmap ( SearchTable );
			
			return reinterpret_cast <void *> ( Table -> SDTableBase [ I ] );
			
		}
		
		mm_kvunmap ( SearchTable );
		
	}
	
	return NULL;
	
};
