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
			
			const uint32_t kDefinitionBlockType_Static = 0;
			const uint32_t kDefinitionBlockType_Dynamic = 1;
			const uint32_t kDefinitionBlockType_Root = 2;
			
			void Init ( uint32_t * Status );
			
			typedef struct
			{
				
				const char * Name;
				uint32_t Type;
				
				void * Data;
				uint32_t Length;
				
			} DefinitionBlock;
			
			void * ACPIAlloc ( size_t Size );
			void ACPIFree ( void * Memory );
			
		};
		
	};
	
};

#endif
