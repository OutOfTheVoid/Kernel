#include <fs/initrd/InitRamDisk.h>

#include <fs/mountfs/FileSystem.h>
#include <fs/vfs/FileSystem.h>

#include <hw/storage/RamDiskStorageDevice.h>

#include <system/func/Panic.h>

#include <util/string/string.h>

void FS::InitRD::InitRamDisk :: Init ( multiboot_info_t * MultibootInfo )
{
	
	uint32_t Length;
	void * ModulePhysical = NULL;
	void * ModuleVirtual = NULL;
	
	for ( uint32_t I = 0; I < MultibootInfo -> mods_count; I ++ )
	{
		
		multiboot_module_t * ModuleListing = & reinterpret_cast <multiboot_module_t *> ( MultibootInfo -> mods_addr ) [ I ];
		
		if ( strcspn ( reinterpret_cast <const char *> ( ModuleListing -> cmdline ), "initrd" ) != strlen ( reinterpret_cast <const char *> ( ModuleListing -> cmdline ) ) )
		{
			
			Length = ModuleListing -> mod_end - ModuleListing -> mod_end;
			ModulePhysical = reinterpret_cast <void *> ( ModuleListing -> mod_start );
			
			break;
			
		}
		
	}
	
	if ( ModulePhysical == reinterpret_cast <void *> ( 0xFFFFFFFF ) )
		KPANIC ( "No initrd module found!" );
	
	( void ) Length;
	( void ) ModuleVirtual;
	
};
