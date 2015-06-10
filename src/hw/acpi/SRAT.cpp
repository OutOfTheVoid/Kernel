#include <hw/acpi/SRAT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

#include <mm/KVMap.h>
#include <mm/KMalloc.h>
#include <mm/paging/PageTable.h>

#include <system/func/kprintf.h>

#include <KernelDef.h>

const char * HW::ACPI::SRAT :: kSearchString = "SRAT";

HW::ACPI::SRAT :: SRATable * HW::ACPI::SRAT :: Table = NULL;

bool HW::ACPI::SRAT :: Validated = false;

MT::Synchronization::Spinlock :: Spinlock_t HW::ACPI::SRAT :: Lock = MT::Synchronization::Spinlock :: Initializer ();

void HW::ACPI::SRAT :: Init ()
{
	
	uint32_t TableLength;
	void * PhysAddr;
	
	if ( Validated || ( ! RSDP :: Found () ) )
		return;
	
	if ( RSDP :: GetACPIRevision () == RSDP :: kACPI_Revision_1 )
	{
		
		if ( ! RSDT :: Valid () )
			return;
		
		PhysAddr = RSDT :: FindTable ( kSearchString );
		
	}
	else
	{
		
		if ( ! XSDT :: Valid () )
			return;
		
		PhysAddr = XSDT :: FindTable ( kSearchString );
		
	}
	
	if ( PhysAddr == NULL )
		return;
	
	Table = reinterpret_cast <SRATable *> ( mm_kvmap ( PhysAddr, 0x2000, MM::Paging::PageTable :: Flags_Writeable ) );
	
	if ( Table == NULL )
		return;
	
	TableLength = ( reinterpret_cast <uint32_t> ( PhysAddr ) + Table -> Header.Length ) - ( reinterpret_cast <uint32_t> ( PhysAddr ) & 0xFFFFF000 );
	
	if ( TableLength > 0x2000 )
	{
		
		mm_kvunmap ( Table );
		Table = reinterpret_cast <SRATable *> ( mm_kvmap ( PhysAddr, TableLength, MM::Paging::PageTable :: Flags_Writeable ) );
		
		if ( Table == NULL )
			return;
		
	}
	
	if ( ! ACPITable :: VerifyTable ( & Table -> Header ) )
	{
		
		mm_kvunmap ( Table );
		
		return;
		
	}
	
	SRATSubTableHeader * EntryHeader = reinterpret_cast <SRATSubTableHeader *> ( reinterpret_cast <uint32_t> ( Table ) + sizeof ( SRATable ) );
	
	while ( reinterpret_cast <uint32_t> ( EntryHeader ) < reinterpret_cast <uint32_t> ( Table ) + Table -> Header.Length )
	{
		
		switch ( EntryHeader -> Type )
		{
		default:
		
			break;
		}
		
		EntryHeader = reinterpret_cast <SRATSubTableHeader *> ( reinterpret_cast <uint32_t> ( EntryHeader ) + EntryHeader -> Length );
		
	}
	
	Validated = true;
	
};

bool HW::ACPI::SRAT :: Valid ()
{
	
	return Validated;
	
};

void HW::ACPI::SRAT :: Discard ()
{
	
	if ( ! Validated )
		return;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	mm_kvunmap ( Table );
	
	Table = NULL;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};
