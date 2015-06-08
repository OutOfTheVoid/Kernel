#ifndef HW_ACPI_MADT_H
#define HW_ACPI_MADT_H

#include <stdint.h>

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

#include <util/Vector.h>

#include <mt/synchronization/Spinlock.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class MADT
		{
		public:
			
			static void Init ();
			static bool Valid ();
			
			static void Discard ();
			
			static uint32_t GetAPICBaseAddress ();
			
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
				
				uint8_t ID;
				
				uint8_t Reserved;
				
				uint32_t Address;
				uint32_t GlobalSystemInterruptBase;
				
			} __attribute__ (( packed )) IOAPICRecord;
			
			typedef struct
			{
				
				uint8_t BusSource;
				uint8_t IRQSource;
				
				uint32_t GlobalSystemInterrupt;
				
				uint16_t Flags;
				
			} __attribute__ (( packed )) InterruptSourceOverrideRecord;
			
			static const uint32_t kAPICFlags_ProcessorEnabled;
			
			static const uint8_t kRecordType_ProcessorLAPICRecord = 0;
			static const uint8_t kRecordType_IOAPICRecord = 1;
			static const uint8_t kRecordType_InterruptSourceOverride = 2;
			
			static const char * kSearchString;
			
			static MADTable * Table;
			
			static Vector <ProcessorLAPICRecord *> * ProcessorLAPICRecords;
			static Vector <IOAPICRecord *> * IOAPICRecords;
			static Vector <InterruptSourceOverrideRecord *> * InterruptSourceOverrideRecords;
			
			static bool Validated;
			
			static MT::Synchronization::Spinlock :: Spinlock_t Lock;
			
		};
		
	};
	
};

#endif
