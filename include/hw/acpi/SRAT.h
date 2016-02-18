#ifndef HW_ACPI_SRAT_H
#define HW_ACPI_SRAT_H

#include <stdint.h>

#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

#include <util/Vector.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class SRAT
		{
		public:
			
			static void Init ();
			static bool Valid ();
			
			static void Discard ();
			
		private:
			
			typedef struct
			{
				
				HW::ACPI::ACPITable :: ACPITableHeader Header;
				
				uint32_t Reserved1;
				uint64_t Reserved2;
				
			} __attribute__ (( packed )) SRATable;
			
			typedef struct
			{
				
				uint8_t Type;
				
				uint8_t Length;
				
			} __attribute__ (( packed )) SRATSubTableHeader;
			
			typedef struct
			{
				
				SRATSubTableHeader Header;
				
				uint32_t ProximityDomain;
				
				uint16_t Reserved;
				
				uint8_t ProximityDomainLow;
				
				uint8_t APICID;
				
				uint32_t FLags;
				
				uint8_t LocalSAPICEID;
				
				uint8_t ProximityDomainMedium;
				uint16_t ProximityDomainHigh;
				
				uint32_t ClockDomain;
				
				uint32_t Flags;
				
			} __attribute__ (( packed )) LAPICAffinityEntry;
			
			typedef struct
			{
				
				SRATSubTableHeader Header;
				
				uint32_t ProximityDomain;
				
				uint16_t Reserved1;
				
				uint32_t BaseAddressLow;
				uint32_t BaseAddressHigh;
				
				uint32_t LengthLow;
				uint32_t LengthHigh;
				
				uint32_t Reserved2;
				
				uint32_t Flags;
				
				uint64_t Reserved3;
				
			} __attribute__ (( packed )) MemoryAffinityEntry;
			
			typedef struct
			{
				
				SRATSubTableHeader Header;
				
				uint16_t Reserved;
				
				uint32_t ProximityDomainLow;
				
				uint32_t X2APICID;
				
				uint32_t Flags;
				
				uint32_t ClockDomain;
				
				uint64_t Reserved2;
				
			} __attribute__ (( packed )) X2APICAffinityEntry;
			
			static const char * kSearchString;
			
			static SRATable * Table;
			
			static bool Validated;
			
		};
		
	};
	
};

#endif
