#include <hw/acpi/MADT.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/RSDP.h>

#include <mm/paging/AddressSpace.h>
#include <mm/paging/PageTable.h>

bool HW::ACPI::MADT :: Validated = false;

const char * HW::ACPI::MADT :: kSearchString = "MADT";

void HW::ACPI::MADT :: Init ( void * MADTAddress )
{
	
	uint32_t Error;
	
	if ( Validated )
		return;
	
	void * PhysAddr;
	
	if ( RSDP :: GetRevision () == RSDP :: kACPI_Revision_1 )
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
	
	MM::Paging::AddressSpace :: RetrieveKernelAddressSpace () -> Alloc ( 0x1000, reinterpret_cast <void **> ( & Table ), & Error );
	
	
	
};

bool HW::ACPI::MADT :: Valid ()
{
	
	return Validated;
	
};

void HW::ACPI::MADT :: Discard ()
{
	
	
	
};
