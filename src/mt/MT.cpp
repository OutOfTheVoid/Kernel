#include <mt/MT.h>
#include <mt/timing/PWaitMS.h>
#include <mt/timing/PIT.h>
#include <mt/apinit/APTrampoline.h>

#include <system/func/KPrintF.h>
#include <system/func/Panic.h>
#include <system/func/PMalloc.h>

#include <mm/paging/PageTable.h>

#include <interrupt/Interrupt.h>
#include <interrupt/APIC.h>

#include <hw/acpi/ACPI.h>
#include <hw/acpi/MADT.h>
#include <hw/cpu/Processor.h>

#include <boot/BootImage.h>

void MT :: Init ()
{
	
	system_func_kprintf ( "MT :: Init ()\n" );
	
	Interrupt :: APICInitEarly ();
	
	system_func_kprintf ( "Initializiing PIT...\n" );
	
	Timing::PIT :: Init ();
	
	system_func_kprintf ( "Initializiing application processors...\n" );
	
	if ( ::HW::ACPI::MADT :: Valid () )
	{
		
		system_func_kprintf ( "Initializiing APTrampoline...\n" );
		
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
					
					system_func_kprintf ( "Initializiing AP (ID: %u, APIC ID: %u)...\n", I, APICID );
					
					void * InitStackBottom = system_func_pmalloc ( 4 );
					
					if ( InitStackBottom == NULL )
						KPANIC ( "Failed to allocate 16KB stack for application processor!" );
					
					APInit::APTrampoline :: SetStack ( reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( InitStackBottom ) + 0x4000 ) );
					
					::HW::CPU::Processor :: CPUInfo * APInfo = ::HW::CPU::Processor :: Define ( false, APICID, InitStackBottom, 0x4000 );
					
					Interrupt::APIC :: SendPhysicalInitIPI ( APICID, true );
					
					mt_timing_pwaitms ( 10.0 );
					
					Interrupt::APIC :: SendPhysicalStartupIPI ( APICID, APInit::APTrampoline :: GetStartupPage () );
					
					mt_timing_pwaitms ( 100.0 );
					
					MT::Synchronization::Spinlock :: SpinAcquire ( & APInfo -> Lock );
					
					bool Started = ( APInfo -> Flags & ::HW::CPU::Processor :: kCPUFlag_StartingUp ) == 0;
					
					MT::Synchronization::Spinlock :: Release ( & APInfo -> Lock );
					
					if ( ! Started )
						Interrupt::APIC :: SendPhysicalStartupIPI ( APICID, APInit::APTrampoline :: GetStartupPage () );
					
					while ( ! Started )
					{
						
						mt_timing_pwaitms ( 0.01 );
						
						MT::Synchronization::Spinlock :: SpinAcquire ( & APInfo -> Lock );
						
						Started = ( APInfo -> Flags & ::HW::CPU::Processor :: kCPUFlag_StartingUp ) == 0;
						
						MT::Synchronization::Spinlock :: Release ( & APInfo -> Lock );
						
					}
					
				}
				else
					::HW::CPU::Processor :: Define ( true, APICID, reinterpret_cast <void *> ( & boot_multiboot_StackBottom ), reinterpret_cast <uint32_t> ( & boot_multiboot_StackTop ) - reinterpret_cast <uint32_t> ( & boot_multiboot_StackBottom ) );
				
			}
			
		}
		
	}
	
};

