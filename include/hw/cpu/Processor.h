#ifndef HW_CPU_PROCESSOR_H
#define HW_CPU_PROCESSOR_H

#include <hw/cpu/CPU.h>

#include <mt/MT.h>
#include <mt/synchronization/Spinlock.h>

#include <stdint.h>

namespace HW
{
	
	namespace CPU
	{
		
		class Processor
		{
		public:
			
			static const uint32_t kCPUFlag_RealMode = 0x01;
			static const uint32_t kCPUFlag_V86 = 0x02;
			static const uint32_t kCPUFlag_ProtectedMode = 0x04;
			
			static const uint32_t kCPUFlag_InitKernelStack = 0x08;
			static const uint32_t kCPUFlag_StartingUp = 0x10;
			
			static const uint32_t kCPUFlag_Paging = 0x20;
			
			static const uint32_t kCPUFlag_BSP = 0x40;
			
			typedef struct
			{
				
				uint32_t Flags;
				uint32_t CurrentCLICount;
				
				uint8_t APICID;
				
				void * InitStackBottom;
				uint32_t InitStackLength;
				
				uint8_t Index;
				
				MT::Synchronization::Spinlock :: Spinlock_t Lock;
				
			} CPUInfo;
			
			static CPUInfo * GetCurrent ();
			
		private:
			
			friend void MT :: Init ();
			
			static CPUInfo * Define ( bool BSP, uint8_t APICID, void * InitStackBottom, uint32_t InitStackLength );
			
			static CPUInfo Infos [ 0x100 ];
			static uint8_t IndexToAPICID [ 0x100 ];
			
			static uint8_t CIndex;
			
		};
		
	};
	
};

#endif
