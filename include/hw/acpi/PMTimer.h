#ifndef HW_ACPI_PMTIMER_H
#define HW_ACPI_PMTIMER_H

#include <hw/acpi/ACPI.h>

#include <stdint.h>

#include <mt/synchronization/Spinlock.h>

namespace HW
{
	
	namespace ACPI
	{
		
		class PMTimer
		{
		public:
			
			static void Init ( uint32_t * Status );
			
			static bool Exists ();
			
			static uint64_t GetTimeNS ();
			
		private:
			
			static const uint32_t kRegisterMask_24 = 0x00FFFFFF;
			static const uint32_t kRegisterMask_32 = 0xFFFFFFFF;
			
			static const uint64_t kTimerFrequencey = 3579545;
			
			static uint64_t TimerUpdate ();
			
			static bool Exist;
			
			static uint32_t LastTValue;
			static uint32_t RegisterMask;
			static uint64_t Count;
			
			static uint32_t Address;
			static uint32_t AddressSpace;
			
			static MT::Synchronization::Spinlock :: Spinlock_t Lock;
			
		};
		
	};
	
};

#endif
