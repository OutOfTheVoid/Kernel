#include <hw/acpi/ACPITable.h>

bool HW::ACPI::ACPITable :: VerifyTable ( ACPITableHeader * Header )
{
	
	unsigned char Sum = 0;
	
	for ( uint32_t i = 0; i < Header -> Length; i ++ )
		Sum += reinterpret_cast <unsigned char *> ( Header ) [ i ];
	
	return ( ( Sum & 0xFF ) == 0 );
	
};
