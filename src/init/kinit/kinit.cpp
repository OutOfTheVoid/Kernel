#include <init/kinit/kinit.h>

#include <hw/video/VText.h>
#include <hw/ACPI/ACPI.h>

#include <hw/cpu/Processor.h>

#include <system/func/kprintf.h>
#include <system/func/panic.h>

#include <MM/MM.h>

#include <mt/MT.h>

#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

#include <fs/FS.h>

#include <system/System.h>
#include <system/permissions/UserList.h>

#include <util/string/string.h>

ASM_LINKAGE void InitTask ();

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
	
	System :: Init ();
	
	FS :: Init ( MultibootInfo );
	
	MT::Tasking :: Task * NewTask = MT::Tasking::Task :: CreateKernelTask ( "init", reinterpret_cast <void *> ( & InitTask ), 0x2000, MT::Tasking::Task :: kPriority_System_Max, MT::Tasking::Task :: kPriority_System_Min );
	
	MT::Tasking::Scheduler :: AddTask ( NewTask );
	
	for ( int i = 0; i < 1000000000; i ++ )
		;
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};

void InitTask ()
{
	
	system_func_kprintf ( "System user ID: %d\n", System::Permissions::UserList :: GetUserID ( "system" ) );
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};
