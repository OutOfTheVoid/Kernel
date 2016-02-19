#ifndef HW_ACPI_DSDT_H
#define HW_ACPI_DSDT_H

#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>
#include <hw/acpi/aml/DefinitionBlock.h>

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
			
			static void Discard ();
			
		private:
			
			typedef struct
			{
				
				ACPITable :: ACPITableHeader Header;
				AML::DefinitionBlock :: DefinitionBlockHeader;
				
			} __attribute__ (( packed )) DSDTable;
			
		};
		
	};
	
};