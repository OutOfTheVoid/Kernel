#include <init/kinit/kinit.h>

#include <hw/video/VText.h>
#include <hw/ACPI/ACPI.h>

#include <system/func/kprintf.h>
#include <system/func/panic.h>

#include <MM/MM.h>

#include <interrupt/Interrupt.h>

#include <mt/MT.h>

#include <mt/tasking/Task.h>
#include <mt/tasking/Scheduler.h>

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
	
	MT::Tasking::Task :: Task_t * NewTask2 = MT::Tasking::Task :: CreateKernelTask ( "Test2", reinterpret_cast <void *> ( & testKernelTask2 ), 0x2000, 0 );
	MT::Tasking::Task :: Task_t * NewTask = MT::Tasking::Task :: CreateKernelTask ( "Test", reinterpret_cast <void *> ( & testKernelTask ), 0x2000, 0 );
	
	MT::Tasking::Scheduler :: AddTask ( NewTask );
	MT::Tasking::Scheduler :: AddTask ( NewTask2 );
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};

void DirTree ( const char * PreviousPath, uint32_t Level )
{
	
	uint32_t PreviousPathLength = strlen ( PreviousPath );
	
	FS :: FSStatus_t Status = FS :: kFSStatus_Success;
	uint32_t Index = 0;
	
	while ( true )
	{
		
		const char * Name;
		
		FS :: Enumerate ( PreviousPath, Index, & Name, & Status );
		
		if ( Status != FS :: kFSStatus_Success )
			return;
		
		for ( uint32_t I = 0; I < Level; I ++ )
			system_func_kprintf ( "|   " );
			
		system_func_kprintf ( "%s\n", Name, Name );
		
		uint32_t NameLength = strlen ( Name );
		
		char SubName [ PreviousPathLength + NameLength + 2 ];
		
		strcpy ( SubName, PreviousPath );
		SubName [ PreviousPathLength - 1 ] = '/';
		strcpy ( & SubName [ PreviousPathLength ], Name );
		
		DirTree ( SubName, Level + 1 );
		
		Index ++;
		
	}
	
};

void testKernelTask ()
{
	
	DirTree ( "", 0 );
	
	uint8_t Buff [ 0x1000 ];
	
	memzero ( Buff, 0x1000 );
	
	FS :: FSStatus_t Status;
	FS :: FSNode * File = NULL;
	FS :: Open ( "/Mount/initrd/testFolder/test3.txt", & File, & Status );
	
	if ( Status == FS :: kFSStatus_Success )
	{
		
		FS :: Read ( File, Buff, 0, 0x1000, & Status );
		
		if ( Status == FS :: kFSStatus_Success )
			system_func_kprintf ( "File contents of test3.txt: \"%s\"\n", Buff );
		
		FS :: Close ( File, & Status );
	
	}
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};

void testKernelTask2 ()
{
	
	uint8_t Buff [ 0x1000 ];
	
	memzero ( Buff, 0x1000 );
	
	FS :: FSStatus_t Status;
	FS :: FSNode * File = NULL;
	FS :: Open ( "/Mount/initrd/test.txt", & File, & Status );
	
	if ( Status == FS :: kFSStatus_Success )
	{
		
		FS :: Read ( File, Buff, 0, 0x1000, & Status );
		
		if ( Status == FS :: kFSStatus_Success )
			system_func_kprintf ( "File contents of test.txt: \"%s\"\n", Buff );
		
		FS :: Close ( File, & Status );
	
	}
	
	MT::Tasking::Scheduler :: KillCurrentTask ();
	
};
