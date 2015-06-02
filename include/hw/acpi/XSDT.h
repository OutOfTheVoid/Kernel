#include <stdint.h>

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class XSDT
		{
		public:
			
			static void Init ( void * RSDTAddress );
			static bool Valid ();
			
			static void Discard ();
			
			static void * FindTable ( const char * Name );
			
		private:
			
			typedef struct
			{
				
				HW::ACPI::ACPITable :: ACPITableHeader Header;
				
				uint64_t SDTableBase [];
				
			} __attribute__ (( packed )) XSDTable;
			
			static XSDTable * Table;
			static uint32_t TableLength;
			
			static bool Validated;
			
			static uint32_t TableCount;
			
		};
		
	};
	
};
