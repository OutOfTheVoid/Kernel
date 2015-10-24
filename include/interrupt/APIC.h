#ifndef INTERRUPT_APIC_H
#define INTERRUPT_APIC_H

#include <stdint.h>

namespace Interrupt
{
	
	class APIC
	{
	public:
		
		static const bool kTimerMode_OneShot = 0;
		static const bool kTimerMode_Periodic = 1;
		
		static const uint32_t kTimerDivision_1 = 0;
		static const uint32_t kTimerDivision_2 = 1;
		static const uint32_t kTimerDivision_4 = 2;
		static const uint32_t kTimerDivision_8 = 3;
		static const uint32_t kTimerDivision_16 = 4;
		static const uint32_t kTimerDivision_32 = 5;
		static const uint32_t kTimerDivision_64 = 6;
		static const uint32_t kTimerDivision_128 = 7;
		
		static bool HasAPIC ();
		
		static void Init ();
		static void APInit ();
		
		static void Enable ();
		static void Disable ();
		
		static bool IsLocalBSP ();
		
		static uint8_t GetLocalID ();
		static void SetLocalID ( uint8_t ID );
		
		static uint8_t GetLocalVersion ();
		
		static void SetLocalTimerVector ( bool InitiallyMasked, uint8_t Vector );
		static void SetLocalTimerDivide ( uint32_t Divide );
		
		static void StartTimerOneShot ( uint32_t InitialValue );
		static bool PollTimerOneShot ();
		static uint32_t ReadTimer ();
		
		static void StartTimerPeriodic ( uint32_t SystemClockPeriod );
		
		static void SendPhysicalInitIPI ( uint8_t TargetID, bool Assert );
		static void SendPhysicalStartupIPI ( uint8_t TargetID, uint32_t EntryPageNum );
		static void SendBroadNMI ();
		
		static void ClearErrorStatus ();
		
		static void EndOfInterrupt ();
		
		static void SetTaskPriority ( uint32_t Priority );
		
		static bool IPIAccepted ();
		
		static double GetBusFrequencey ();
		
		static void ClearLatchedInterrupts ();
		
	private:
		
		static uint8_t GetLocalLVTCount ();
		
		static uint32_t GetAPICBaseAddress ();
		
		static void ReadRegister ( uint32_t Offset, volatile uint32_t * DataOut, uint32_t DWordLength );
		static void WriteRegister ( uint32_t Offset, volatile uint32_t * DataIn, uint8_t DwordLength );
		
		static bool Availible;
		
		static uint32_t Base;
		static volatile uint32_t * BaseVirtual;
		
		static double BusFrequencey;
		
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
		
		static const uint32_t kRegisterOffset_InterruptCommand_Lower = 0x300;
		static const uint32_t kRegisterOffset_InterruptCommand_Upper = 0x310;
		
		static const uint32_t kRegisterOffset_LVTTimer = 0x320;
		static const uint32_t kRegisterOffset_LVTLINT0 = 0x350;
		static const uint32_t kRegisterOffset_LVTLINT1 = 0x360;
		static const uint32_t kRegisterOffset_LVTError = 0x370;
		
		static const uint32_t kRegisterOffset_TimerInitialCount = 0x380;
		static const uint32_t kRegisterOffset_TimerCurrentCount = 0x390;
		static const uint32_t kRegisterOffset_TimerDivideConfig = 0x3E0;
		
		static const uint32_t kSpuriousInterruptVectorFlag_APICEnable = 0x00000100;
		
		static const uint32_t kLVTFlag_Mask = 0x10000;
		static const uint32_t kLVTFlag_TimerModePeriodic = 0x20000;
		
		static const uint32_t kIPIDeliveryMode_Fixed = 0x000;
		static const uint32_t kIPIDeliveryMode_LowestPriority = 0x100;
		static const uint32_t kIPIDeliveryMode_SMI = 0x200;
		static const uint32_t kIPIDeliveryMode_NMI = 0x400;
		static const uint32_t kIPIDeliveryMode_INIT = 0x500;
		static const uint32_t kIPIDeliveryMode_StartUp = 0x600;
		
		static const uint32_t kIPIDestinationMode_Logical = 0x800;
		static const uint32_t kIPIDestinationMode_Physical = 0x000;
		
		static const uint32_t kIPIDeliverStatus_Pending = 0x1000;
		
		static const uint32_t kIPILevel_LTrig = 0x8000;
		static const uint32_t kIPILevel_Assert = 0x4000;
		
		static const uint32_t kIPITriggerMode_Edge = 0x0000;
		static const uint32_t kIPITriggerMode_Level = 0x8000;
		
		static const uint32_t kIPIDestinationShorthand_None = 0x00000;
		static const uint32_t kIPIDestinationShorthand_Self = 0x40000;
		static const uint32_t kIPIDestinationShorthand_All = 0x80000;
		static const uint32_t kIPIDestinationShorthand_AllExcludingSelf = 0xC0000;
		
		static const uint16_t kIMCR_SelectPort = 0x70;
		static const uint16_t kIMCR_DataPort = 0x01;
		
		static const uint8_t kIMCR_SelectData = 0x22;
		static const uint8_t kIMCR_DataData = 0x23;
		
	};
	
};

#endif
