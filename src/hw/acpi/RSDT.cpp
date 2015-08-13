#include <hw/acpi/RSDT.h>

#include <util/string/String.h>

#include <mm/KVMap.h>
#include <mm/paging/PageTable.h>

#include <system/func/KPrintf.h>

bool HW::ACPI::RSDT :: Validated = false;
uint32_t HW::ACPI::RSDT :: TableCount = 0;

HW::ACPI::RSDT :: RSDTable * HW::ACPI::RSDT :: Table = 0;
uint32_t HW::ACPI::RSDT :: TableLength = 0;

void HW::ACPI::RSDT :: Init ( void * RSDTAddress )
{
	
	if ( Validated )
		return;
	
	Table = reinterpret_cast <RSDTable *> ( mm_kvmap ( RSDTAddress, 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( Table == NULL )
		return;
	
	TableLength = ( reinterpret_cast <uint32_t> ( RSDTAddress ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( RSDTAddress ) & 0xFFFFF000 );
	
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
	
	TableCount = ( Table -> Header.Length - sizeof ( HW::ACPI::ACPITable :: ACPITableHeader ) ) / 8;
	
	Validated = true;
	
};

bool HW::ACPI::RSDT :: Valid ()
{
	
	return Validated;
	
};

void * HW::ACPI::RSDT :: FindTable ( const char * Name )
{
	
	ACPITable :: ACPITableHeader * SearchTable;
	uint32_t SearchTableLength;
	
	for ( uint32_t I = 0; I < TableCount; I ++ )
	{
		
		SearchTable = reinterpret_cast <HW::ACPI::ACPITable :: ACPITableHeader *> ( mm_kvmap ( reinterpret_cast <void *> ( Table -> SDTableBase [ I ] ), 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( SearchTable == NULL )
			return NULL;
		
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

void HW::ACPI::RSDT :: FindTables ( const char * Name, void * Addresses [], uint32_t Max )
{
	
	if ( Max == 0 )
		return;
	
	ACPITable :: ACPITableHeader * SearchTable;
	uint32_t SearchTableLength;
	
	uint32_t Index = 0;
	
	uint32_t I;
	
	for ( I = 0; I < Max; I ++ )
		Addresses [ I ] = NULL;
	
	for ( I = 0; I < TableCount; I ++ )
	{
		
		SearchTable = reinterpret_cast <HW::ACPI::ACPITable :: ACPITableHeader *> ( mm_kvmap ( reinterpret_cast <void *> ( Table -> SDTableBase [ I ] ), 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( SearchTable == NULL )
			return;
		
		SearchTableLength = ( reinterpret_cast <uint32_t> ( SearchTable ) + SearchTable -> Length ) - ( reinterpret_cast <uint32_t> ( SearchTable ) & 0xFFFFF000 );
		
		if ( SearchTableLength > 0x2000 )
		{
			
			mm_kvunmap ( SearchTable );
			
			SearchTable = reinterpret_cast <HW::ACPI::ACPITable :: ACPITableHeader *> ( mm_kvmap ( reinterpret_cast <void *> ( Table -> SDTableBase [ I ] ), SearchTableLength, MM::Paging::PageTable :: Flags_Writeable ) );
			
			if ( SearchTable == NULL )
				return;
			
		}
		
		if ( ( strncmp ( SearchTable -> Signature, Name, 4 ) == 0 ) && ( SearchTable -> Length > sizeof ( HW::ACPI::ACPITable :: ACPITableHeader ) ) )
		{
			
			Addresses [ Index ] = reinterpret_cast <void *> ( Table -> SDTableBase [ I ] );
			Index ++;
			
			if ( Index == Max )
				return;
			
		}
		
		mm_kvunmap ( SearchTable );
		
	}
	
};
			
uint32_t HW::ACPI::RSDT :: CountTables ( const char * Name )
{
	
	uint32_t Count = 0;
	
	uint32_t I;
	
	ACPITable :: ACPITableHeader * SearchTable;
	
	for ( I = 0; I < TableCount; I ++ )
	{
		
		SearchTable = reinterpret_cast <HW::ACPI::ACPITable :: ACPITableHeader *> ( mm_kvmap ( reinterpret_cast <void *> ( Table -> SDTableBase [ I ] ), 0x1000, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( SearchTable == NULL )
			return Count;
		
		if ( strncmp ( SearchTable -> Signature, Name, 4 ) == 0 )
			Count ++;
		
		mm_kvunmap ( SearchTable );
		
	}
	
	return Count;
	
};
