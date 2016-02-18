#include <hw/acpi/aml/GlobalLock.h>

#include <hw/acpi/FADT.h>

#include <cpputil/Linkage.h>

C_LINKAGE uint32_t * hw_acpi_aml_globallockaddr;

void hw_acpi_aml_globallockinit ( uint32_t * Status )
{
	
	hw_acpi_aml_globallockaddr = HW::ACPI::FADT :: GetGlobalLockAddress ( Status );
	
};
