#ifndef HW_ACPI_AML_AML_H
#define HW_ACPI_AML_AML_H

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>

#include <stdint.h>

namespace HW
{
	
	namespace ACPI
	{
		
		namespace AML
		{
			
			void Init ( uint32_t * Status );
			
			typedef struct
			{
				
				void * Data;
				uint32_t Length;
				
			} DefinitionBlock;
			
		};
		
	};
	
};

#endif
