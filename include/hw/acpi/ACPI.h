#ifndef HW_ACPI_ACPI_H
#define HW_ACPI_ACPI_H

#include <hw/HW.h>

/*
* ACPI.h: Contains all-encompasing definitions for the Advanced Control and Power Interface code.
*/

namespace HW
{
	
	namespace ACPI
	{
		
		/* Initializes the acpi table readers, but does not start ACPI.
		* This must be called before anything to do with ACPI, but does
		* not require multi-tasking support.
		*/
		bool StaticInit ();
		
	};
	
};

#endif