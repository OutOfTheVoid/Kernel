#include <mt/timing/HPET.h>

#include <hw/acpi/ACPI.h>

#include <hw/cpu/Processor.h>

#include <interrupt/IOAPIC.h>
#include <interrupt/APIC.h>

#include <system/func/Panic.h>

uint32_t MT::Timing::HPET :: GlobalSystemInterrupt = 0xFFFFFFFF;

bool MT::Timing::HPET :: CounterValid = false;

uint32_t MT::Timing::HPET :: CounterPeriod = 0;

MT::Synchronization::Spinlock :: Spinlock_t MT::Timing::HPET :: TimeoutLock = MT::Synchronization::Spinlock :: Initializer ();
MT::Synchronization::Spinlock :: Spinlock_t MT::Timing::HPET :: ServicedLock = MT::Synchronization::Spinlock :: Initializer ();

void ( * MT::Timing::HPET :: TimeoutCallback ) () = NULL;

uint64_t MT::Timing::HPET :: NextExternalTimout;
bool MT::Timing::HPET :: ScheduledTimeout;

uint32_t MT::Timing::HPET :: NextInternalTimeout;

uint64_t MT::Timing::HPET :: LastTimeValue;
uint64_t MT::Timing::HPET :: LastSerivcedTimeout;

::HW::ACPI::HPET :: HPETCounterInfo MT::Timing::HPET :: CounterInfo;

void MT::Timing::HPET :: Init ()
{
	
	uint32_t ACPIStatus = ::HW::ACPI :: kACPIStatus_Success;
	uint32_t I;
	
	CounterValid = false;
	
	if ( ! ::HW::ACPI::HPET :: Valid () )
		return;
	
	// I fucking hate interrupt hardware detection.
	// Look at this bullshit. I have to try allocating an interrupt so that I can try to find an HPET comparrator that supports delivery to it.
	// That's two levels of "trying" to make something connect.
	// Honestly what the hell were PC/ACPI designers thinking?????
	
	for ( I = kIRQ_Min; I <= kIRQ_Max; I ++ )
	{
		
		if ( Interrupt::IOAPIC :: TryAllocateGlobalSystemInterrupt ( I ) )
		{
			
			::HW::ACPI::HPET :: AllocCounter ( I, & CounterInfo, ::HW::ACPI::HPET :: kRequirement_None, & ACPIStatus );
			
			if ( ACPIStatus == ::HW::ACPI :: kACPIStatus_Success )
				break;
			
			Interrupt::IOAPIC :: FreeGlobalSystemInterrupt ( I );
			
		}
		
	}
	
	if ( I > kIRQ_Max )
		return;
	
	GlobalSystemInterrupt = I;
	
	// Active low, level triggered. Follows linux source code. Not entirely sure why?
	Interrupt::IOAPIC :: DefineLowestPriorityRedirectionEntry ( I, kInterruptVector_TimerHPET, false, false, true );
	Interrupt::InterruptHandlers :: SetInterruptHandler ( kInterruptVector_TimerHPET, TimeoutInterruptHandler );
	
	::HW::ACPI::HPET :: SetLegacyRoutingHPET ( CounterInfo.HPETInfoPointer, false );
	
	::HW::ACPI::HPET :: SetupCounter ( & CounterInfo, true, false );
	::HW::ACPI::HPET :: SetCounterComparratorWritable ( & CounterInfo, true );
	::HW::ACPI::HPET :: ConfigureCounterWidth32BitForced ( & CounterInfo, true );
	
	Interrupt::IOAPIC :: SetRedirectionEntryEnabled ( I, true );
	
	CounterPeriod = ::HW::ACPI::HPET :: GetHPETPeriodFemptoSeconds ( CounterInfo.HPETInfoPointer );
	
	CounterValid = true;
	
};

bool MT::Timing::HPET :: CounterInitialized ()
{
	
	return CounterValid;
	
};

void MT::Timing::HPET :: Start ()
{
	
	if ( ! CounterValid )
		return;
	
	NextExternalTimout = 0xFFFFFFFFFFFFFFFF;
	LastSerivcedTimeout = 0x0000000000000000;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & TimeoutLock );
	
	LastTimeValue = static_cast <uint64_t> ( ::HW::ACPI::HPET :: ReadMasterCounter32 ( CounterInfo.HPETInfoPointer ) );
	NextInternalTimeout = static_cast <uint32_t> ( LastTimeValue ) + kInterruptDelta_TimekeepingDefault;
	ScheduledTimeout = false;
	
	::HW::ACPI::HPET :: SetCounterComparratorWritable ( & CounterInfo, true );
	::HW::ACPI::HPET :: WriteCounterComparrator32 ( & CounterInfo, NextInternalTimeout );
	::HW::ACPI::HPET :: ClearCounterInterruptLevel ( & CounterInfo );
	::HW::ACPI::HPET :: SetCounterEnabled ( & CounterInfo, true );
	
	::HW::ACPI::HPET :: GlobalEnableHPET ( CounterInfo.HPETInfoPointer );
	
	MT::Synchronization::Spinlock :: Release ( & TimeoutLock );
	
};

void MT::Timing::HPET :: TimeoutInterruptHandler ( Interrupt::InterruptHandlers :: ISRFrame * )
{
	
	Synchronization::Spinlock :: SpinAcquire ( & TimeoutLock );
	
	UpdateAndRead ();
	
	void ( * TimeoutCallback ) () = NULL;
	
	system_func_kprintf ( "HPET: INTERRUPT! (LastTime: %h)\n", LastTimeValue );
	
	NextInternalTimeout = static_cast <uint32_t> ( LastTimeValue ) + kInterruptDelta_TimekeepingDefault;
	
	if ( ScheduledTimeout )
	{
		
		if ( NextExternalTimout <= LastTimeValue )
		{
			
			TimeoutCallback = HPET :: TimeoutCallback;
			
			ScheduledTimeout = false;
			NextExternalTimout = 0xFFFFFFFFFFFFFFFF;
			
		}
		else if ( NextExternalTimout < NextInternalTimeout )
		{
			
			NextInternalTimeout = NextExternalTimout;
			
			system_func_kprintf ( "HPET: Interrupt scheduled (%h)\n", NextInternalTimeout );
			
			::HW::ACPI::HPET :: SetCounterComparratorWritable ( & CounterInfo, true );
			::HW::ACPI::HPET :: WriteCounterComparrator32 ( & CounterInfo, NextInternalTimeout );
			::HW::ACPI::HPET :: SetCounterEnabled ( & CounterInfo, true );
			::HW::ACPI::HPET :: ClearCounterInterruptLevel ( & CounterInfo );
			
			UpdateAndRead ();
			
			if ( NextExternalTimout < LastTimeValue && ! ::HW::ACPI::HPET :: ReadCounterInterruptLevel ( & CounterInfo ) )
			{
				
				TimeoutCallback = HPET :: TimeoutCallback;
				
				ScheduledTimeout = false;
				NextExternalTimout = 0xFFFFFFFFFFFFFFFF;
				
				LastSerivcedTimeout = LastTimeValue;
				
			}
			else
			{
				
				MT::Synchronization::Spinlock :: Release ( & TimeoutLock );
				
				Interrupt::APIC :: EndOfInterrupt ();
				
				return;
				
			}
			
		}
		
	}
	
	system_func_kprintf ( "HPET: Interrupt scheduled (%h)\n", NextInternalTimeout );
	
	::HW::ACPI::HPET :: SetCounterComparratorWritable ( & CounterInfo, true );
	::HW::ACPI::HPET :: WriteCounterComparrator32 ( & CounterInfo, NextInternalTimeout );
	::HW::ACPI::HPET :: SetCounterEnabled ( & CounterInfo, true );
	::HW::ACPI::HPET :: ClearCounterInterruptLevel ( & CounterInfo );
	
	MT::Synchronization::Spinlock :: Release ( & TimeoutLock );
	
	Interrupt::APIC :: EndOfInterrupt ();
	
	if ( TimeoutCallback != NULL )
		return TimeoutCallback ();
	
};

void MT::Timing::HPET :: SetTimeoutCallback ( void ( * TimeoutCallback ) () )
{
	
	Synchronization::Spinlock :: SpinAcquire ( & TimeoutLock );
	
	HPET :: TimeoutCallback = TimeoutCallback;
	
	Synchronization::Spinlock :: Release ( & TimeoutLock );
	
};

void MT::Timing::HPET :: SetTimeoutNS ( uint64_t NanoSeconds )
{
	
	double FPDelta = 1000000.0 * static_cast <double> ( NanoSeconds );
	FPDelta /= static_cast <double> ( CounterPeriod );
	uint64_t Delta = static_cast <uint64_t> ( FPDelta );
	
	Synchronization::Spinlock :: SpinAcquire ( & TimeoutLock );
	
	UpdateAndRead ();
	
	uint64_t NextExternalTimoutLocal = LastTimeValue + Delta;
	
	void ( * TimeoutCallback ) () = NULL;
	
	if ( NextExternalTimoutLocal < NextExternalTimout )
	{
		
		NextExternalTimout = NextExternalTimoutLocal;
		
		if ( NextExternalTimout <= NextInternalTimeout )
		{
			
			NextInternalTimeout = static_cast <uint32_t> ( NextExternalTimout );
			ScheduledTimeout = true;
			
			system_func_kprintf ( "HPET: External interrupt scheduled (%h)\n", NextInternalTimeout );
			
			::HW::ACPI::HPET :: SetCounterComparratorWritable ( & CounterInfo, true );
			::HW::ACPI::HPET :: WriteCounterComparrator32 ( & CounterInfo, NextInternalTimeout );
			::HW::ACPI::HPET :: SetCounterEnabled ( & CounterInfo, true );
			::HW::ACPI::HPET :: ClearCounterInterruptLevel ( & CounterInfo );
			
			UpdateAndRead ();
			
			if ( NextExternalTimout < LastTimeValue && ! ::HW::ACPI::HPET :: ReadCounterInterruptLevel ( & CounterInfo ) )
			{
				
				TimeoutCallback = HPET :: TimeoutCallback;
				
				UpdateAndRead ();
				
				NextInternalTimeout = static_cast <uint32_t> ( LastTimeValue ) + kInterruptDelta_TimekeepingDefault;
				ScheduledTimeout = false;
				
				system_func_kprintf ( "HPET: ! Interrupt rescheduled (%h)\n", NextInternalTimeout );
	
				::HW::ACPI::HPET :: SetCounterComparratorWritable ( & CounterInfo, true );
				::HW::ACPI::HPET :: WriteCounterComparrator32 ( & CounterInfo, NextInternalTimeout );
				::HW::ACPI::HPET :: SetCounterEnabled ( & CounterInfo, true );
				::HW::ACPI::HPET :: ClearCounterInterruptLevel ( & CounterInfo );
	
				Synchronization::Spinlock :: Release ( & TimeoutLock );
	
				if ( TimeoutCallback != NULL )
					return TimeoutCallback ();
				
			}
			
		}
		
	}
	
	Synchronization::Spinlock :: Release ( & TimeoutLock );
	
};

uint64_t MT::Timing::HPET :: ReadTimeNS ()
{
	
	Synchronization::Spinlock :: SpinAcquire ( & TimeoutLock );
	
	UpdateAndRead ();
	
	uint64_t Time = LastTimeValue;
	
	Synchronization::Spinlock :: Release ( & TimeoutLock );
	
	return static_cast <uint64_t> ( ( static_cast <double> ( Time ) * 1000000.0 ) / static_cast <double> ( CounterPeriod ) );
	
};

uint64_t MT::Timing::HPET :: ReadTimeRaw ()
{
	
	Synchronization::Spinlock :: SpinAcquire ( & TimeoutLock );
	
	UpdateAndRead ();
	
	uint64_t Time = LastTimeValue;
	
	Synchronization::Spinlock :: Release ( & TimeoutLock );
	
	return Time;
	
};
