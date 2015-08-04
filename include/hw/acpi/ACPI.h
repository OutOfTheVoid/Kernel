#ifndef HW_ACPI_ACPI_H
#define HW_ACPI_ACPI_H

#include <hw/HW.h>

#include <stdint.h>

/*
* ACPI.h: Contains all-encompasing definitions for the Advanced Control and Power Interface code.
*/

namespace HW
{
	
	namespace ACPI
	{
		
		typedef struct
		{
			
			uint8_t AddressSpaceID;
			
			uint8_t RegisterBitWidth;
			uint8_t RegisterBitOffset;
			
			uint8_t Reserved;
			
			uint64_t Address;
			
		} __attribute__ (( packed )) ACPIAddress;
		
		const uint8_t kACPIAddress_AddressSpaceID_Memory = 0x00;
		const uint8_t kACPIAddress_AddressSpaceID_SystemIO = 0x01;
		const uint8_t kACPIAddress_AddressSpaceID_PCIConfiguration = 0x02;
		const uint8_t kACPIAddress_AddressSpaceID_EmbeddedController = 0x03;
		const uint8_t kACPIAddress_AddressSpaceID_SMBus = 0x04;
		const uint8_t kACPIAddress_AddressSpaceID_FFHW = 0x7F;
		
		const uint8_t kACPIAddress_AccessSize_Undefined = 0x00;
		const uint8_t kACPIAddress_AccessSize_8 = 0x01;
		const uint8_t kACPIAddress_AccessSize_16 = 0x02;
		const uint8_t kACPIAddress_AccessSize_32 = 0x03;
		const uint8_t kACPIAddress_AccessSize_64 = 0x04;
		
		const uint32_t kACPIStatus_Success = 0x00000000;
		const uint32_t kACPIStatus_Failure_Unknown = 0x00000001;
		const uint32_t kACPIStatus_Failure_InvalidTable = 0x00000002;
		
		/* Initializes the acpi table readers, but does not start ACPI.
		* This must be called before anything to do with ACPI, but does
		* not require multi-tasking support.
		*/
		bool StaticInit ();
		
	};
	
};

#endif