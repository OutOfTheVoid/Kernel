#ifndef HW_ACPI_HPET_H
#define HW_ACPI_HPET_H

#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class HPET
		{
		public:
			
			static void Init ();
			static bool Valid ();
			
			static void Discard ();
			
		private:
			
			typedef struct
			{
				
				ACPITable :: ACPITableHeader Header;
				
				uint8_t HardwareRevision;
				
				uint8_t ComparatorCount : 5;
				uint8_t CounterSize : 1;
				uint8_t Reserved : 1;
				uint8_t LegacyReplacement : 1;
				
				uint16_t PCIVendor;
				
				ACPITable :: ACPIAddress Address;
				
				uint8_t HPETNumber;
				
				uint16_t MinimumTick;
				
				uint8_t PageProtection;
				
			} __attribute__ (( packed )) HPETTable;
			
		};
		
	};
	
};

#endif
