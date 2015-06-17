#include <mm/paging/PFA.h>
#include <mm/paging/PageTable.h>

#include <system/func/KPrintf.h>
#include <system/func/Panic.h>

#include <util/string/string.h>

#include <cpputil/Unused.h>

#include <hw/apm/APMHeader.h>

#include <math/Bitmath.h>
#include <boot/Bootimage.h>

MM::Paging::PAlloc :: PageFreeZone * MM::Paging::PFA :: GeneralPhysicalFree;
MM::Paging::PAlloc :: PageAllocZone * MM::Paging::PFA :: KernelGeneralPhysicalAlloc;

void MM::Paging::PFA :: Init ( multiboot_info_t * MultibootInfo, uint32_t MultibootInfoSize, uint32_t * KVStart )
{
	
	UNUSED ( MultibootInfoSize );
	
	* KVStart += 0xFFF;
	* KVStart &= 0xFFFFF000;
	
	MBEarlyAnalyze ( MultibootInfo );
	
	// TODO: Build PMM
	
	uint32_t BaseAddr = 0;
	uint32_t Length = 0;
	
	multiboot_memory_map_t * MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( MultibootInfo -> mmap_addr );
	
	while ( reinterpret_cast <uint64_t> ( MMapEntry ) < MultibootInfo -> mmap_addr + MultibootInfo -> mmap_length )
	{
		
		if ( ( MMapEntry -> type == MULTIBOOT_MEMORY_AVAILABLE ) && ( MMapEntry -> len != 0 ) )
		{
			
			BaseAddr = ( static_cast <uint32_t> ( MMapEntry -> addr ) + 0xFFF ) & 0xFFFFF000;
			Length = static_cast <uint32_t> ( MMapEntry -> len ) - ( static_cast <uint32_t> ( MMapEntry -> addr ) - BaseAddr );
			
			if ( Length >= 0x2000 )
			{
				
				MMapEntry -> type = MULTIBOOT_MEMORY_RESERVED;
				
				break;
				
			}
			
		}
	
		MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( reinterpret_cast <uint32_t> ( MMapEntry ) + MMapEntry -> size + 4 );
		
	}
	
	if ( reinterpret_cast <uint64_t> ( MMapEntry ) >= MultibootInfo -> mmap_addr + MultibootInfo -> mmap_length )
		KPANIC ( "Unable to find free physical region greater than 4KB!" );
	
	uint32_t Error;
	
	PAlloc :: InitFreeZone ( & GeneralPhysicalFree, "General physical memory", BaseAddr, Length, * KVStart, & Error );
	* KVStart += 0x1000;
	
	if ( Error != PAlloc :: kError_None )
		KPANIC ( "PAlloc error creating kernel physical free zone: %s", PAlloc :: GetErrorString ( Error ) );
	
	KernelGeneralPhysicalAlloc = PAlloc :: MakePageAllocationZone ( "KPAlloc (PFA)", GeneralPhysicalFree, & Error );
	
	if ( Error != PAlloc :: kError_None )
		KPANIC ( "PAlloc error creating kernel physical alloc zone: %s", PAlloc :: GetErrorString ( Error ) );
	
};

bool MM::Paging::PFA :: Alloc ( uint32_t Length, void ** Address )
{
	
	uint32_t Error;
	
	PAlloc :: Alloc ( KernelGeneralPhysicalAlloc, Address, Length, & Error );
	
	return ( Error == PAlloc :: kError_None );
	
};

void MM::Paging::PFA :: Free ( void * Address )
{
	
	uint32_t Error;
	
	PAlloc :: Free ( KernelGeneralPhysicalAlloc, Address, & Error );
	
	if ( Error != PAlloc :: kError_None )
		system_func_kprintf ( "WARNING: PAlloc: %s\n", PAlloc :: GetErrorString ( Error ) );
	
};

uint32_t MM::Paging::PFA :: GetAllocationSize ( void * Address )
{
	
	return PAlloc :: GetAllocationSize ( KernelGeneralPhysicalAlloc, Address );
	
};

uint32_t MM::Paging::PFA :: GetFreeKB ()
{
	
	return PAlloc :: GetFreePages ( GeneralPhysicalFree ) << 2;
	
};

void MM::Paging::PFA :: CopyMBI ( multiboot_info_t * Old, multiboot_info_t * New )
{
	
	size_t MultibootInfoSize;
	
	memcpy ( New, Old, sizeof ( multiboot_info_t ) );
	MultibootInfoSize = sizeof ( multiboot_info_t );
	
	New -> mmap_addr = reinterpret_cast <uint32_t> ( New ) + MultibootInfoSize;
	
	memcpy ( reinterpret_cast <void *> ( New -> mmap_addr ), reinterpret_cast <void *> ( Old -> mmap_addr ), Old -> mmap_length );
	MultibootInfoSize += static_cast <size_t> ( Old -> mmap_length );
	
	if ( Old -> flags & MULTIBOOT_INFO_MODS )
	{
		
		New -> mods_addr = reinterpret_cast <uint32_t> ( New ) + MultibootInfoSize;
		
		memcpy ( reinterpret_cast <void *> ( New -> mods_addr ), reinterpret_cast <void *> ( Old -> mods_addr ), Old -> mods_count * sizeof ( multiboot_module_t ) );
		MultibootInfoSize += static_cast <size_t> ( Old -> mods_count * sizeof ( multiboot_module_t ) );
		
	}
	
	if ( Old -> flags & MULTIBOOT_INFO_ELF_SHDR )
	{
		
		New -> u.elf_sec.addr = reinterpret_cast <uint32_t> ( New ) + MultibootInfoSize;
		
		memcpy ( reinterpret_cast <void *> ( New -> u.elf_sec.addr ), reinterpret_cast <void *> ( Old -> u.elf_sec.addr ), Old -> u.elf_sec.num * Old -> u.elf_sec.size );
		MultibootInfoSize += static_cast <size_t> ( Old -> u.elf_sec.num * Old -> u.elf_sec.size );
		
	}
	
	if ( Old -> flags & MULTIBOOT_INFO_DRIVE_INFO )
	{
		
		New -> drives_addr = reinterpret_cast <uint32_t> ( New ) + MultibootInfoSize;
		
		memcpy ( reinterpret_cast <void *> ( New -> drives_addr ), reinterpret_cast <void *> ( Old -> drives_addr ), Old -> drives_length );
		MultibootInfoSize += static_cast <size_t> ( Old -> drives_length );
		
	}
	
	if ( Old -> flags & MULTIBOOT_INFO_CONFIG_TABLE )
	{
		
		New -> config_table = reinterpret_cast <uint32_t> ( New ) + MultibootInfoSize;
		
		memcpy ( reinterpret_cast <void *> ( New -> config_table ), reinterpret_cast <void *> ( Old -> config_table ), * reinterpret_cast <uint16_t *> ( Old -> config_table ) );
		MultibootInfoSize += static_cast <size_t> ( * reinterpret_cast <uint16_t *> ( Old -> config_table ) );
		
	}
	
	if ( Old -> flags & MULTIBOOT_INFO_BOOT_LOADER_NAME )
	{
		
		New -> boot_loader_name = reinterpret_cast <uint32_t> ( New ) + MultibootInfoSize;
		
		memcpy ( reinterpret_cast <void *> ( New -> boot_loader_name ), reinterpret_cast <void *> ( Old -> boot_loader_name ), strlen ( reinterpret_cast <const char *> ( Old -> boot_loader_name ) ) + 1 );
		MultibootInfoSize += static_cast <size_t> ( strlen ( reinterpret_cast <const char *> ( Old -> boot_loader_name ) ) + 1 );

		
	}
	
	if ( Old -> flags & MULTIBOOT_INFO_APM_TABLE )
	{
		
		New -> apm_table = reinterpret_cast <uint32_t> ( New ) + MultibootInfoSize;
		
		memcpy ( reinterpret_cast <void *> ( New -> apm_table ), reinterpret_cast <void *> ( Old -> apm_table ), sizeof ( APMHeader ) );
		MultibootInfoSize += sizeof ( APMHeader );
		
	}
	
};

size_t MM::Paging::PFA :: CalcMBICopySize ( multiboot_info_t * MultibootInfo )
{
	
	size_t MultibootInfoSize = sizeof ( multiboot_info_t );
	MultibootInfoSize += static_cast <size_t> ( MultibootInfo -> mmap_length );
	
	if ( MultibootInfo -> flags & MULTIBOOT_INFO_MODS )
		MultibootInfoSize += static_cast <size_t> ( MultibootInfo -> mods_count * sizeof ( multiboot_module_t ) );
	
	if ( MultibootInfo -> flags & MULTIBOOT_INFO_ELF_SHDR )
		MultibootInfoSize += static_cast <size_t> ( MultibootInfo -> u.elf_sec.num * MultibootInfo -> u.elf_sec.size );
	
	if ( MultibootInfo -> flags & MULTIBOOT_INFO_DRIVE_INFO )
		MultibootInfoSize += static_cast <size_t> ( MultibootInfo -> drives_length );
	
	if ( MultibootInfo -> flags & MULTIBOOT_INFO_CONFIG_TABLE )
		MultibootInfoSize += static_cast <size_t> ( * reinterpret_cast <uint16_t *> ( MultibootInfo -> config_table ) );
	
	if ( MultibootInfo -> flags & MULTIBOOT_INFO_BOOT_LOADER_NAME )
		MultibootInfoSize += static_cast <size_t> ( strlen ( reinterpret_cast <const char *> ( MultibootInfo -> boot_loader_name ) ) + 1 );
	
	if ( MultibootInfo -> flags & MULTIBOOT_INFO_APM_TABLE )
		MultibootInfoSize += sizeof ( APMHeader );
	
	return MultibootInfoSize;
	
};

void MM::Paging::PFA :: MBEarlyAnalyze ( multiboot_info_t * MultibootInfo )
{
	
	if ( ( MultibootInfo -> flags & MULTIBOOT_INFO_MEM_MAP ) == 0 )
		KPANIC ( "Multiboot memory map not supplied!" );
	
	multiboot_memory_map_t * MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( MultibootInfo -> mmap_addr );
	
	if ( MultibootInfo -> mmap_length == 0 )
		KPANIC ( "Multiboot memory map is null!" );
	
	while ( reinterpret_cast <uint64_t> ( MMapEntry ) < MultibootInfo -> mmap_addr + MultibootInfo -> mmap_length )
	{
		
		if ( MMapEntry -> type == MULTIBOOT_MEMORY_AVAILABLE )
		{
			
			if ( MMapEntry -> addr > 0xFFFFFFFF )
				MMapEntry -> len = 0;
			else if ( MMapEntry -> addr + MMapEntry -> len > 0xFFFFFFFF )
				MMapEntry -> len = 0x100000000 - ( MMapEntry -> addr + MMapEntry -> len );
			
			if ( ( MMapEntry -> addr <= reinterpret_cast <uint32_t> ( & __kbegin ) ) && ( MMapEntry -> addr + MMapEntry -> len > reinterpret_cast <uint32_t> ( & __kbegin ) ) )
			{
				
				MMapEntry -> len -= reinterpret_cast <uint32_t> ( & __kend ) - MMapEntry -> addr;
				MMapEntry -> addr = reinterpret_cast <uint32_t> ( & __kend );
				
			}
			
			if ( MultibootInfo -> flags & MULTIBOOT_INFO_MODS )
			{
				
				multiboot_module_t * ModuleEntry = reinterpret_cast <multiboot_module_t *> ( MultibootInfo -> mods_addr );
				
				for ( uint32_t i = 0; i < MultibootInfo -> mods_count; i ++ )
				{
					
					if ( ( MMapEntry -> addr <= ModuleEntry [ i ].mod_start ) && ( MMapEntry -> addr + MMapEntry -> len > ModuleEntry [ i ].mod_start ) )
					{
						
						MMapEntry -> len -= ModuleEntry [ i ].mod_end - MMapEntry -> addr;
						MMapEntry -> addr = ModuleEntry [ i ].mod_end;
						
					}
					
				}
				
			}
			
		}
		
		MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( reinterpret_cast <uint32_t> ( MMapEntry ) + MMapEntry -> size + 4 );
		
	};
	
};
