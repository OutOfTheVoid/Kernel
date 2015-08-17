#ifndef HW_ACPI_HPET_H
#define HW_ACPI_HPET_H

#include <hw/acpi/ACPI.h>
#include <hw/acpi/ACPITable.h>

#include <hw/cpu/IO.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class HPET
		{
		public:
			
			static const uint32_t kRequirement_LegacyReplacement_PIT = 0x00000001;
			static const uint32_t kRequirement_LegacyReplacement_RTC = 0x00000002;
			static const uint32_t kRequirement_FSBDelivery = 0x00000004;
			static const uint32_t kRequirement_Periodic = 0x00000008;
			
			typedef struct
			{
				
				uint32_t Address;
				uint8_t AddressSpaceID;
				
				uint8_t Index;
				uint8_t ComparratorCount;
				
				uint16_t MinimumClockTick;
				bool WideCounter;
				
				bool LegacyIRQ;
				
				uint32_t AllocationBitmap;
				
			} HPETInfo;
			
			typedef struct
			{
				
				HPETInfo * HPETInfoPointer;
				uint8_t Counter;
				
				uint32_t GlobalInterrupt;
				
				uint32_t CapabilitiesLow;
				uint32_t CapabilitiesHigh;
				
			} HPETCounterInfo;
			
			static void Init ( uint32_t * Status );
			static bool Valid ();
			
			static uint32_t GetHPETCount ( uint32_t * Status );
			static HPETInfo * GetHPET ( uint32_t Index, uint32_t * Status );
			
			static void AllocCounter ( uint32_t GlobalInterrupt, HPETCounterInfo * CounterInfo, uint32_t Requirements, uint32_t * Status );
			static void FreeCounter ( HPETCounterInfo * CounterHandle, uint32_t * Status );
			
			static void SetCounter32Bits ( HPETCounterInfo * Counter, uint32_t * Status );
			static void SetCounter64Bits ( HPETCounterInfo * Counter, uint32_t * Status );
			
			//static void 
			
		private:
			
			typedef struct
			{
				
				ACPITable :: ACPITableHeader Header;
				
				uint8_t HardwareRevision;
				
				uint8_t ComparratorCount : 5;
				uint8_t CounterSize : 1;
				uint8_t Reserved : 1;
				uint8_t LegacyReplacementCapable : 1;
				
				uint16_t PCIVendor;
				
				ACPIAddress TimerBlockAddress;
				
				uint8_t HPETNumber;
				
				uint16_t MinimumClockTick;
				
				uint8_t PageProtection;
				
			} __attribute__ (( packed )) HPETTable;
			
			static const uint32_t kRegsiter_Capabilities = 0x0000;
			static const uint32_t kRegister_Configuration = 0x0010;
			static const uint32_t kRegister_InterruptStatus = 0x0020;
			static const uint32_t kRegister_MainCounter = 0x00F0;
			
			static const uint32_t kRegister_TimerNConfigurationCapabilitiesBase = 0x0100;
			static const uint32_t kRegister_TimerNComparratorBase = 0x0108;
			static const uint32_t kRegister_TimerNFSBInterruptRouteBase = 0x0110;
			
			static const uint32_t kRegisterStride_TimerN = 0x0020;
			
			static const uint32_t kRegisterFlags_Capabilities_Low_Revision_Mask = 0x000000FF;
			static const uint32_t kRefisterFlags_Capabilities_low_Revision_Shift = 0;
			static const uint32_t kRegisterFlags_Capabilities_Low_TimerCount_Mask = 0x00001F00;
			static const uint32_t kRefisterFlags_Capabilities_Low_TimerCount_Shift = 8;
			static const uint32_t kRegisterFlags_Capabilities_Low_CounterSize = 0x00002000;
			static const uint32_t kRefisterFlags_Capabilities_Low_LegacyReplacement = 0x00008000;
			static const uint32_t kRefisterFlags_Capabilities_Low_PCIVendor_Mask = 0xFFFF0000;
			static const uint32_t kRefisterFlags_Capabilities_Low_PCIVendor_Shift = 16;
			static const uint32_t kRefisterFlags_Capabilities_High_CounterPeriod_Mask = 0xFFFFFFFF;
			static const uint32_t kRegisterFlags_Capabilities_High_CounterPeriod_Shift = 0;
			
			static const uint32_t kRegisterFlags_Configuration_Low_GlobalEnable = 0x00000001;
			static const uint32_t kRegisterFlags_Configuration_Low_LegacyReplacement = 0x00000002;
			
			static const uint32_t kRegisterFlags_InterruptStatus_Low_Timer00STSBitBase = 0x00000001;
			static const uint32_t kRegisterFlags_InterruptStatis_Low_TimerNNSTSBitShiftMultiplier = 1;
			
			static const uint32_t kRegisterFlags_MainCounter_Low_ValueLow_Mask = 0xFFFFFFFF;
			static const uint32_t kRegisterFlags_MainCounter_Low_ValueLow_Shift = 0;
			static const uint32_t kRegisterFlags_MainCounter_High_ValueHigh_Mask = 0xFFFFFFFF;
			static const uint32_t kRegisterFlags_MainCounter_High_ValueHigh_Shift = 0;
			
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_InterruptType_Edge = 0x00000000;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_InterruptType_Level = 0x00000002;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_InterruptEnable = 0x00000004;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_NonPeriodic = 0x00000000;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_Periodic = 0x00000008;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_PeriodicCapable = 0x00000010;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_WideComparrator = 0x00000020;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_EnableAccumulatorSet = 0x00000040;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_Force32Bit = 0x00000100;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_InterruptRouting_Mask = 0x0000001F;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_InterruptRouting_Shift = 9;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_EnableFSBDelivery = 0x00004000;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_Low_EnableFSBDeliveryCapable = 0x00008000;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_High_InterruptCapableBitfield_Mask = 0xFFFFFFFF;
			static const uint32_t kRegisterFlags_TimerNConfigurationCapabilities_High_InterruptCapableBitfield_Shift = 0;
			
			static const uint32_t kRegisterFlags_TimerNComparratorValue_Low_ComparratorValueLow_Mask = 0xFFFFFFFF;
			static const uint32_t kRegisterFlags_TimerNComparratorValue_Low_ComparratorValueLow_Shift = 0;
			static const uint32_t kRegisterFlags_TimerNComparratorValue_High_ComparratorValueHigh_Mask = 0xFFFFFFFF;
			static const uint32_t kRegisterFlags_TimerNComparratorValue_High_ComparratorValueHigh_Shift = 0;
			
			static const char * kTableSignature;
			
			static HPETInfo * HPETs;
			static uint32_t HPETCount;
			
			static bool Validated;
			
			inline static void ReadRegister ( volatile uint32_t Address, uint8_t AddressSpace, uint32_t Register, volatile uint32_t * Low, volatile uint32_t * High )
			{
				
				switch ( AddressSpace )
				{
					
				case kACPIAddress_AddressSpaceID_Memory:
				
					* Low = * reinterpret_cast <volatile uint32_t *> ( Address + Register );
					* High = * reinterpret_cast <volatile uint32_t *> ( Address + Register + 0x04 );
					
					break;
					
				case kACPIAddress_AddressSpaceID_SystemIO:
					
					* Low = HW::CPU::IO :: In32 ( Address + Register );
					* High = HW::CPU::IO :: In32 ( Address + Register + 0x04 );
					
					break;
					
				}
				
			};
			
			inline static void WriteRegister ( volatile uint32_t Address, uint8_t AddressSpace, uint32_t Register, volatile uint32_t Low, volatile uint32_t High )
			{
				
				switch ( AddressSpace )
				{
					
				case kACPIAddress_AddressSpaceID_Memory:
				
					* reinterpret_cast <volatile uint32_t *> ( Address + Register ) = Low;
					* reinterpret_cast <volatile uint32_t *> ( Address + Register + 0x04 ) = High;
					
					break;
					
				case kACPIAddress_AddressSpaceID_SystemIO:
					
					HW::CPU::IO :: Out32 ( Address + Register, Low );
					HW::CPU::IO :: Out32 ( Address + Register + 0x04, High );
					
					break;
					
				}
				
			};
			
		};
		
	};
	
};

#endif
