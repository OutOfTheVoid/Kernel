#include <mm/paging/PageTable.h>
#include <mm/paging/PFA.h>
#include <mm/KVMap.h>
#include <mm/MM.h>

#include <cpputil/linkage.h>

#include <system/func/panic.h>
#include <system/func/kprintf.h>

#include <boot/multiboot/multiboot.h>
#include <boot/bootimage.h>

C_LINKAGE MM::Paging::PageTable :: PDirectory Kernel_PDirectory;

MM::Paging::PageTable :: PDirectory Kernel_PDirectory = 0;

C_LINKAGE uint32_t mm_paging_kernelpt;
C_LINKAGE uint32_t mm_paging_kernelpd;

ASM_LINKAGE void mm_paging_loadPageDirectory ( MM::Paging::PageTable :: PDirectory );
ASM_LINKAGE void mm_paging_enable ();
ASM_LINKAGE void mm_paging_disable ();
ASM_LINKAGE void mm_paging_flushCR3 ();
ASM_LINKAGE void mm_paging_invalPage ( uint32_t Virtual );

ASM_LINKAGE uint32_t mm_paging_getStatus ();

void MM::Paging::PageTable :: KInit ()
{
	
	Kernel_PDirectory = reinterpret_cast <PDirectory> ( & mm_paging_kernelpd );
	
	uint32_t I;
	
	for ( I = 0; I < 1024; I ++ )
	{
		
		uint32_t * DirectoryEntry = & Kernel_PDirectory [ I ];
		SetDirectoryEntry ( DirectoryEntry, reinterpret_cast <uint32_t> ( & mm_paging_kernelpt ) + 0x1000 * I, DFlags_Present | DFlags_User | DFlags_Writeable );
		
		for ( uint32_t P = 0; P < 1024; P ++ )
			SetKernelMapping ( I * 0x400000 + P * 0x1000, I * 0x400000 + P * 0x1000, Flags_Writeable );
	
	}
	
	for ( I = 0; I < reinterpret_cast <uint32_t> ( & __kend ); I += 0x1000 )
		SetKernelMapping ( I, I, Flags_Present | Flags_Writeable );
	
};

void MM::Paging::PageTable :: Load ()
{
	
	mm_paging_loadPageDirectory ( DirectoryPhysical );
	mm_paging_flushCR3 ();
	
};

MM::Paging::PageTable :: PDirectory MM::Paging::PageTable :: GetKernelPD ()
{
	
	return Kernel_PDirectory;
	
};

MM::Paging::PageTable :: PageTable ( bool MapKernel )
{
	
	uint32_t * NewDirectoryPhysical;
	uint32_t * NewDirectoryVirtual;
	
	if ( ! PFA :: Alloc ( 0x1000, reinterpret_cast <void **> ( & NewDirectoryPhysical ) ) )
		KPANIC ( "Failed to allocate new page table!" );
	
	NewDirectoryVirtual = reinterpret_cast <uint32_t *> ( mm_kvmap ( NewDirectoryPhysical, 0x1000, Flags_Writeable ) );
	
	if ( NewDirectoryVirtual == NULL )
		KPANIC ( "Failed to allocate new page table!" );
	
	 uint32_t I;
	
	for ( I = 0; I < 1024; I ++ )
		SetDirectoryEntry ( reinterpret_cast <uint32_t *> ( & ( NewDirectoryVirtual [ I ] ) ), 0, 0 );
	
	if ( MapKernel )
	{
		
		 for ( I = 0; I < kKernelVM_Length; I += 0x400000 )
		 	NewDirectoryVirtual [ I >> 22 ] = Kernel_PDirectory [ I >> 22 ];
		
	}
	
	DirectoryPhysical = NewDirectoryPhysical;
	DirectoryVirtual = NewDirectoryVirtual;
	
	RefCount = 0;
	
	Lock = MT::Synchronization::Spinlock :: Initializer ();
	
};

MM::Paging::PageTable :: ~PageTable ()
{
	
	uint32_t I;
	
	for ( I = 0; I < 1024; I ++ )
	{
		
		if ( DirectoryVirtual [ I ] != Kernel_PDirectory [ I ] )
		{
			
			uint32_t * TablePTR = reinterpret_cast <uint32_t *> ( DirectoryVirtual [ I ] & 0xFFFFF000 );
			
			if ( TablePTR != NULL )
				PFA :: Free ( reinterpret_cast <void *> ( TablePTR ) );
			
		}
		
	}
	
	mm_kvunmap ( DirectoryVirtual );
	
	PFA :: Free ( reinterpret_cast <void *> ( DirectoryPhysical ) );
	
};

void MM::Paging::PageTable :: IncrementRef ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	RefCount ++;
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

void MM::Paging::PageTable :: DecrementRef ()
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	RefCount --;
	
	if ( RefCount == 0 )
	{
	
		delete this;
		
		return;
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

void MM::Paging::PageTable :: SetMapping ( uint32_t Virtual, uint32_t Physical, uint32_t Flags )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	uint32_t DirectoryEntry = DirectoryVirtual [ Virtual >> 22 ];
	
	if ( ( DirectoryEntry & DFlags_Present ) == 0 )
	{
		
		if ( ! PFA :: Alloc ( 0x1000, reinterpret_cast <void **> ( & DirectoryVirtual [ Virtual >> 22 ] ) ) )
			KPANIC ( "Failed to allocate new directory entry!" );
		
		DirectoryEntry = DirectoryVirtual [ Virtual >> 22 ];
		
	}
	
	SetKernelMapping ( reinterpret_cast <uint32_t> ( DirectoryVirtual ), reinterpret_cast <uint32_t> ( DirectoryEntry ) & 0xFFFFF000, Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
	
	DirectoryVirtual [ ( Virtual >> 12 ) & 0x3FF ] = ( Physical & 0xFFFFF000 ) | ( Flags & 0xFFF );
	
	SetKernelMapping ( reinterpret_cast <uint32_t> ( DirectoryVirtual ), reinterpret_cast <uint32_t> ( DirectoryPhysical ), Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

void MM::Paging::PageTable :: ClearMapping ( uint32_t Virtual )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	uint32_t DirectoryEntry = DirectoryVirtual [ Virtual >> 22 ];
	
	if ( ( DirectoryEntry & DFlags_Present ) == 0 )
	{
		
		MT::Synchronization::Spinlock :: Release ( & Lock );
		
		return;
		
	}
	
	SetKernelMapping ( reinterpret_cast <uint32_t> ( DirectoryVirtual ), DirectoryEntry & 0xFFFFF000, Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
	
	DirectoryVirtual [ ( Virtual >> 12 ) & 0x3FF ] = 0;
	
	SetKernelMapping ( reinterpret_cast <uint32_t> ( DirectoryVirtual ), reinterpret_cast <uint32_t> ( DirectoryPhysical ), Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

void MM::Paging::PageTable :: SetRegionMapping ( uint32_t Virtual, uint32_t Physical, uint32_t Length, uint32_t Flags )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	for ( uint32_t Offset = 0; Offset < Length; Offset += 0x1000 )
	{
		
		uint32_t DirectoryEntry = DirectoryVirtual [ ( Virtual + Offset ) >> 22 ];
		
		if ( ( DirectoryEntry & DFlags_Present ) == 0 )
		{
			
			if ( ! PFA :: Alloc ( 0x1000, reinterpret_cast <void **> ( & DirectoryVirtual [ ( Virtual + Offset ) >> 22 ] ) ) )
				KPANIC ( "Failed to allocate new directory entry!" );
			
			DirectoryEntry = DirectoryVirtual [ ( Virtual + Offset ) >> 22 ];
			
		}
		
		SetKernelMapping ( reinterpret_cast <uint32_t> ( DirectoryVirtual ), DirectoryEntry & 0xFFFFF000, Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
		
		DirectoryVirtual [ ( ( Virtual + Offset ) >> 12 ) & 0x3FF ] = ( ( Physical + Offset ) & 0xFFFFF000 ) | ( Flags & 0xFFF );
	
		SetKernelMapping ( reinterpret_cast <uint32_t> ( DirectoryVirtual ), reinterpret_cast <uint32_t> ( DirectoryPhysical ), Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

void MM::Paging::PageTable :: ClearRegionMapping ( uint32_t Virtual, uint32_t Length )
{
	
	MT::Synchronization::Spinlock :: SpinAcquire ( & Lock );
	
	for ( uint32_t Offset = 0; Offset < Length; Offset += 0x1000 )
	{
		
		uint32_t DirectoryEntry = DirectoryVirtual [ ( Virtual + Offset ) >> 22 ];
		
		if ( ( DirectoryEntry & DFlags_Present ) == 0 )
		{
			
			if ( ! PFA :: Alloc ( 0x1000, reinterpret_cast <void **> ( & DirectoryVirtual [ ( Virtual + Offset ) >> 22 ] ) ) )
				KPANIC ( "Failed to allocate new directory entry!" );
			
			DirectoryEntry = DirectoryVirtual [ ( Virtual + Offset ) >> 22 ];
			
		}
		
		SetKernelMapping ( reinterpret_cast <uint32_t> ( DirectoryVirtual ), DirectoryEntry & 0xFFFFF000, Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
		
		DirectoryVirtual [ ( ( Virtual + Offset ) >> 12 ) & 0x3FF ] = 0;
	
		SetKernelMapping ( reinterpret_cast <uint32_t> ( DirectoryVirtual ), reinterpret_cast <uint32_t> ( DirectoryPhysical ), Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
		
	}
	
	MT::Synchronization::Spinlock :: Release ( & Lock );
	
};

uint32_t MM::Paging::PageTable :: GetCR3 ()
{
	
	return reinterpret_cast <uint32_t> ( DirectoryPhysical );
	
};

void MM::Paging::PageTable :: SetKernelMapping ( uint32_t Virtual, uint32_t Physical, uint32_t Flags )
{
	
	uint32_t DirectoryEntry = Kernel_PDirectory [ Virtual >> 22 ];
	
	reinterpret_cast <uint32_t *> ( DirectoryEntry & 0xFFFFF000 ) [ ( Virtual >> 12 ) & 0x3FF ] = ( Physical & 0xFFFFF000 ) | ( Flags & 0xFFF );
	
	mm_paging_invalPage ( Virtual );
	
};

void MM::Paging::PageTable :: ClearKernelMapping ( uint32_t Virtual )
{
	
	uint32_t DirectoryEntry = Kernel_PDirectory [ Virtual >> 22 ];
	
	reinterpret_cast <uint32_t *> ( DirectoryEntry & 0xFFFFF000 ) [ ( Virtual >> 12 ) & 0x3FF ] = Flags_Writeable;
	
};

void MM::Paging::PageTable :: SetKernelRegionMapping ( uint32_t Virtual, uint32_t Physical, uint32_t Length, uint32_t Flags )
{
	
	if ( Length == 0 )
		return;
	
	if ( Length < 0x1000 )
		Length = 0x1000;
	
	for ( uint32_t i = 0; i < Length; i += 0x1000 )
		SetKernelMapping ( Virtual + i, ( Physical & 0xFFFFF000 ) + i, Flags );
	
};

void MM::Paging::PageTable :: ClearKernelRegionMapping ( uint32_t Virtual, uint32_t Length )
{
	
	if ( Length == 0 )
		return;
	
	if ( Length < 0x1000 )
		Length = 0x1000;
	
	for ( uint32_t i = 0; i < Length; i ++ )
		ClearKernelMapping ( Virtual + i );
	
};

uint32_t MM::Paging::PageTable :: KernelVirtualToPhysical ( uint32_t Virtual )
{
	
	uint32_t DirectoryEntry = Kernel_PDirectory [ Virtual >> 22 ];
	
	uint32_t TableEntry = reinterpret_cast <uint32_t *> ( DirectoryEntry & 0xFFFFF000 ) [ ( Virtual >> 12 ) & 0x3FF ] & 0xFFFFF000;
	
	return TableEntry | ( Virtual & 0xFFF );
	
};

void MM::Paging::PageTable :: SetDirectoryEntry ( uint32_t * DirectoryEntry, uint32_t TableAddress, uint32_t Flags )
{
	
	* DirectoryEntry = ( TableAddress & 0xFFFFF000 ) | ( Flags & 0xFFF );
	
};

void MM::Paging::PageTable :: EnableKPaging ()
{
	
	mm_paging_loadPageDirectory ( Kernel_PDirectory );
	mm_paging_enable ();
	
};

void MM::Paging::PageTable :: KLoad ()
{
	
	mm_paging_loadPageDirectory ( Kernel_PDirectory );
	mm_paging_flushCR3 ();
	
};

void MM::Paging::PageTable :: DisableKPaging ()
{
	
	mm_paging_disable ();
	
};

void MM::Paging::PageTable :: FlushKPDirectory ()
{
	
	mm_paging_flushCR3 ();
	
};

bool MM::Paging::PageTable :: KPagingStatus ()
{
	
	return mm_paging_getStatus ();
	
};

void MM::Paging::PageTable :: FlushEntry ( uint32_t Virtual )
{
	
	mm_paging_invalPage ( Virtual );
	
};

bool MM::Paging::PageTable :: ShouldFault ( uint32_t Address, bool Write, bool User )
{
	
	( void ) Address;
	( void ) Write;
	( void ) User;
	
	return true;
	
};
