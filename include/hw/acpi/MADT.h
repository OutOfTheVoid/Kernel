#ifndef HW_ACPI_MADT_H
#define HW_ACPI_MADT_H

#include <stdint.h>

#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

#include <util/Vector.h>

#include <mt/synchronization/RWLock.h>

/* MADT.h: Contains definitions for parsing the ACPI Multiple Apic Description Table,
* required for multi-processor interrupt support, and this, multi-processor support.
*/

namespace HW
{
	
	namespace ACPI
	{
		
		class MADT
		{
		public:
			
			static const uint16_t kInterruptSourceOverrideRecord_Flags_PolarityMask = 0x03;
			static const uint16_t kInterruptSourceOverrideRecord_Flags_ConformsToBusPolarity = 0x00;
			static const uint16_t kInterruptSourceOverrideRecord_Flags_ActiveHigh = 0x01;
			static const uint16_t kInterruptSourceOverrideRecord_Flags_ActiveLow = 0x03;
			
			static const uint16_t kInterruptSourceOverrideRecord_Flags_TriggerMask = 0x0C;
			static const uint16_t kInterruptSourceOverrideRecord_Flags_ConformsToBusTrigger = 0x00;
			static const uint16_t kInterruptSourceOverrideRecord_Flags_EdgeTriggered = 0x04;
			static const uint16_t kInterruptSourceOverrideRecord_Flags_LevelTriggered = 0x0C;
			
			static void Init ();
			static bool Valid ();
			
			static void Discard ();
			
			static uint32_t GetAPICBaseAddress ();
			
			static uint32_t GetProcessorCount ();
			static uint8_t GetProcessorLAPICID ( uint32_t Index );
			static uint8_t GetProcessorID ( uint32_t Index );
			static bool GetProcessorEnabled ( uint32_t Index );
			
			static uint32_t GetIOAPICCount ();
			static uint32_t GetIOAPICBaseAddress ( uint32_t Index );
			static uint8_t GetIOAPICID ( uint32_t Index );
			static uint32_t GetIOAPICGlobalSystemInterruptBase ( uint32_t Index );
			
			static uint32_t GetInterruptSourceOverrideCount ();
			static uint8_t GetInterruptSourceOverrideBus ( uint32_t Index );
			static uint8_t GetInterruptSourceOverrideSourceIRQ ( uint32_t Index );
			static uint32_t GetInterruptSourceOverrideInterrupt ( uint32_t Index );
			static uint16_t GetInterruptSourceOverrideFlags ( uint32_t Index );
			
		private:
			
			typedef struct
			{
				
				HW::ACPI::ACPITable :: ACPITableHeader Header;
				
				uint32_t LAPICAddress;
				uint32_t Flags;
				
			} __attribute__ (( packed )) MADTable;
			
			typedef struct
			{
				
				uint8_t Type;
				uint8_t Length;
				
			} __attribute__ (( packed )) RecordHeader;
			
			typedef struct
			{
				
				RecordHeader Header;
				
				uint8_t APICProcessorID;
				uint8_t APICID;
				
				uint32_t Flags;
				
			} __attribute__ (( packed )) ProcessorLAPICRecord;
			
			typedef struct
			{
				
				RecordHeader Header;
				
				uint8_t ID;
				
				uint8_t Reserved;
				
				uint32_t Address;
				uint32_t GlobalSystemInterruptBase;
				
			} __attribute__ (( packed )) IOAPICRecord;
			
			typedef struct
			{
				
				RecordHeader Header;
				
				uint8_t Bus;
				uint8_t Source;
				
				uint32_t GlobalSystemInterrupt;
				
				uint16_t Flags;
				
			} __attribute__ (( packed )) InterruptSourceOverrideRecord;
			
			static const uint32_t kAPICFlags_ProcessorEnabled = 1;
			
			static const uint8_t kRecordType_ProcessorLAPICRecord = 0;
			static const uint8_t kRecordType_IOAPICRecord = 1;
			static const uint8_t kRecordType_InterruptSourceOverride = 2;
			
			static const char * kSearchString;
			
			static MADTable * Table;
			
			static Vector <ProcessorLAPICRecord *> * ProcessorLAPICRecords;
			static Vector <IOAPICRecord *> * IOAPICRecords;
			static Vector <InterruptSourceOverrideRecord *> * InterruptSourceOverrideRecords;
			
			static bool Validated;
			
			static MT::Synchronization::RWLock :: RWLock_t Lock;
			
		};
		
	};
	
};

#endif
