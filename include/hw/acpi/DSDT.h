#ifndef HW_ACPI_DSDT_H
#define HW_ACPI_DSDT_H

#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

#include <stdint.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class DSDT
		{
		public:
			
			static void Init ( uint32_t * Status );
			static bool Valid ();
			
			static void * GetDefinitionBlockAddress ( uint32_t * Status );
			static uint32_t GetDefinitionBlockLength ( uint32_t * Status );
			
		private:
			
			typedef struct
			{
				
				ACPITable :: ACPITableHeader Header;
				
				uint8_t DefinitionBlockStart [ 0 ];
				
			} DSDTable;
			
			static bool Validated;
			
			static DSDTable * Table;
			static uint32_t TableLength;
			
		};
		
	};
	
};

#endif
