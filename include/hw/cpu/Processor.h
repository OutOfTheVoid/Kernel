#ifndef HW_CPU_PROCESSOR_H
#define HW_CPU_PROCESSOR_H

#include <hw/cpu/CPU.h>

#include <mt/MT.h>
#include <mt/tasking/Task.h>
#include <mt/synchronization/Spinlock.h>
#include <mt/hw/TSS.h>

#include <stdint.h>

namespace HW
{
	
	namespace CPU
	{
		
		class Processor
		{
		public:
			
			static const uint32_t kCPUFlag_RealMode = 0x001;
			static const uint32_t kCPUFlag_V86 = 0x002;
			static const uint32_t kCPUFlag_ProtectedMode = 0x004;
			
			static const uint32_t kCPUFlag_InitKernelStack = 0x008;
			static const uint32_t kCPUFlag_StartingUp = 0x010;
			
			static const uint32_t kCPUFlag_Paging = 0x020;
			
			static const uint32_t kCPUFlag_BSP = 0x040;
			
			static const uint32_t kCPUFlag_Interrupts = 0x080;
			
			static const uint32_t kCPUFlag_Wait = 0x100;
			
			static const uint32_t kCPUFlag_Scheduling = 0x200;
			
			typedef struct
			{
				
				uint32_t Flags;
				uint32_t CurrentCLICount;
				
				uint8_t APICID;
				
				void * InitStackBottom;
				uint32_t InitStackLength;
				
				uint8_t Index;
				
				MT::HW::TSS :: TSS_t CrossPrivelegeInterruptTSS;
				
				MT::Synchronization::Spinlock :: Spinlock_t Lock;
				
				MT::Tasking::Task :: Task_t * IdleTask;
				
				MT::Tasking::Task :: Task_t * CurrentTask;
				
			} CPUInfo;
			
			static CPUInfo * GetCurrent ();
			
			static uint32_t GetCurrentAPICID32 ();
			
			static uint32_t GetProcessorCount ();
			static CPUInfo * GetProcessorByAPICID ( uint8_t ID );
			static CPUInfo * GetProcessorByIndex ( uint8_t ID );
			
		private:
			
			friend void MT :: MPInit ();
			
			static CPUInfo * Define ( bool BSP, uint8_t APICID, void * InitStackBottom, uint32_t InitStackLength );
			
			static CPUInfo Infos [ 0x100 ];
			static uint8_t IndexToAPICID [ 0x100 ];
			
			static uint8_t CIndex;
			
		};
		
	};
	
};

#endif
