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

#include <hw/cpu/Processor.h>

#include <mt/MT.h>

#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

#include <mt/timing/PWaitMS.h>

#include <math/bitmath.h>

ASM_LINKAGE void hw_cpu_hang ();

ASM_LINKAGE void testKernelTask ();
ASM_LINKAGE void testKernelTask2 ();
ASM_LINKAGE void testKernelTask3 ();

ASM_LINKAGE void init_kinit_kinit ( uint32_t Magic, multiboot_info_t * MultibootInfo )
{
	
	if ( Magic != MULTIBOOT_BOOTLOADER_MAGIC )
		KPANIC ( "Multiboot bootloader magic number failed!" );

	if ( MultibootInfo == NULL )
		KPANIC ( "Multiboot information not supplied!" );

	HW::Video::VText :: Init ( HW::Video::VText :: MakeColor ( HW::Video::VText :: Color_White, HW::Video::VText :: Color_Black ) );
	MM :: Init ( MultibootInfo );
	HW::ACPI :: StaticInit ();
	MT :: MPInit ();
	Interrupt :: Init ();
	MT :: MTInit ();
	
	MT::Tasking::Task :: Task_t * NewTask = MT::Tasking::Task :: CreateKernelTask ( "Test", reinterpret_cast <void *> ( & testKernelTask ), 0x1000, 0 );
	MT::Tasking::Scheduler :: AddTask ( NewTask );
	MT::Tasking::Task :: Task_t * NewTask2 = MT::Tasking::Task :: CreateKernelTask ( "Test2", reinterpret_cast <void *> ( & testKernelTask2 ), 0x1000, 0 );
	MT::Tasking::Scheduler :: AddTask ( NewTask2 );
	MT::Tasking::Task :: Task_t * NewTask3 = MT::Tasking::Task :: CreateKernelTask ( "Test3", reinterpret_cast <void *> ( & testKernelTask3 ), 0x1000, 0 );
	MT::Tasking::Scheduler :: AddTask ( NewTask3 );
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};

void testKernelTask ()
{
	
	float F = 9000.0;
	
	while ( true )
	{
		
		for ( uint32_t I = 0; I < 0x10000000; I ++ )
		{
			
			F = 9000.0;
			F *= 1.1;
			
		}
		
		for ( uint32_t I = 0; I < 0x10000000; I ++ );
		
		HW::CPU::Processor :: CPUInfo * ThisCPU = HW::CPU::Processor :: GetCurrent ();
		
		system_func_kprintf ( "Task 1, CPU %i; %s\n", ThisCPU -> Index, ( static_cast <int32_t> ( F ) == 9900 ) ? "FPU: YES!" : "FPU: NO!!!!!!" );
		
	}
	
};

void testKernelTask2 ()
{
	
	float F = 8000.0;
	
	while ( true )
	{
		
		for ( uint32_t I = 0; I < 0x10000000; I ++ )
		{
			
			F = 8000.0;
			F *= 1.1;
			
		}
		
		for ( uint32_t I = 0; I < 0x10000000; I ++ );
		
		HW::CPU::Processor :: CPUInfo * ThisCPU = HW::CPU::Processor :: GetCurrent ();
		
		system_func_kprintf ( "Task 2, CPU %i; %s\n", ThisCPU -> Index, ( static_cast <int32_t> ( F ) == 8800 ) ? "FPU: YES!" : "FPU: NO!!!!!!" );
		
	}
	
};

void testKernelTask3 ()
{
	
	float F = 7000.0;
	
	while ( true )
	{
		
		for ( uint32_t I = 0; I < 0x10000000; I ++ )
		{
			
			F = 7000.0;
			F *= 1.1;
			
		}
		
		for ( uint32_t I = 0; I < 0x10000000; I ++ );
		
		HW::CPU::Processor :: CPUInfo * ThisCPU = HW::CPU::Processor :: GetCurrent ();
		
		system_func_kprintf ( "Task 3, CPU %i; %s\n", ThisCPU -> Index, ( static_cast <int32_t> ( F ) == 7700 ) ? "FPU: YES!" : "FPU: NO!!!!!!" );
		
	}
	
};
