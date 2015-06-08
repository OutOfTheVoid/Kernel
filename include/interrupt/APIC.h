#ifndef INTERRUPT_APIC_H
#define INTERRUPT_APIC_H

#include <stdint.h>

namespace Interrupt
{
	
	class APIC
	{
	public:
		
		static void Init ();
		static bool HasAPIC ();
		
	private:
		
		static void SetAPICBaseAddress ( uint32_t Base );
		static uint32_t GetAPICBaseAddress ();
		
		static void EnableAPIC ();
		
		static void ReadRegister ( uint32_t Offset, uint32_t * DataOut, uint32_t DWordLength );
		static void WriteRegister ( uint32_t Offset, uint32_t * DataIn, uint8_t DwordLength );
		
		static bool Availible;
		
		static uint32_t Base;
		static uint32_t * BaseVirtual;
		
		static const uint32_t kRegisterOffset_ID = 0x020;
		static const uint32_t kRegisterOffset_Version = 0x030;
		
		static const uint32_t kRegisterOffset_TaskPriority = 0x080;
		static const uint32_t kRegisterOffset_ArbitrationPriority = 0x090;
		static const uint32_t kRegisterOffset_ProcessorPriority = 0x090;
		
		static const uint32_t kRegisterOffset_EOI = 0x0B0;
		
		static const uint32_t kRegisterOffset_RemoteRead = 0x0C0;
		
		static const uint32_t kRegisterOffset_LogicalDestination = 0x0D0;
		static const uint32_t kRegisterOffset_DestinationFormat = 0x0E0;
		
		static const uint32_t kRegisterOffset_SpuriousInterruptVector = 0x0F0;
		
		static const uint32_t kRegisterOffset_InterruptStatus = 0x100;
		static const uint32_t kRegisterOffset_TriggerMode = 0x180;
		static const uint32_t kRegisterOffset_InterruptRequest = 0x200;
		static const uint32_t kRegisterOffset_ErrorStatus = 0x280;
		
		static const uint32_t kRegisterOffset_LVTCMCI = 0x2F0;
		
		static const uint32_t kRegisterOffset_InterruptCommand_Lower = 0x300;
		static const uint32_t kRegisterOffset_InterruptCommand_Upper = 0x310;
		
		static const uint32_t kRegisterOffset_LVTTimer = 0x320;
		static const uint32_t kRegisterOffset_LVTThermal = 0x330;
		static const uint32_t kRegisterOffset_LVTPerformance = 0x340;
		static const uint32_t kRegisterOffset_LVTLINT0 = 0x350;
		static const uint32_t kRegisterOffset_LVTLINT1 = 0x360;
		static const uint32_t kRegisterOffset_LVTError = 0x370;
		
		static const uint32_t kRegisterOffset_TimerInitialCount = 0x380;
		static const uint32_t kRegisterOffset_TimerCurrentCount = 0x390;
		static const uint32_t kRegisterOffset_TimerDivideConfig = 0x3E0;
		
	};
	
};

#endif
