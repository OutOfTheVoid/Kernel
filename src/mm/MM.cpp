#include <mm/MM.h>

#include <mm/paging/PageTable.h>
#include <mm/paging/PFA.h>
#include <mm/paging/AddressSpace.h>

#include <system/func/kprintf.h>

#include <math/IntMath.h>

#include <boot/bootimage.h>

void MM :: Init ( multiboot_info_t * MultibootInfo )
{
	
	Paging::PFA :: Init ( MultibootInfo );
	
	Paging::PageTable :: KInit ( Paging::PFA :: RetrieveMultibootInfoCopy () );
	Paging::PageTable :: EnableKPaging ();
	
	uint32_t KAddStart = math_max ( reinterpret_cast <uint32_t> ( & __kend ), KernelVMStart );
	
	Paging::AddressSpace :: KInit ( KAddStart, KernelVMLength - KAddStart );
	
};

