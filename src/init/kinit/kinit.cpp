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

#include <mt/timing/PWaitMS.h>

#include <math/bitmath.h>

C_LINKAGE void hw_cpu_hang ();

C_LINKAGE void init_kinit_kinit ( uint32_t Magic, multiboot_info_t * MultibootInfo )
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
	
	system_func_kprintf ( "MT Init...\n" );
	
	MT :: Init ();
	
	system_func_kprintf ( "Interrupt Init...\n" );
	
	Interrupt :: Init ();
	
	system_func_kprintf ( "System bus frequencey: %u MHz \n", static_cast <uint32_t> ( Interrupt::APIC :: GetBusFrequencey () / 1000000.0 ) );
	
	system_func_kprintf ( "BSP Initialized!\n" );
	
};
