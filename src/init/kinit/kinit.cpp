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
#include <interrupt/InterruptHandlers.h>
#include <interrupt/APIC.h>

#include <hw/acpi/ACPI.h>

#include <hw/cpu/Processor.h>
#include <hw/cpu/TSC.h>

#include <hw/PC/ISA.h>

#include <mt/MT.h>

#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

#include <mt/timing/TaskSleep.h>
#include <mt/timing/PIT.h>

#include <mt/timing/PWaitMS.h>

#include <math/bitmath.h>

#include <fs/FS.h>

ASM_LINKAGE void hw_cpu_hang ();

ASM_LINKAGE void testKernelTask ();

ASM_LINKAGE void init_kinit_kinit ( uint32_t Magic, multiboot_info_t * MultibootInfo )
{
	
	if ( Magic != MULTIBOOT_BOOTLOADER_MAGIC )
		KPANIC ( "Multiboot bootloader magic number failed!" );

	if ( MultibootInfo == NULL )
		KPANIC ( "Multiboot information not supplied!" );

	HW::Video::VText :: Init ( HW::Video::VText :: MakeColor ( HW::Video::VText :: Color_White, HW::Video::VText :: Color_Black ) );
	MM :: Init ( MultibootInfo );
	
	uint32_t ACPIStatus;
	
	HW::ACPI :: StaticInit ( & ACPIStatus );
	
	if ( ACPIStatus != HW::ACPI :: kACPIStatus_Success )
		KPANIC ( "ACPI static init failure!" );
	
	MT :: MPInit ();
	Interrupt :: Init ();
	MT :: MTInit ();
	FS :: Init ();
	
	MT::Tasking::Task :: Task_t * NewTask = MT::Tasking::Task :: CreateKernelTask ( "Test", reinterpret_cast <void *> ( & testKernelTask ), 0x2000, 0 );
	MT::Tasking::Scheduler :: AddTask ( NewTask );
	
	HW::ACPI :: Enable ( & ACPIStatus );
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};

void testKernelTask ()
{
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};
