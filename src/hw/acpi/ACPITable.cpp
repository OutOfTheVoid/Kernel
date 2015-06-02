#include <hw/acpi/ACPITable.h>

bool HW::ACPI::ACPITable :: VerifyTable ( ACPITableHeader * Header )
{
	
	uint8_t Sum = 0;
	
	for ( uint32_t i = 0; i < Header -> Length; i ++ )
		Sum += reinterpret_cast <uint8_t *> ( Header ) [ i ];
	
	return ( Sum == 0 );
	
};