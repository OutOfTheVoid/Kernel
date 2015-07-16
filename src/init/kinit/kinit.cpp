#include <init/kinit/kinit.h>

#include <hw/cpu/hang.h>
#include <hw/cpu/CPUID.h>

#include <hw/video/VText.h>

#include <system/func/kprintf.h>
#include <system/func/panic.h>

#include <MM/MM.h>

#include <boot/bootimage.h>

#include <interrupt/IState.h>
#include <interrupt/Interrupt.h>
#include <interrupt/APIC.h>

#include <hw/acpi/ACPI.h>

#include <mt/MT.h>

#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

#include <mt/timing/PWaitMS.h>

#include <math/bitmath.h>

ASM_LINKAGE void hw_cpu_hang ();

ASM_LINKAGE void testKernelTask ();

ASM_LINKAGE void init_kinit_kinit ( uint32_t Magic, multiboot_info_t * MultibootInfo )
{
	
	if ( Magic != MULTIBOOT_BOOTLOADER_MAGIC )
		KPANIC ( "Multiboot bootloader magic number failed!" );

	if ( MultibootInfo == NULL )
		KPANIC ( "Multiboot information not supplied!" );

	HW::Video::VText :: Init ( HW::Video::VText :: MakeColor ( HW::Video::VText :: Color_White, HW::Video::VText :: Color_Black ) );
	
	system_func_kprintf ( "Kernel 0.0.5 dev\n(C) Liam Taylor 2015.\n\nMM Init...\n" );
	
	MM :: Init ( MultibootInfo );
	
	system_func_kprintf ( "ACPI Init...\n" );
	
	HW::ACPI :: StaticInit ();
	
	system_func_kprintf ( "MP Init...\n" );
	
	MT :: MPInit ();
	
	system_func_kprintf ( "Interrupt Init...\n" );
	
	Interrupt :: Init ();
	
	system_func_kprintf ( "MT Init...\n" );
	
	MT :: MTInit ();
	
	MT::Tasking::Task :: Task_t * NewTask = MT::Tasking::Task :: CreateKernelTask ( "Test", reinterpret_cast <void *> ( & testKernelTask ), 0x1000, 0 );
	MT::Tasking::Scheduler :: AddTask ( NewTask );
	
	system_func_kprintf ( "System bus frequecey: %i MHz", static_cast <uint32_t> ( Interrupt::APIC :: GetBusFrequencey () / 1000000.0 ) );
	
	while ( true )
	{
	
		system_func_kprintf ( "A\n" );
		
		for ( uint32_t I = 0; I < 0x80000000; I ++ );
			
	}
	
};

void testKernelTask ()
{
	
	while ( true )
	{
	
		system_func_kprintf ( "B\n" );
		
		for ( uint32_t I = 0; I < 0x70000000; I ++ );
			
	}
	
};
