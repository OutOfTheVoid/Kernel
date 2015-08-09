#ifndef HW_ACPI_FADT_H
#define HW_ACPI_FADT_H

#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

#include <mt/synchronization/Spinlock.h>

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
			
			static const uint16_t kIAPCFlag_LegacyDevices = 0x0001;
			static const uint16_t kIAPCFlag_8042 = 0x0002;
			static const uint16_t kIAPCFlag_NoVGA = 0x0004;
			static const uint16_t kIAPCFlag_NoMSI = 0x0008;
			static const uint16_t kIAPCFlag_NoASPM = 0x0010;
			static const uint16_t kIAPCFlag_RTCCMOS = 0x0020;
			
			static const uint32_t kFixedFeatureFlag_WBINVD = 0x00000001;
			static const uint32_t kFixedFeatureFlag_WBINVDFlush = 0x00000002;
			static const uint32_t kFixedFeatureFlag_CPUC1 = 0x00000004;
			static const uint32_t kFixedFeatureFlag_CPUC2MP = 0x00000008;
			static const uint32_t kFixedFeatureFlag_PowerButtonControlMethod = 0x00000010;
			static const uint32_t kFixedFeatureFlag_SleepButtonControlMethod = 0x00000020;
			static const uint32_t kFixedFeatureFlag_RTCWakeFixedRegister = 0x00000040;
			static const uint32_t kFixedFeatureFlag_RTCWakeS4 = 0x00000080;
			static const uint32_t kFixedFeatureFlag_PMTimerExtended = 0x00000100;
			
			static void Init ( uint32_t * Status );
			static bool Valid ();
			
			static void Discard ();
			
			static uint8_t GetPreferredPMProfile ( uint32_t * Status );
			
			static uint16_t GetACPISystemControlInterrupt ( uint32_t * Status );
			
			static uint32_t GetSMICommandPort ( uint32_t * Status );
			
			static void WriteACPIEnable ( bool Enable, uint32_t * Status );
			
			static ACPIAddress GetPM1aEventBlockAddress ( uint32_t * Status );
			static ACPIAddress GetPM1bEventBlockAddress ( uint32_t * Status );
			static uint8_t GetPM1EventBlockByteCount ( uint32_t * Status );
			
			static ACPIAddress GetPM1aControlBlockAddress ( uint32_t * Status );
			static ACPIAddress GetPM1bControlBlockAddress ( uint32_t * Status );
			static uint8_t GetPM1ControlBlockByteCount ( uint32_t * Status );
			
			static ACPIAddress GetPM2ControlBlockAddress ( uint32_t * Status );
			static uint8_t GetPM2ControlBlockByteCount ( uint32_t * Status );
			
			static ACPIAddress GetPMTimerBlockAddress ( uint32_t * Status );
			static uint8_t GetPMTimerBlockByteCount ( uint32_t * Status );
			
			static ACPIAddress GetGeneralPurposeEvent0BlockAddress ( uint32_t * Status );
			static uint8_t GetGeneralPurposeEvent0BlockByteCount ( uint32_t * Status );
			
			static ACPIAddress GetGeneralPurposeEvent1BlockAddress ( uint32_t * Status );
			static uint8_t GetGeneralPurposeEvent1BlockByteCount ( uint32_t * Status );
			static uint8_t GetGeneralPurposeEvent1Base ( uint32_t * Status );
			
			static ACPIAddress GetResetRegisterAddress ( uint32_t * Status );
			static uint8_t GetResetValue ( uint32_t * Status );
			
			static uint16_t GetIAPCFlags ( uint32_t * Status );
			
			static uint32_t GetFixedFeatureFlags ( uint32_t * Status );
			
		private:
			
			static const uint32_t kFADTRevision_2 = 3;
			
			typedef struct
			{
				
				ACPITable :: ACPITableHeader Header;
				
				uint32_t FACSAddress;
				uint32_t DSDTAddress;
				
				uint8_t Reserved;
				
				uint8_t PreferredPMProfile;
				
				uint16_t SystemControlInterrupt;
				
				uint32_t SMICommandPort;
				
				uint8_t ACPIEnableCommand;
				uint8_t ACPIDisableCommand;
				
				uint8_t S4BIOSRequest;
				
				uint8_t ProcessorControlCommand;
				
				uint32_t PM1aEventBlockAddress;
				uint32_t PM1bEventBlockAddress;
				uint32_t PM1aControlBlockAddress;
				uint32_t PM1bControlBlockAddress;
				uint32_t PM2ControlBlockAddress;
				
				uint32_t PMTimerBlockAddress;
				
				uint32_t GPE0BlockAddress;
				uint32_t GPE1BlockAddress;
				
				uint8_t PM1EventBlockLength;
				uint8_t PM1ControlBlockLength;
				uint8_t PM2ControlBlockLength;
				
				uint8_t PMTimerBlockLength;
				
				uint8_t GPE0BlockLength;
				uint8_t GPE1BlockLength;
				uint8_t GPE1Base;
				
				uint8_t CSTCommand;
				
				uint16_t C2LatencyMS;
				uint16_t C3LatencyMS;
				
				uint16_t WBInvalFlushSize;
				uint16_t WBInvalFlushStride;
				
				uint8_t DutyOffset;
				uint8_t DutyWidth;
				
				uint8_t RTCCMOSDayAlarm;
				uint8_t RTCCMOSMonthAlarm;
				uint8_t RTCCMOSCentury;
				
				uint16_t IAPCFlags;
				
				uint8_t Reserved2;
				
				uint32_t FixedFlags;
				
				ACPIAddress ResetRegister;
				
				uint8_t ResetValue;
				
				uint8_t Reserved3 [ 3 ];
				
				uint64_t ExtendedFACSAddress;
				uint64_t ExtendedDSDTAddress;
				
				ACPIAddress ExtendedPM1aEventBlockAddress;
				ACPIAddress ExtendedPM1bEventBlockAddress;
				ACPIAddress ExtendedPM1aControlBlockAddress;
				ACPIAddress ExtendedPM1bControlBlockAddress;
				ACPIAddress ExtendedPM2ControlBlockAddress;
				
				ACPIAddress ExtendedPMTimerBlockAddress;
				
				ACPIAddress ExtendedGPE0BlockAddress;
				ACPIAddress ExtendedGPE1BlockAddress;
				
				ACPIAddress SleepControlRegister;
				ACPIAddress SleepStatusRegister;
				
			} __attribute__ (( packed )) FADTable;
			
			static bool Validated;
			
			static const char * kSearchString;
			
			static FADTable * Table;
			
		};
		
	};
	
}

#endif
