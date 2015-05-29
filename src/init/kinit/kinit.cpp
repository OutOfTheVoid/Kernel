#include <init/kinit/kinit.h>

#include <hw/cpu/hang.h>
#include <hw/cpu/GDT.h>
#include <hw/cpu/CPUID.h>

#include <hw/video/VText.h>

#include <system/func/kprintf.h>
#include <system/func/panic.h>

#include <mm/MM.h>
#include <mm/KMalloc.h>

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
	
	system_func_kprintf ( "Kernel 0.0.3 dev\n(C) Liam Taylor 2015.\n\n" );
	
	MM :: Init ( MultibootInfo );
	
	HW::CPU::GDT :: Init ( 3 );
	HW::CPU::GDT :: Swap ();
	
	system_func_kprintf ( "Initialized!\n" );
	
	hw_cpu_hang ();
	
};
