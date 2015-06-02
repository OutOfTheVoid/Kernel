#ifndef HW_ACPI_RSDP_H
#define HW_ACPI_RSDP_H

#include <stdint.h>

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class RSDP
		{
		public:
			
			static const uint8_t kACPI_Revision_1 = 0;
			static const uint8_t kACPI_Revision_2 = 1;
			
			static void Locate ();
			static void Discard ();
			
			static bool Found ();
			
			static uint8_t GetACPIRevision ();
			
			static void * GetSDTPointer ();
			
		private:
			
			typedef struct
			{
				
				char Signature [ 8 ];
				uint8_t Checksum;
				
				char OEMID [ 6 ];
				
				uint8_t Revision;
				
				uint32_t RSDTAddress;
				
			} __attribute__ (( packed )) RSDPTable;
			
			typedef struct
			{
				
				RSDPTable HeadTable;
				
				uint32_t Length;
				uint64_t XSDTAddress;
				
				uint8_t ExtendedChecksum;
				
				uint8_t Reserved [ 3 ];
				
			} __attribute__ (( packed )) RSDPTable2;
			
			static bool Check1 ( RSDPTable * Table );
			static bool Check2 ( RSDPTable2 * Table );
			
			static const uint32_t kEBDA_PotentialStart = 0x00080000;
			static const uint32_t kEBDA_End = 0x000A0000;
			
			static const uint32_t kHighBIOS_Start = 0x000E0000;
			static const uint32_t kHighBIOS_End = 0x00100000;
			
			static const char * kTestString;
			
			static bool TableFound;
			
			static uint8_t Revision;
			
			static uint32_t TablePhysical;
			
			static RSDPTable * Table;
			static RSDPTable2 * Table2;
			
		};
		
	};
	
};

#endif
