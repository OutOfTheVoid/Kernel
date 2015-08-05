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
			
			static const bool kRegisterLength_24 = false;
			static const bool kRegisterLength_32 = true;
			
			static void TimerEvent ();
			
			static bool Exist;
			
			static bool RegisterLength;
			static uint64_t IntCount;
			
			//static MT::Synchronization::Spinlock :: Spinlock_t Lock;
			
			static bool IHandlerCompleted;
			
		};
		
	};
	
};

#endif
