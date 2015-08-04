#include <hw/acpi/ACPI.h>
#include <hw/acpi/RSDP.h>
#include <hw/acpi/RSDT.h>
#include <hw/acpi/XSDT.h>
#include <hw/acpi/MADT.h>
#include <hw/acpi/SRAT.h>
#include <hw/acpi/FADT.h>

#include <system/func/kprintf.h>

#include <KernelDef.h>

bool HW::ACPI :: StaticInit ()
{
	
	uint32_t Status;
	
	RSDP :: Locate ();
	
	if ( ! RSDP :: Found () )
		return false;
	
	if ( RSDP :: GetACPIRevision () == RSDP :: kACPI_Revision_1 )
	{
		
		RSDT :: Init ( RSDP :: GetSDTPointer () );
		
		if ( ! RSDT :: Valid () )
			return false;
		
	}
	else
	{
		
		XSDT :: Init ( RSDP :: GetSDTPointer () );
		
		if ( ! XSDT :: Valid () )
			return false;
		
	}
	
	MADT :: Init ();
	SRAT :: Init ();
	
	FADT :: Init ( & Status );
	
	return true;
	
};
