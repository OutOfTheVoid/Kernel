#ifndef HW_ACPI_FADT_H
#define HW_ACPI_FADT_H

#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class FADT
		{
		public:
			
			static const uint8_t kPMProfile_Unspecified = 0;
			static const uint8_t kPMProfile_Desktop = 1;
			static const uint8_t kPMProfile_Mobile = 2;
			static const uint8_t kPMProfile_Workstation = 3;
			static const uint8_t kPMProfile_EnterpriseServer = 4;
			static const uint8_t kPMProfile_SOHOServer = 5;
			static const uint8_t kPMProfile_AppliancePC = 6;
			static const uint8_t kPMProfile_PerformanceServer = 7;
			static const uint8_t kPMProfile_Tablet = 8;
			
			static void Init ();
			static bool Valid ();
			
			static void Discard ();
			
			static uint8_t GetPreferredPMProfile ();
			
			static uint16_t GetACPISystemControlInterrupt ();
			
			static uint32_t GetSMICommandPort ();
			
			static void WriteACPIEnable ( bool Enable );
			
			static uint64_t GetPM1aEventBlockAddress ();
			static uint64_t GetPM1bEventBlockAddress ();
			static uint64_t GetPM1EventBlockByteCount ();
			
			static uint64_t GetPM1aControlBlockAddress ();
			static uint64_t GetPM1bControlBlockAddress ();
			static uint64_t GetPM1ControlBlockByteCount ();
			
			static uint64_t GetPM2ControlBlockAddress ();
			static uint64_t GetPM2ControlBlockByteCount ();
			
			static uint64_t GetPMTimerBlockAddress ();
			static uint64_t GetPMTimerBlockByteCount ();
			
			static uint64_t GetGeneralPurposeEvent0BlockAddress ();
			static uint64_t GetGeneralPurposeEvent1BlockAddress ();
			
			static uint64_t GetGeneralPurposeEvent1Base ();
			
			static ACPITable :: ACPIAddress GetResetRegisterAddress ();
			static uint8_t GetResetValue ();
			
		private:
			
			typedef struct
			{
				
				ACPITable :: ACPITableHeader Header;
				
				uint32_t FACSAddress;
				uint32_t DSDTAddress;
				
				uint8_t Reserved;
				
				uint8_t PreferredPMProfile;
				
				uint16_t SystemControlInterrupt;
				
			} __attribute__ (( packed )) FADTable;
			
			static bool Validated;
			
			static const char * kSearchString;
			
			static FADTable * Table;
			
			static MT::Synchronization::Spinlock :: Spinlock_t Lock;
			
		};
		
	};
	
}

#endif
