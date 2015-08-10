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
			
			static bool Interrupt ();
			
			static SCIHandlerHook TimerIntHook;
			
			static const uint32_t kRegisterMask_24 = 0x00FFFFFF;
			static const uint32_t kRegisterMask_32 = 0xFFFFFFFF;
			
			static const uint64_t kTimerFrequencey = 3579545;
			
			static uint64_t TimerUpdate ();
			
			// Tells whether we've detected and intialized a valid PM Timer.
			
			static bool Exist;
			
			// These deal with emulating a larger timer.
			
			static uint32_t LastTValue;
			static uint32_t RegisterMask;
			static uint64_t Count;
			
			// Actual address of the timer value register.
			
			static uint32_t TimerAddress;
			static uint32_t TimerAddressSpace;
			
			// Address of the event/status register that contains TMR_STS
			
			static uint32_t EventAddressA;
			static uint32_t EventAddressB;
			static uint32_t EventAddressSpaceA;
			static uint32_t EventAddressSpaceB;
			
			// Lock to prevent corruption of timer values.
			
			static MT::Synchronization::Spinlock :: Spinlock_t Lock;
			
		};
		
	};
	
};

#endif
