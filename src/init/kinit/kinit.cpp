#include <init/kinit/kinit.h>

#include <hw/video/VText.h>
#include <hw/ACPI/ACPI.h>

#include <hw/cpu/Processor.h>

#include <system/func/kprintf.h>
#include <system/func/panic.h>

#include <MM/MM.h>

#include <interrupt/Interrupt.h>
#include <interrupt/IState.h>

#include <mt/MT.h>

#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

#include <mt/timing/TaskSleep.h>

#include <mm/PMalloc.h>
#include <mm/paging/Invalidation.h>

#include <math/bitmath.h>

#include <fs/FS.h>
#include <fs/vfs/FileSystem.h>
#include <fs/initrd/InitRamDisk.h>

#include <util/string/string.h>

ASM_LINKAGE void hw_cpu_hang ();

ASM_LINKAGE void testKernelTask ();
ASM_LINKAGE void testKernelTask2 ();

ASM_LINKAGE void init_kinit_kinit ( uint32_t Magic, multiboot_info_t * MultibootInfo )
{
	
	if ( Magic != MULTIBOOT_BOOTLOADER_MAGIC )
		KPANIC ( "Multiboot bootloader magic number failed!" );

	if ( MultibootInfo == NULL )
		KPANIC ( "Multiboot information not supplied!" );

	HW::Video::VText :: Init ( HW::Video::VText :: MakeColor ( HW::Video::VText :: Color_White, HW::Video::VText :: Color_Black ) );
	
	MM :: Init ( MultibootInfo, & MultibootInfo );
	
	uint32_t ACPIStatus;
	
	HW::ACPI :: StaticInit ( & ACPIStatus );
	
	if ( ACPIStatus != HW::ACPI :: kACPIStatus_Success )
		KPANIC ( "ACPI static init failure!" );
	
	MT :: MPInit ();
	Interrupt :: Init ();
	MT :: MTInit ();
	FS :: Init ( MultibootInfo );
	
	MT::Tasking::Task :: Task_t * NewTask2 = MT::Tasking::Task :: CreateKernelTask ( "Test2", reinterpret_cast <void *> ( & testKernelTask2 ), 0x2000, MT::Tasking::Task :: kPriority_System_Max + 2, MT::Tasking::Task :: kPriority_LowUser_Min );
	MT::Tasking::Task :: Task_t * NewTask = MT::Tasking::Task :: CreateKernelTask ( "Test", reinterpret_cast <void *> ( & testKernelTask ), 0x2000, MT::Tasking::Task :: kPriority_System_Max, MT::Tasking::Task :: kPriority_LowUser_Min );
	
	MT::Tasking::Scheduler :: AddTask ( NewTask );
	MT::Tasking::Scheduler :: AddTask ( NewTask2 );
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};

void testKernelTask ()
{
	
	while ( true )
	{
		
		for ( int i = 0; i < 100000000; i ++ )
			;
		
		volatile uint32_t Prio;
		
		bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
		
		::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
		volatile MT::Tasking::Task :: Task_t * ThisTask = ThisCPU -> CurrentTask;
		
		uint32_t MaxPrio;
		uint32_t MinPrio;
		
		Prio = ThisTask -> Priority;
		MaxPrio = ThisTask -> MaxPriority;
		MinPrio = ThisTask -> MinPriority;
		
		system_func_kprintf ( "        [A: %u of <%u, %u>]\n", Prio, MinPrio, MaxPrio );
		
		Interrupt::IState :: WriteBlock ( ReInt );
		
	}
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};

void testKernelTask2 ()
{
	
	bool ReInt = Interrupt::IState :: ReadAndSetBlock ();
	
	::HW::CPU::Processor :: CPUInfo * ThisCPU = ::HW::CPU::Processor :: GetCurrent ();
	volatile MT::Tasking::Task :: Task_t * ThisTask = ThisCPU -> CurrentTask;
	
	Interrupt::IState :: WriteBlock ( ReInt );
	
	while ( true )
	{
		
		system_func_kprintf ( "\n\n" );
		
		volatile uint32_t Prio = 0;
		
		while ( Prio < MT::Tasking::Task :: kPriority_LowUser_Min )
		{
			
			ReInt = Interrupt::IState :: ReadAndSetBlock ();
			
			uint32_t MaxPrio;
			uint32_t MinPrio;
			
			Prio = ThisTask -> Priority;
			MaxPrio = ThisTask -> MaxPriority;
			MinPrio = ThisTask -> MinPriority;
			
			Interrupt::IState :: WriteBlock ( ReInt );
			
			system_func_kprintf ( "(B: %u of <%u, %u>)\n", Prio, MinPrio, MaxPrio );
			
			for ( int i = 0; i < 100000000; i ++ )
				;
			
		}
		
		for ( int q = 0; q < 4; q ++ )
			MT::Timing::TaskSleep :: SleepCurrent ( 250 );
		
	}
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};
