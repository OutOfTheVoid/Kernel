#include <mm/paging/PFA.h>

#include <system/func/KPrintf.h>
#include <system/func/Panic.h>

#include <util/string/string.h>

#include <hw/apm/APMHeader.h>

#include <hw/video/VText.h>

#include <math/Bitmath.h>
#include <boot/Bootimage.h>

multiboot_info_t * MM::Paging::PFA :: MBICopy;
size_t MM::Paging::PFA :: MBICopySize;

size_t MM::Paging::PFA :: TableSize;
MM::Paging::PFA :: Entry * MM::Paging::PFA :: Table;
uint32_t MM::Paging::PFA :: TopLevel;
uint32_t MM::Paging::PFA :: FreeCount;

void MM::Paging::PFA :: Init ( multiboot_info_t * MultibootInfo )
{
	
	// Make sure we have a multiboot information structure before continuing.
	if ( MultibootInfo == NULL )
		KPANIC ( "Multiboot information structure not supplied!" );
	
	Table = NULL;
	TableSize = 0;
	
	uint32_t MaxAddress;
	
	// Analyze the multiboot info structure and modify "free" memory listings so as to preserve kernel code and multiboot info
	MBEarlyAnalyze ( MultibootInfo, & MaxAddress );
	
	// Figure out how large to make the temporary buffer in stack space. 
	// This holds MultibootInfo while we look for a place to put it, as
	// it's currently in some arbitrary location that could get in the way
	// of the page frame tree.
	MBICopySize = CalcMBICopySize ( MultibootInfo );
	
	// Reserve stack space
	char MultibootInfoCopy [ MBICopySize ];
	MBICopy = reinterpret_cast <multiboot_info_t *> ( MultibootInfoCopy );
	
	// Copy the multiboot information structure
	CopyMBI ( MultibootInfo, MBICopy );
	
	// Calculate how many page frame records we will need in our tree
	
	// TopLevel: the maximum "level" in the page tree. Level 0 refers to 4KB records, increasing by powers of two from there.
	TopLevel = 32 - math_bitmath_clz ( math_bitmath_nextPowerOfTwo ( MaxAddress ) );
	
	if ( TopLevel == 0 )
		TopLevel = 20;
	else
		TopLevel -= 12;
	
	// The amound of entries in our tree
	uint32_t TableEntryCount = ( 2 << TopLevel ) - 1;
	
	// Search the memory map for an appropriate storage zone for the page frame tree.
	multiboot_memory_map_t * MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( MBICopy -> mmap_addr );
	
	while ( reinterpret_cast <uint64_t> ( MMapEntry ) < MBICopy -> mmap_addr + MBICopy -> mmap_length )
	{
		
		if ( MMapEntry -> type == MULTIBOOT_MEMORY_AVAILABLE )
		{
			
			if ( MMapEntry -> len >= TableEntryCount * sizeof ( Entry ) )
			{
				
				Table = reinterpret_cast <Entry *> ( MMapEntry -> addr );
				TableSize = TableEntryCount;
				
				MMapEntry -> addr += TableEntryCount * sizeof ( Entry );
				
				break;
				
			}
			
		}
		
		MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( reinterpret_cast <uint32_t> ( MMapEntry ) + MMapEntry -> size + 4 );
		
	}
	
	if ( TableSize == 0 )
		KPANIC ( "Unable to find suitable memory region for PFA tree!\n" );
	
	// Look for an appopriate place to store the MBI structure and put it there.
	MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( MBICopy -> mmap_addr );
	bool MBIRelocated = false;
	
	while ( reinterpret_cast <uint64_t> ( MMapEntry ) < MBICopy -> mmap_addr + MBICopy -> mmap_length )
	{
		
		if ( MMapEntry -> type == MULTIBOOT_MEMORY_AVAILABLE )
		{
			
			if ( MMapEntry -> len >= MBICopySize )
			{
				
				multiboot_info_t * NewMBI = reinterpret_cast <multiboot_info_t *> ( MMapEntry -> addr );
				
				CopyMBI ( MBICopy, NewMBI );
				MBICopy = NewMBI;
				
				MMapEntry -> addr += MBICopySize;
				
				MBIRelocated = true;
				
				break;
				
			}
			
		}
		
		MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( reinterpret_cast <uint32_t> ( MMapEntry ) + MMapEntry -> size + 4 );
		
	}
	
	if ( ! MBIRelocated )
		KPANIC ( "Could not find a location to relocate Multiboot info to!" );
	
	uint32_t e;
	
	// Fill our table with default values. All but the bottom level are split.
	for ( e = 0; e < __CalcTreeIndex ( 0, 0 ); e ++ )
	{
		
		Table [ e ].AFree = 0;
		Table [ e ].BFree = 0;
		Table [ e ].Split = 1;
		Table [ e ].Usable = 0;
		
	}
	
	for ( e = __CalcTreeIndex ( 0, 0 ); e < TableSize; e ++ )
	{
		
		Table [ e ].AFree = 0;
		Table [ e ].BFree = 0;
		Table [ e ].Split = 0;
		Table [ e ].Usable = 0;
		
	}
	
	// Fill out our page tree and count free space
	FreeCount = 0;
	
	for ( e = 0; e < ( MaxAddress & 0xFFFFF000 ); e += ( e + 4096 == 0 ) ? 1 : 4096 )
	{
		
		uint32_t Index = __CalcTreeIndex ( e, 0 );
		
		if ( TestRegionUsable ( e, 4096 ) )
		{
			
			Table [ Index ].AFree = 1;
			Table [ Index ].BFree = 1;
			Table [ Index ].Split = 0;
			Table [ Index ].Usable = 1;
			
			FreeCount ++;
			
		}
		
		__CascadeChangesUp ( __CalcParentIndex ( Index ) );
		
	}
	
};

multiboot_info_t * MM::Paging::PFA :: RetrieveMultibootInfoCopy ()
{
	
	return MBICopy;
	
};

bool MM::Paging::PFA :: Alloc ( uint32_t Length, void ** Address )
{
	
	if ( Length < 4096 )
		Length = 4096;
	
	uint32_t Level = 19 - math_bitmath_clz ( math_bitmath_nextPowerOfTwo ( Length ) );
	
	uint32_t Index = __FindFree ( 0, Level, TopLevel );
	
	if ( Index == 0xFFFFFFFF )
	{
		
		* Address = reinterpret_cast <void *> ( 0xFFFFFFFF );
		
		return false;
		
	}
	
	Table [ Index ].Split = 0;
	Table [ Index ].AFree = 0;
	Table [ Index ].BFree = 0;
	
	FreeCount -= 1 << Level;
	
	if ( Level != TopLevel )
		__CascadeChangesUp ( __CalcParentIndex ( Index ) );
	
	* Address = reinterpret_cast <void *> ( ( 4096 << Level ) * ( Index - ( ( 1 << ( TopLevel - Level ) ) - 1 ) ) );
	
	return true;
	
};

void MM::Paging::PFA :: Free ( void * Address )
{
	
	uint32_t FoundLevel = 0;
	uint32_t Index = __FindAllocated ( reinterpret_cast <uint32_t> ( Address ), TopLevel, & FoundLevel );
	
	if ( Index == 0xFFFFFFFF )
		return;
	
	Table [ Index ].AFree = 1;
	Table [ Index ].BFree = 1;
	Table [ Index ].Split = 0;
	
	FreeCount += 1 << FoundLevel;
	
	__CascadeChangesUp ( __CalcParentIndex ( Index ) );
	
};

uint32_t MM::Paging::PFA :: GetAllocationSize ( void * Address )
{
	
	uint32_t FoundLevel = 0;
	
	if ( __FindAllocated ( reinterpret_cast <uint32_t> ( Address ), TopLevel, & FoundLevel ) == 0xFFFFFFFF )
		return 0;
	
	return 0x1000 << FoundLevel;
	
};

uint32_t MM::Paging::PFA :: GetFreeKB ()
{
	
	return FreeCount * 4;
	
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

void MM::Paging::PFA :: MBEarlyAnalyze ( multiboot_info_t * MultibootInfo, uint32_t * MaxAddress )
{
	
	if ( ( MultibootInfo -> flags & MULTIBOOT_INFO_MEM_MAP ) == 0 )
		KPANIC ( "Multiboot memory map not supplied!" );
	
	* MaxAddress = 0;
	
	multiboot_memory_map_t * MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( MultibootInfo -> mmap_addr );
	
	if ( MultibootInfo -> mmap_length == 0 )
		KPANIC ( "Multiboot memory map is null!" );
	
	while ( reinterpret_cast <uint64_t> ( MMapEntry ) < MultibootInfo -> mmap_addr + MultibootInfo -> mmap_length )
	{
		
		if ( MMapEntry -> type == MULTIBOOT_MEMORY_AVAILABLE )
		{
			
			if ( ( MMapEntry -> addr <= reinterpret_cast <uint32_t> ( & __kbegin ) ) && ( MMapEntry -> addr + MMapEntry -> len > reinterpret_cast <uint32_t> ( & __kbegin ) ) )
				MMapEntry -> addr = reinterpret_cast <uint32_t> ( & __kend );
			
			if ( MultibootInfo -> flags & MULTIBOOT_INFO_MODS )
			{
				
				multiboot_module_t * ModuleEntry = reinterpret_cast <multiboot_module_t *> ( MultibootInfo -> mods_addr );
				
				for ( uint32_t i = 0; i < MultibootInfo -> mods_count; i ++ )
				{
					
					if ( ( MMapEntry -> addr <= ModuleEntry [ i ].mod_start ) && ( MMapEntry -> addr + MMapEntry -> len > ModuleEntry [ i ].mod_start ) )
						MMapEntry -> addr = ModuleEntry [ i ].mod_end;
					
				}
				
			}
			
		}
		
		if ( * MaxAddress < MMapEntry -> addr + MMapEntry -> len )
			* MaxAddress = ( MMapEntry -> addr + MMapEntry -> len > 0xFFFFFFFF ) ? 0xFFFFFFFF : MMapEntry -> addr + MMapEntry -> len;
		
		MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( reinterpret_cast <uint32_t> ( MMapEntry ) + MMapEntry -> size + 4 );
		
	};
	
};

bool MM::Paging::PFA :: TestRegionUsable ( uint32_t Offset, uint32_t Length )
{
	
	bool FoundFree = false;
	
	uint32_t FreeOffset = Offset;
	uint32_t FreeLength = Length;
	
	multiboot_memory_map_t * MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( MBICopy -> mmap_addr );
	
	while ( ( reinterpret_cast <uint64_t> ( MMapEntry ) < MBICopy -> mmap_addr + MBICopy -> mmap_length ) || ( ! FoundFree ) )
	{
		
		if ( reinterpret_cast <uint64_t> ( MMapEntry ) >= MBICopy -> mmap_addr + MBICopy -> mmap_length )
		{
			
			MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( MBICopy -> mmap_addr );
			
			if ( FreeOffset == Offset )
				return false;
			
		}
		
		if ( ( MMapEntry -> type == MULTIBOOT_MEMORY_AVAILABLE ) && ( MMapEntry -> addr < FreeOffset ) && ( MMapEntry -> addr + MMapEntry -> len > FreeOffset ) )
		{
			
			uint32_t OffsetShift = ( ( MMapEntry -> addr + MMapEntry -> len > FreeOffset + FreeLength ) ? FreeOffset + FreeLength : MMapEntry -> addr + MMapEntry -> len ) - FreeOffset;
			FreeOffset = OffsetShift;
			FreeLength = FreeLength - OffsetShift;
			
			if ( FreeLength == 0 )
				FoundFree = true;
			
		}
		
		if ( ( MMapEntry -> type != MULTIBOOT_MEMORY_AVAILABLE ) && ( MMapEntry -> addr < Offset + Length ) && ( MMapEntry -> addr + MMapEntry -> len >= Offset ) )
		{
			
			return false;
			
		}
		
		MMapEntry = reinterpret_cast <multiboot_memory_map_t *> ( reinterpret_cast <uint32_t> ( MMapEntry ) + MMapEntry -> size + 4 );
		
	}
	
	if ( MBICopy -> flags & MULTIBOOT_INFO_MODS )
	{
		
		multiboot_module_t * ModuleEntry = reinterpret_cast <multiboot_module_t *> ( MBICopy -> mods_addr );
		
		for ( uint32_t i = 0; i < MBICopy -> mods_count; i ++ )
		{
			
			if ( ( ModuleEntry [ i ].mod_start < Offset + Length ) && ( ModuleEntry [ i ].mod_end > Offset ) )
				return false;
			
		}
		
	}
	
	if ( ( Offset < reinterpret_cast <uint32_t> ( Table ) + sizeof ( Entry ) * TableSize ) && ( Offset + Length > reinterpret_cast <uint32_t> ( Table ) ) )
		return false;
	
	return true;
	
};
