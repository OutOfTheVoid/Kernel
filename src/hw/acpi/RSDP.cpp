#include <hw/acpi/RSDP.h>

#include <util/string/String.h>

#include <mm/paging/AddressSpace.h>
#include <mm/paging/PageTable.h>

#include <system/func/KPrintf.h>

bool HW::ACPI::RSDP :: TableFound = false;

uint8_t HW::ACPI::RSDP :: Revision = 0;

uint32_t HW::ACPI::RSDP :: TablePhysical = 0;

HW::ACPI::RSDP :: RSDPTable * HW::ACPI::RSDP :: Table = NULL;
HW::ACPI::RSDP :: RSDPTable2 * HW::ACPI::RSDP :: Table2 = NULL;

const char * HW::ACPI::RSDP :: kTestString = "RSD PTR ";

void HW::ACPI::RSDP :: Locate ()
{
	
	uint32_t Error;
	
	if ( TableFound )
		return;
	
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Alloc ( 0x1000, reinterpret_cast <void **> ( & Table ), & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kAlloc_Error_None )
	{
		
		return;
		
	}
	
	TablePhysical = kEBDA_PotentialStart;
	
	while ( TablePhysical < kEBDA_End )
	{
		
		Table = reinterpret_cast <RSDPTable *> ( ( reinterpret_cast <uint32_t> ( Table ) & 0xFFFFF000 ) + ( TablePhysical & 0x00000FFF ) );
		Table2 = reinterpret_cast <RSDPTable2 *> ( Table );
		
		MM::Paging::PageTable :: SetKernelMapping ( reinterpret_cast <uint32_t> ( Table ), TablePhysical, MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
		
		if ( Check1 ( Table ) )
		{
			
			if ( Table -> Revision != kACPI_Revision_1 )
			{
				
				if ( Check2 ( Table2 ) )
				{
					
					HW::ACPI::RSDP :: Revision = kACPI_Revision_2;
					
					TableFound = true;
					
					return;
					
				}
				
			}
			else
			{
				
				HW::ACPI::RSDP :: Revision = kACPI_Revision_1;
				
				TableFound = true;
				
				return;
				
			}
			
		}
		
		TablePhysical += 0x10;
		
	}
	
	TablePhysical = kHighBIOS_Start;
	
	while ( TablePhysical < kHighBIOS_End )
	{
		
		Table = reinterpret_cast <RSDPTable *> ( ( reinterpret_cast <uint32_t> ( Table ) & 0xFFFFF000 ) + ( TablePhysical & 0x00000FFF ) );
		Table2 = reinterpret_cast <RSDPTable2 *> ( Table );
		
		MM::Paging::PageTable :: SetKernelMapping ( reinterpret_cast <uint32_t> ( Table ), TablePhysical, MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
		
		if ( Check1 ( Table ) )
		{
			
			if ( Table -> Revision != kACPI_Revision_1 )
			{
				
				if ( Check2 ( Table2 ) )
				{
					
					HW::ACPI::RSDP :: Revision = kACPI_Revision_2;
					
					TableFound = true;
					
					return;
					
				}
				
			}
			else
			{
				
				HW::ACPI::RSDP :: Revision = kACPI_Revision_1;
				
				TableFound = true;
				
				return;
				
			}
			
		}
		
		TablePhysical += 0x10;
		
	}
	
	MM::Paging::PageTable :: ClearKernelMapping (  reinterpret_cast <uint32_t> ( Table ) );
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Free ( reinterpret_cast <void *> ( Table ), & Error );
	
	Revision = 0;
	
	Table = NULL;
	Table2 = NULL;
	
};

void HW::ACPI::RSDP :: Discard ()
{
	
	uint32_t Error;
	
	if ( ! TableFound )
		return;
	
	TableFound = false;
	
	Revision = 0;
	
	MM::Paging::PageTable :: ClearKernelMapping ( reinterpret_cast <uint32_t> ( Table ) );
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Free ( reinterpret_cast <void *> ( Table ), & Error );
	
	Table = NULL;
	Table2 = NULL;
	
	TablePhysical = 0;
	
};

bool HW::ACPI::RSDP :: Found ()
{
	
	return TableFound;
	
}

bool HW::ACPI::RSDP :: Check1 ( RSDPTable * Table )
{
	
	if ( strncmp ( Table -> Signature, kTestString, 8 ) != 0 )
		return false;
	
	uint8_t Sum = 0;
	
	for ( uint32_t i = 0; i < sizeof ( RSDPTable ); i ++ )
		Sum += reinterpret_cast <char *> ( Table ) [ i ];
	
	return Sum == 0;
	
};

bool HW::ACPI::RSDP :: Check2 ( RSDPTable2 * Table )
{
	
	uint8_t Sum = 0;
	
	for ( uint32_t i = 0; i < Table -> Length; i ++ )
		Sum += reinterpret_cast <char *> ( Table ) [ i ];
	
	return Sum == 0;
	
};

void * HW::ACPI::RSDP :: GetSDTPointer ()
{
	
	if ( ! TableFound )
		return NULL;
	
	if ( Revision == kACPI_Revision_1 )
		return reinterpret_cast <void *> ( Table -> RSDTAddress );
	
	return reinterpret_cast <void *> ( Table2 -> XSDTAddress );
	
};

uint8_t HW::ACPI::RSDP :: GetACPIRevision ()
{
	
	return Revision;
	
};
