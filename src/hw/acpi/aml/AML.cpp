#include <hw/acpi/aml/AML.h>
#include <hw/acpi/aml/NameSpace.h>

#include <mm/KMalloc.h>

void HW::ACPI::AML :: Init ( uint32_t * Status )
{
	
	uint32_t SubStatus = kACPIStatus_Success;
	
	NameSpace :: Init ( & SubStatus );
	
	if ( SubStatus != kACPIStatus_Success )
	{
		
		* Status = SubStatus;
		
		return;
		
	}
	
	
	
};

void * HW::ACPI::AML :: ACPIAlloc ( size_t Size )
{
	
	return mm_kmalloc ( Size );
	
};

void HW::ACPI::AML :: ACPIFree ( void * Memory )
{
	
	mm_kfree ( Memory );
	
};
