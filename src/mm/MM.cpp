#include <mm/MM.h>

#include <mm/paging/PageTable.h>
#include <mm/paging/PFA.h>
#include <mm/paging/AddressSpace.h>
#include <mm/segmentation/GDT.h>

#include <system/func/kprintf.h>

#include <math/IntMath.h>

#include <boot/bootimage.h>

#include <mt/APInit/APTrampoline.h>

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
	
	Segmentation::GDT :: Init ( 7 );
	
	Segmentation::GDT :: SetDataEntry32 ( 3, 0, 0xFFFFFFFF, 3, true );
	Segmentation::GDT :: SetDataEntry32 ( 4, 0, 0xFFFFFFFF, 3, true );
	
	//Real Mode -> Protected Mode trampoline segments. Base = tramp_start for highmem real mode.
	Segmentation::GDT :: SetCodeEntry16 ( 5, reinterpret_cast <uint32_t> ( & __krealbegin ), reinterpret_cast <uint32_t> ( & __krealend ) - reinterpret_cast <uint32_t> ( & __krealbegin ), 0, true );
	Segmentation::GDT :: SetDataEntry16 ( 6, reinterpret_cast <uint32_t> ( & __krealbegin ), reinterpret_cast <uint32_t> ( & __krealend ) - reinterpret_cast <uint32_t> ( & __krealbegin ), 0, true );
	
	Segmentation::GDT :: Swap ();
	
#ifdef KSTARTUP_DEBUG
	system_func_kprintf ( "Memory manager initialized!\n\n" );
#endif
	
};

