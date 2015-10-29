#include <mm/paging/Invalidation.h>

#include <interrupt/APIC.h>

#include <mt/tasking/Task.h>

#include <hw/cpu/Processor.h>
#include <hw/cpu/Atomic.h>

#include <cpputil/Unused.h>

uint32_t MM::Paging::Invalidation :: CR3 = 0;

uint32_t MM::Paging::Invalidation :: VirtualStart = 0;
uint32_t MM::Paging::Invalidation :: PageCount = 0;

uint32_t MM::Paging::Invalidation :: ProccessorCount;

bool MM::Paging::Invalidation :: Inited = false;

MT::Synchronization::Spinlock :: Spinlock_t MM::Paging::Invalidation :: InvalLock = MT::Synchronization::Spinlock :: Initializer ();


void MM::Paging::Invalidation :: InitInterrupt ()
{
	
	Interrupt::InterruptHandlers :: SetInterruptHandler ( kVector_InvalidateIPI, & InvalidationHandler );
	
};

void MM::Paging::Invalidation :: InvalidatePages ( uint32_t CR3, uint32_t PageAddress, uint32_t Count )
{
	
	if ( ! Inited )
		return;
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & InvalLock );
	
	Invalidation :: CR3 = CR3;
	
	VirtualStart = PageAddress;
	PageCount = Count;
	
	ProccessorCount = 1;
	
	Interrupt::APIC :: SendBroadFixedIPI ( kVector_InvalidateIPI );
	
	uint32_t TotalCount = HW::CPU::Processor :: GetProcessorCount ();
	
	while ( ProccessorCount < TotalCount )
		__asm__ volatile ( "pause" );
	
	MT::Synchronization::Spinlock :: Release ( & InvalLock );
	
};

bool MM::Paging::Invalidation :: PageFault ( uint32_t ErrorCode )
{
	
	( void ) ErrorCode;
	
	uint32_t Address = GetCR2 (); 
	
	( void ) Address;
	
	PageTable * Mapping = HW::CPU::Processor :: GetCurrent () -> CurrentTask -> MemoryMapping;
	
	return true;
	
};

void MM::Paging::Invalidation :: InvalidationHandler ( Interrupt::InterruptHandlers :: ISRFrame * Frame )
{
	
	UNUSED ( Frame );
	
	uint32_t CR3 = Invalidation :: CR3;
	
	uint32_t VirtualStart = Invalidation :: VirtualStart;
	uint32_t PageCount = Invalidation :: PageCount;
	
	HW::CPU::Atomic :: Increment ( & ProccessorCount );
	
	if ( MM::Paging::PageTable :: ReadCR3 () == CR3 )
	{
		
		for ( uint32_t I = 0; I < PageCount; I ++ )
			MM::Paging::PageTable :: FlushEntry ( VirtualStart + 0x1000 * I );
		
	}
	
	Interrupt::APIC :: EndOfInterrupt ();
	
};
