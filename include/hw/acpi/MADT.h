#ifndef HW_ACPI_MADT_H
#define HW_ACPI_MADT_H

#include <stdint.h>

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class MADT
		{
		public:
			
			static void Init ( void * MADTAddress );
			static bool Valid ();
			
			static void Discard ();
			
		private:
			
			typedef struct
			{
				
				HW::ACPI::ACPITable :: ACPITableHeader Header;
				
				uint32_t LAPICAddress;
				uint32_t Flags;
				
			} MADTable;
			
			static const char * kSearchString;
			
			static RSDTTable * Table;
			
			static bool Validated;
			
		};
		
	};
	
};

#endif
