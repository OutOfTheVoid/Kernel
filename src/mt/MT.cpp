#include <mt/MT.h>

#include <mt/timing/PWaitMS.h>
#include <mt/timing/PIT.h>
#include <mt/timing/TaskSleep.h>

#include <mt/exception/MPException.h>

#include <mt/apinit/APTrampoline.h>

#include <mt/tasking/Scheduler.h>

#include <system/func/KPrintF.h>
#include <system/func/Panic.h>

#include <mm/KMalloc.h>
#include <mm/PMalloc.h>
#include <mm/paging/PageTable.h>

#include <interrupt/Interrupt.h>
#include <interrupt/APIC.h>
#include <interrupt/IState.h>

#include <hw/acpi/ACPI.h>
#include <hw/acpi/MADT.h>
#include <hw/cpu/Processor.h>

#include <boot/BootImage.h>

bool __smp_Initialized = false;

void MT :: MPInit ()
{
	
	MT::Exception::MPException :: Init ();
	
	Timing::PIT :: InitPWait ();
	Interrupt :: APICInitEarly ();
	
	if ( ::HW::ACPI::MADT :: Valid () )
	{
		
		APInit::APTrampoline :: Init ();
		APInit::APTrampoline :: SetPagingDirectory ( MM::Paging::PageTable :: GetKernelPD () );
		
		uint32_t ProcessorCount = ::HW::ACPI::MADT :: GetProcessorCount ();
		uint32_t I;
		
		for ( I = 0; I < ProcessorCount; I ++ )
		{
			
			if ( ::HW::ACPI::MADT :: GetProcessorEnabled ( I ) )
			{
				
				uint8_t APICID = ::HW::ACPI::MADT :: GetProcessorLAPICID ( I );
				bool IsBSP = Interrupt::APIC :: GetLocalID () == APICID;
				
				if ( ! IsBSP )
				{
					
					void * InitStackBottom = mm_pmalloc ( 4 );
					
					if ( InitStackBottom == NULL )
						KPANIC ( "Failed to allocate 16KB stack for application processor!" );
					
					APInit::APTrampoline :: SetStack ( reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( InitStackBottom ) + 0x4000 ) );
					
					::HW::CPU::Processor :: CPUInfo * APInfo = ::HW::CPU::Processor :: Define ( false, APICID, InitStackBottom, 0x4000 );
					
					Interrupt::APIC :: SendPhysicalInitIPI ( APICID, true );
					
					mt_timing_pwaitms ( 10.0 );
					
					Interrupt::APIC :: SendPhysicalStartupIPI ( APICID, APInit::APTrampoline :: GetStartupPage () );
					
					mt_timing_pwaitms ( 100.0 );
					
					Synchronization::Spinlock :: SpinAcquire ( & APInfo -> Lock );
					
					bool Started = ( APInfo -> Flags & ::HW::CPU::Processor :: kCPUFlag_StartingUp ) == 0;
					
					Synchronization::Spinlock :: Release ( & APInfo -> Lock );
					
					if ( ! Started )
						Interrupt::APIC :: SendPhysicalStartupIPI ( APICID, APInit::APTrampoline :: GetStartupPage () );
					
					while ( ! Started )
					{
						
						mt_timing_pwaitms ( 0.01 );
						
						Synchronization::Spinlock :: SpinAcquire ( & APInfo -> Lock );
						
						Started = ( APInfo -> Flags & ::HW::CPU::Processor :: kCPUFlag_StartingUp ) == 0;
						
						Synchronization::Spinlock :: Release ( & APInfo -> Lock );
						
					}
					
				}
				else
					::HW::CPU::Processor :: Define ( true, APICID, reinterpret_cast <void *> ( & boot_multiboot_StackBottom ), reinterpret_cast <uint32_t> ( & boot_multiboot_StackTop ) - reinterpret_cast <uint32_t> ( & boot_multiboot_StackBottom ) );
				
			}
			
		}
		
	}
	
};

void MT :: MTInit ()
{
	
	Timing::TaskSleep :: Init ();
	
	Tasking::Scheduler :: Init ();
	Tasking::Scheduler :: PInit ( Tasking::Task :: kPriority_System_Max );
	
	uint32_t I;
	
	::HW::CPU::Processor :: CPUInfo * TargetCPUInfo;
	
	for ( I = 0; I < ::HW::CPU::Processor :: GetProcessorCount (); I ++ )
	{
		
		TargetCPUInfo = ::HW::CPU::Processor :: GetProcessorByIndex ( I );
		
		MT::Synchronization::Spinlock :: SpinAcquire ( & TargetCPUInfo -> Lock );
		
		if ( ( TargetCPUInfo -> Flags & ::HW::CPU::Processor :: kCPUFlag_BSP ) == 0 )
			TargetCPUInfo -> Flags &= ! ::HW::CPU::Processor :: kCPUFlag_Wait;
		
		Synchronization::Spinlock :: Release ( & TargetCPUInfo -> Lock );
		
	}
	
	bool InitSchedulersUnintialized = true;
	
	while ( InitSchedulersUnintialized )
	{
		
		InitSchedulersUnintialized = false;
		
		I = 0;
		
		while ( I < ::HW::CPU::Processor :: GetProcessorCount () )
		{
			
			TargetCPUInfo = ::HW::CPU::Processor :: GetProcessorByIndex ( I );
			
			if ( ( TargetCPUInfo -> Flags & ::HW::CPU::Processor :: kCPUFlag_Scheduling ) == 0 )
				InitSchedulersUnintialized = true;
			
			I ++;
			
		}
		
	}
	
	liballoc_switchlockmode ();
	
};
