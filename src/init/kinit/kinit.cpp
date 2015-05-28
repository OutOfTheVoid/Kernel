#include <init/kinit/kinit.h>

#include <hw/cpu/hang.h>
#include <hw/cpu/GDT.h>
#include <hw/cpu/CPUID.h>

#include <hw/video/VText.h>

#include <system/func/kprintf.h>
#include <system/func/panic.h>

#include <mm/paging/PageTable.h>
#include <mm/paging/PFA.h>
#include <mm/paging/AddressSpace.h>

#include <interrupt/InterruptHandlers.h>

#include <boot/bootimage.h>

#include <interrupt/IState.h>
#include <interrupt/PIC.h>
#include <interrupt/APIC.h>

#include <math/bitmath.h>

C_LINKAGE void init_kinit_kinit ( uint32_t Magic, multiboot_info_t * MultibootInfo )
{
	
	if ( Magic != MULTIBOOT_BOOTLOADER_MAGIC )
		KPANIC ( "Multiboot bootloader magic number failed!" );

	if ( MultibootInfo == NULL )
		KPANIC ( "Multiboot information not supplied!" );
	
	Interrupt::IState :: Init ();
	Interrupt::IState :: IncrementBlock ();

	HW::Video::VText :: Init ( HW::Video::VText :: MakeColor ( HW::Video::VText :: Color_LightBlue, HW::Video::VText :: Color_LightGrey ) );
	
	system_func_kprintf ( "Kernel 0.0.2 dev\n(C) Liam Taylor 2015.\n\n" );
	
	MM::Paging::PFA :: Init ( MultibootInfo );
	
	MM::Paging::PageTable :: KInit ( MultibootInfo );
	
	HW::CPU::GDT :: Init ( 3 );
	HW::CPU::GDT :: Swap ();
	
	MM::Paging::AddressSpace :: KInit ( reinterpret_cast <uint32_t> ( & __kend ), 0x100000 );
	
	//system_func_kprintf ( "KADR-INIT: %h\n", reinterpret_cast <uint32_t> ( & __kend ) );
	
	MM::Paging::AddressSpace * KAddrSpace = MM::Paging::AddressSpace :: ReteriveKernelSpace ();
	
	uint32_t Error;
	
	void * Allocs [ 16 ];
	
	KAddrSpace -> DebugPrint ();
	
	for ( uint32_t i = 0; i < 16; i ++ )
	{
		
		KAddrSpace -> Alloc ( 0x1000 << i, & Allocs [ i ], & Error );
		
		if ( Error != MM::Paging::AddressSpace :: kAlloc_Error_None )
			system_func_kprintf ( "Allocation error: %s\n", MM::Paging::AddressSpace :: GetErrorString ( Error ) );
		
	}
	
	for ( uint32_t i = 0; i < 16; i ++ )
	{
		
		KAddrSpace -> Free ( Allocs [ i ], & Error );
		
		if ( Error != MM::Paging::AddressSpace :: kAlloc_Error_None )
			system_func_kprintf ( "Freeing error: %s\n", MM::Paging::AddressSpace :: GetErrorString ( Error ) );
		
	}
	
	KAddrSpace -> DebugPrint ();
	
	/*Interrupt::PIC :: Init ();
	Interrupt::APIC :: Init ();
	
	Interrupt::InterruptHandlers :: Init ();*/
	
	//system_func_kprintf ( "Initialized!\n\n" );
	
	
	hw_cpu_hang ();
	
};
