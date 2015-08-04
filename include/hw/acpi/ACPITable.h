#ifndef HW_ACPI_ACPITABLE_H
#define HW_ACPI_ACPITABLE_H

#include <stdint.h>

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>

/* ACPITable.h: Overarching support for ACPI Tables. This includes the standard ACPI table header,
* as well as a way to verify a table for it's checksum.
*/

namespace HW
{
	
	namespace ACPI
	{
		
		class ACPITable
		{
		public:
			
			typedef struct
			{
				
				char Signature [ 4 ];
				
				uint32_t Length;
				
				uint8_t Revision;
				
				uint8_t Checksum;
				
				char OEMID [ 6 ];
				char OEMTableID [ 8 ];
				
				uint32_t OEMRevision;
				uint32_t CreatorID;
				uint32_t CreatorRevision;
				
			} __attribute__ (( packed )) ACPITableHeader;
			
			static bool VerifyTable ( ACPITableHeader * Header );
			
		};
		
	};
	
};

#endif