#ifndef HW_ACPI_RSDT_H
#define HW_ACPI_RSDT_H

#include <stdint.h>

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class RSDT
		{
		public:
			
			static void Init ( void * RSDTAddress );
			static bool Valid ();
			
			static void * FindTable ( const char * Name );
			static void FindTables ( const char * Name, void * Addresses [], uint32_t Max );
			
			static uint32_t CountTables ( const char * Name );
			
		private:
			
			typedef struct
			{
				
				HW::ACPI::ACPITable :: ACPITableHeader Header;
				
				uint32_t SDTableBase [];
				
			} __attribute__ (( packed )) RSDTable;
			
			static RSDTable * Table;
			static uint32_t TableLength;
			
			static bool Validated;
			
			static uint32_t TableCount;
			
		};
		
	};
	
};

#endif
