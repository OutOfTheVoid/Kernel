#include <mm/MM.h>

#include <mm/paging/PageTable.h>
#include <mm/paging/PFA.h>
#include <mm/paging/AddressSpace.h>
#include <mm/segmentation/GDT.h>
#include <mm/KMalloc.h>

#include <system/func/kprintf.h>

#include <math/IntMath.h>
#include <math/Bitmath.h>

#include <boot/bootimage.h>

#include <KernelDef.h>

extern "C" void hw_cpu_hang ();

void MM :: Init ( multiboot_info_t * MultibootInfo )
{
	
	uint32_t MBICopySize = MM::Paging::PFA :: CalcMBICopySize ( MultibootInfo );
	char MBICopySpace [ MBICopySize ];
	
	MM::Paging::PFA :: CopyMBI ( MultibootInfo, reinterpret_cast <multiboot_info_t *> ( MBICopySpace ) );
	
	Paging::PageTable :: KInit ();
	Paging::PageTable :: EnableKPaging ();
	
	volatile uint32_t KVStart = math_max ( reinterpret_cast <uint32_t> ( & __kend ), KernelVMStart );
	
	Paging::PFA :: Init ( MultibootInfo, MBICopySize, const_cast <uint32_t *> ( & KVStart ) );
	
	Paging::AddressSpace :: KInit ( KVStart, KernelVMLength - KVStart );
	
	Segmentation::GDT :: Init ( 5 );
	
	Segmentation::GDT :: SetDataEntry32 ( 3, 0, 0xFFFFFFFF, 3, true );
	Segmentation::GDT :: SetDataEntry32 ( 4, 0, 0xFFFFFFFF, 3, true );
	
	Segmentation::GDT :: Swap ();
	
	
};

