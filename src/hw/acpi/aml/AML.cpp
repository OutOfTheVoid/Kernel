#include <hw/acpi/aml/AML.h>
#include <hw/acpi/aml/NameSpace.h>

void HW::ACPI::AML::Init ( uint32_t * Status )
{
	
	uint32_t SubStatus = kACPIStatus_Success;
	
	NameSpace :: Init ( & SubStatus );
	
	if ( SubStatus != kACPIStatus_Success )
	{
		
		* Status = SubStatus;
		
		return;
		
	}
	
	
	
};
