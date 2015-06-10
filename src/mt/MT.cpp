#include <mt/MT.h>
#include <mt/timing/PIT.h>
#include <mt/apinit/APTrampoline.h>

#include <system/func/KPrintF.h>
#include <system/func/Panic.h>
#include <system/func/PMalloc.h>

#include <mm/paging/PageTable.h>

#include <interrupt/APIC.h>

#include <hw/acpi/MADT.h>

void MT :: Init ()
{
	
	system_func_kprintf ( "MT :: Init ()\n" );
	
	system_func_kprintf ( "Initializiing PIT...\n" );
	
	Timing::PIT :: Init ();
	
	system_func_kprintf ( "Initializiing APTrampoline...\n" );
	
	APInit::APTrampoline :: Init ();
	APInit::APTrampoline :: SetPagingDirectory ( MM::Paging::PageTable :: GetKernelPD () );
	
	system_func_kprintf ( "Initializiing application processors...\n" );
	
	if ( HW::ACPI::MADT :: Valid () )
	{
		
		uint32_t ProcessorCount = HW::ACPI::MADT :: GetProcessorCount ();
		
		for ( uint32_t I = 0; I < ProcessorCount; I ++ )
		{
			
			system_func_kprintf ( "Initializiing AP %i...\n", I );
			
			if ( HW::ACPI::MADT :: GetProcessorEnabled ( I ) )
			{
				
				system_func_kprintf ( "Alloc KStack...\n" );
				
				void * InitStackBottom = system_func_pmalloc ( 1 );
				
				if ( InitStackBottom == NULL )
					KPANIC ( "Failed to allocate 16KB stack fot application processor!" );
				
				system_func_kprintf ( "Set KStack...\n" );
				
				APInit::APTrampoline :: SetStack ( reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( InitStackBottom ) + 0x4000 ) );
				
				
				
			}
			
		}
		
	}
	
};

