#include <mm/paging/PageTable.h>
#include <mm/paging/PFA.h>

#include <cpputil/linkage.h>

#include <system/func/panic.h>
#include <system/func/kprintf.h>

#include <boot/multiboot/multiboot.h>
#include <boot/bootimage.h>

C_LINKAGE MM::Paging::PageTable :: PDirectory Kernel_PDirectory;

MM::Paging::PageTable :: PDirectory Kernel_PDirectory = 0;

C_LINKAGE uint32_t mm_paging_kernelpt;
C_LINKAGE uint32_t mm_paging_kernelpd;

C_LINKAGE void mm_paging_loadPageDirectory ( MM::Paging::PageTable :: PDirectory );
C_LINKAGE void mm_paging_enable ();
C_LINKAGE void mm_paging_disable ();
C_LINKAGE void mm_paging_flushCR3 ();
C_LINKAGE void mm_paging_invalPage ( uint32_t Virtual );

void MM::Paging::PageTable :: KInit ( multiboot_info_t * MultibootInfo )
{
	
	Kernel_PDirectory = reinterpret_cast <PDirectory> ( & mm_paging_kernelpd );
	
	// Set up initial PDE's
	uint32_t i;
	
	for ( i = 0; i < 1024; i ++ )
	{
		
		uint32_t * DirectoryEntry = & Kernel_PDirectory [ i ];
		SetDirectoryEntry ( DirectoryEntry, reinterpret_cast <uint32_t> ( & mm_paging_kernelpt ) + 0x1000 * i, DFlags_Present | DFlags_Writeable );
		
		for ( uint32_t p = 0; p < 1024; p ++ )
			SetKernelMapping ( i * 0x400000 + p * 0x1000, i * 0x400000 + p * 0x1000, Flags_Writeable );
	
	}
	
	// Identitiy map kernel && loaded modules
	for ( i = 0; i < reinterpret_cast <uint32_t> ( & __kend ); i += 0x1000 )
		SetKernelMapping ( i, i, Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
	
	//for ( i = reinterpret_cast <uint32_t> ( MM::Paging::PFA :: Table ); i < reinterpret_cast <uint32_t> ( MM::Paging::PFA :: Table ) + MM::Paging::PFA :: TableSize * sizeof ( MM::Paging::PFA :: Entry ); i += 0x1000 )
	//	SetKernelMapping ( i, i, Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
	
	if ( MultibootInfo -> flags & MULTIBOOT_INFO_MODS )
	{
		
		multiboot_module_t * Modules = reinterpret_cast <multiboot_module_t *> ( MultibootInfo -> mods_addr );
		
		for ( i = 0; i < MultibootInfo -> mods_count; i ++ )
			SetKernelRegionMapping ( Modules [ i ].mod_start & 0xFFFFF000, Modules [ i ].mod_start & 0xFFFFF000, Modules [ i ].mod_end - Modules [ i ].mod_start , Flags_Present | Flags_Writeable | Flags_Cutsom_KMap );
		
	}
	
};

MM::Paging::PageTable :: PDirectory MM::Paging::PageTable :: GetKernelPT ()
{
	
	return Kernel_PDirectory;
	
};

MM::Paging::PageTable :: PDirectory MM::Paging::PageTable :: Create ()
{
	
	MM::Paging::PageTable :: PDirectory NewDirectory;
	
	if ( ! MM::Paging::PFA :: Alloc ( 0x1000, reinterpret_cast <void **> ( & NewDirectory ) ) )
		KPANIC ( "Failed to allocate new page table!" );
	
	for ( uint32_t i = 0; i < 1024; i ++ )
		SetDirectoryEntry ( reinterpret_cast <uint32_t *> ( & ( NewDirectory [ i ] ) ), 0, DFlags_Writeable );
	
	return NewDirectory;
	
};

void MM::Paging::PageTable :: Destroy ( PDirectory Directory )
{
	
	for ( uint32_t i = 0; i < 1024; i ++ )
	{
		
		uint32_t DirectoryEntry = ( Directory ) [ i ];
		
		if ( ( DirectoryEntry & DFlags_Present ) != 0 )
			MM::Paging::PFA :: Free ( reinterpret_cast <void *> ( DirectoryEntry & 0xFFFFF000 ) );
		
	}
	
	MM::Paging::PFA :: Free ( reinterpret_cast <void *> ( Directory ) );
	
};

void MM::Paging::PageTable :: SetMapping ( PDirectory Directory, uint32_t Virtual, uint32_t Physical, uint32_t Flags )
{
	
	uint32_t * DirectoryEntry = & Directory [ Virtual >> 22 ];
	
	if ( ( * DirectoryEntry & DFlags_Present ) == 0 )
	{
		
		uint32_t * PTE;
		
		if ( ! MM::Paging::PFA :: Alloc ( 0x1000, reinterpret_cast <void **> ( & PTE ) ) )
			KPANIC ( "Failed to allocate paging directory entry!" );
		
		SetDirectoryEntry ( DirectoryEntry, reinterpret_cast <uint32_t> ( PTE ), DFlags_Present | DFlags_Writeable | ( Flags & Flags_User ) );
		
		for ( uint32_t i = 0; i < 1024; i ++ )
			PTE [ i ] = Flags_Writeable;
		
	}
	
	reinterpret_cast <uint32_t *> ( * DirectoryEntry & 0xFFFFF000 ) [ ( Virtual >> 12 ) & 0x3FF ] = ( Physical & 0xFFFFF000 ) | ( Flags & 0xFFF );
	
};

void MM::Paging::PageTable :: ClearMapping ( PDirectory Directory, uint32_t Virtual )
{
	
	uint32_t DirectoryEntry = Directory [ Virtual >> 22 ];
	
	if ( ( DirectoryEntry & DFlags_Present ) == 0 )
		return;
	
	reinterpret_cast <uint32_t *> ( DirectoryEntry & 0xFFFFF000 ) [ ( Virtual >> 12 ) & 0x3FF ] = Flags_Writeable;
	
	mm_paging_invalPage ( Virtual );
	
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
	
	for ( uint32_t i = 0; i < Length; i ++ )
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

void MM::Paging::PageTable :: DisableKPaging ()
{
	
	mm_paging_disable ();
	
};

void MM::Paging::PageTable :: FlushKPDirectory ()
{
	
	mm_paging_flushCR3 ();
	
};

