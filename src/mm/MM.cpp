#include <mm/MM.h>

#include <mm/paging/PageTable.h>
#include <mm/paging/PFA.h>
#include <mm/paging/AddressSpace.h>

#include <mm/segmentation/GDT.h>

#include <system/func/kprintf.h>

#include <math/IntMath.h>

#include <boot/bootimage.h>

#include <KernelDef.h>

void MM :: Init ( multiboot_info_t * MultibootInfo )
{
	
#ifdef KSTARTUP_DEBUG
	system_func_kprintf ( "Memory manager initializing:\nPage frame allocator init...\n" );
#endif
	
	Paging::PFA :: Init ( MultibootInfo );
	
#ifdef KSTARTUP_DEBUG
	system_func_kprintf ( "Physical memory - %u KB Total, %u KB Free\nKernel page table init...\n", Paging::PFA :: GetTotalKB (), Paging::PFA :: GetFreeKB () );
#endif
	
	Paging::PageTable :: KInit ();
	Paging::PageTable :: EnableKPaging ();
	
#ifdef KSTARTUP_DEBUG
	system_func_kprintf ( "Kernel address space init...\n" );
#endif
	
	uint32_t KAddStart = math_max ( reinterpret_cast <uint32_t> ( & __kend ), KernelVMStart );
	
	Paging::AddressSpace :: KInit ( KAddStart, KernelVMLength - KAddStart );
	
#ifdef KSTARTUP_DEBUG
	system_func_kprintf ( "GDT init...\n" );
#endif
	
	Segmentation::GDT :: Init ( 3 );
	Segmentation::GDT :: Swap ();
	
#ifdef KSTARTUP_DEBUG
	system_func_kprintf ( "Memory manager initialized!\n\n" );
#endif
	
};

