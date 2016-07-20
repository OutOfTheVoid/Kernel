#include <init/kinit/kinit.h>

#include <hw/video/VText.h>
#include <hw/ACPI/ACPI.h>

#include <hw/cpu/Processor.h>

#include <system/func/kprintf.h>
#include <system/func/panic.h>

#include <MM/MM.h>

#include <MM/Paging/PageTable.h>
#include <MM/Paging/AddressSpace.h>

#include <mt/MT.h>

#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

#include <mt/process/UserProcess.h>

#include <mt/timing/TaskSleep.h>
#include <mt/timing/HPET.h>

#include <fs/FS.h>

#include <system/System.h>
#include <system/permissions/UserList.h>

#include <util/string/string.h>

#include <hw/cpu/hang.h>

ASM_LINKAGE void InitTask ();
ASM_LINKAGE void UserModeTask ();

void HPETInterrupt ();

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
	
	MT::Timing::HPET :: SetTimeoutCallback ( HPETInterrupt );
	MT::Timing::HPET :: SetTimeoutNS ( 100000000 );
	
	while ( true )
	{
		
		MT::Timing::TaskSleep :: SleepCurrent ( 1000 );
		
		//system_func_kprintf ( "HPET Time: %h\n", static_cast <uint32_t> ( MT::Timing::HPET :: ReadTimeRaw () ) );
		
	}
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};

uint32_t TDSCount = 0;

void HPETInterrupt ()
{
	
	TDSCount ++;
	
	system_func_kprintf ( "CALLBACK! HPET Time: %h\n", static_cast <uint32_t> ( MT::Timing::HPET :: ReadTimeRaw () ) );
	
	MT::Timing::HPET :: SetTimeoutNS ( 100000000 );
	
};

void InitTask ()
{
	
	uint32_t Error;
	
	MM::Paging::PageTable * PTable = new MM::Paging :: PageTable ( true );
	
	MM::Paging :: AddressSpace * MSpace = new MM::Paging :: AddressSpace ();
	MM::Paging::AddressSpace :: CreateAddressSpace ( MSpace, false, MM :: kUserVM_Start, MM :: kUserVM_Length, & Error );
	
	if ( Error != MM::Paging::AddressSpace :: kCreateAddressSpace_Error_None )
		KPANIC ( "Failed to create userspace virtual memory!" );
	
	uint32_t EntryPhys = MM::Paging::PageTable :: KernelVirtualToPhysical ( reinterpret_cast <uint32_t> ( & UserModeTask ) );
	MM::Paging::PageTable :: SetKernelMapping ( reinterpret_cast <uint32_t> ( & UserModeTask ) & 0xFFFFF000, EntryPhys & 0xFFFFF000, MM::Paging::PageTable :: Flags_User | MM::Paging::PageTable :: Flags_Present | MM::Paging::PageTable :: Flags_Writeable );
	
	MT::Process :: UserProcess * UProcess = MT::Process::UserProcess :: CreateUserProcess ( "USERLAND!", PTable, MSpace, NULL );
	MT::Tasking :: Task * UTask = MT::Tasking::Task :: CreateUserTask ( "userland task 1", UProcess, reinterpret_cast <void *> ( & UserModeTask ), 0x1000, 0x8000, MT::Tasking::Task :: kPriority_LowUser_Max, MT::Tasking::Task :: kPriority_LowUser_Min );
	
	MT::Tasking::Scheduler :: AddTask ( UTask );
	
	MT::Timing::TaskSleep :: SleepCurrent ( 100 );
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};

void UserModeTask ()
{
	
	__asm__ volatile (
		"mov eax, 0\n"
		"int 0x80"
		);
	
};
