#ifndef MT_TIMING_HPET_H
#define MT_TIMING_HPET_H

#include <stdint.h>

#include <mt/timing/Timing.h>

#include <mt/synchronization/Spinlock.h>

#include <interrupt/InterruptHandlers.h>

#include <hw/acpi/HPET.h>

namespace MT
{

	namespace Timing
	{

		class HPET
		{
		public:
			
			static void Init ();
			
			static bool CounterInitialized ();
			
			static void SetTimeoutCallback ( void ( * TimeoutCallback ) () );
			
			static void SetTimeoutNS ( uint64_t NanoSeconds );
			
			static void Start ();
			
			static uint64_t ReadTimeNS ();
			static uint64_t ReadTimeRaw ();
			
		private:
			
			static const uint32_t kInterruptDelta_TimekeepingDefault = 0x80000000;
			
			static const uint32_t kIRQ_Min = 0x10;
			static const uint32_t kIRQ_Max = 0x19;
			static const uint8_t kInterruptVector_TimerHPET = 0x22;
			
			static void TimeoutInterruptHandler ( Interrupt::InterruptHandlers :: ISRFrame * );
			
			static uint32_t GlobalSystemInterrupt;
			
			static bool CounterValid;
			
			static uint32_t CounterPeriod;
			
			static Synchronization::Spinlock :: Spinlock_t TimeoutLock;
			static Synchronization::Spinlock :: Spinlock_t ServicedLock;
			
			static void ( * TimeoutCallback ) ();
			static uint64_t NextExternalTimout;
			
			static bool ScheduledTimeout;
			
			static uint32_t NextInternalTimeout;
			static uint64_t LastTimeValue;
			
			static uint64_t LastSerivcedTimeout;
			
			static ::HW::ACPI::HPET :: HPETCounterInfo CounterInfo;
			
			static inline uint32_t UpdateAndRead ()
			{
				
				volatile uint32_t CurrentTVal = ::HW::ACPI::HPET :: ReadMasterCounter32 ( CounterInfo.HPETInfoPointer );
				
				if ( static_cast <uint32_t> ( LastTimeValue & 0x00000000FFFFFFFF ) > CurrentTVal )
					LastTimeValue = ( ( LastTimeValue & 0xFFFFFFFF00000000 ) | static_cast <uint64_t> ( CurrentTVal ) ) + 0x100000000;
				else
					LastTimeValue = ( LastTimeValue & 0xFFFFFFFF00000000 ) | static_cast <uint64_t> ( CurrentTVal );
				
				return CurrentTVal;
				
			};
			
		};

	};

};

#endif
