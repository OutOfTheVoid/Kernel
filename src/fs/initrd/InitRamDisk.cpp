#include <fs/initrd/InitRamDisk.h>

#include <fs/mountfs/FileSystem.h>
#include <fs/vfs/FileSystem.h>

#include <system/func/Panic.h>

#include <system/func/KPrintF.h>

#include <util/string/string.h>

#include <mm/KVMap.h>

HW::Storage::RamDiskStorageDevice * FS::InitRD::InitRamDisk :: Device = NULL;

void * FS::InitRD::InitRamDisk :: ModulePhysical = NULL;
void * FS::InitRD::InitRamDisk :: ModuleVirtual = NULL;
uint32_t FS::InitRD::InitRamDisk :: Length = 0;

void FS::InitRD::InitRamDisk :: Init ( multiboot_info_t * MultibootInfo )
{
	
	for ( uint32_t I = 0; I < MultibootInfo -> mods_count; I ++ )
	{
		
		multiboot_module_t * ModuleListing = & reinterpret_cast <multiboot_module_t *> ( MultibootInfo -> mods_addr ) [ I ];
		
		if ( strcspn ( reinterpret_cast <const char *> ( ModuleListing -> cmdline ), "initrd" ) != strlen ( reinterpret_cast <const char *> ( ModuleListing -> cmdline ) ) )
		{
			
			Length = ( ModuleListing -> mod_end ) - ( ModuleListing -> mod_end );
			system_func_kprintf ( "Module length: %u\n", Length );
			ModulePhysical = reinterpret_cast <void *> ( ModuleListing -> mod_start );
			
			break;
			
		}
		
	}
	
	if ( ModulePhysical == reinterpret_cast <void *> ( 0xFFFFFFFF ) )
		KPANIC ( "No initrd module found!" );
	
	ModuleVirtual = reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( mm_kvmap ( reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( ModulePhysical ) & 0xFFFFF000 ), ( Length + 0x1000 ) & 0xFFFFF000, 0 ) ) | ( reinterpret_cast <uint32_t> ( ModulePhysical ) & 0x00000FFF ) );
	
	if ( reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( ModuleVirtual ) & 0xFFFFF000 ) == NULL )
		KPANIC ( "Failed to vmap initrd!" );
	
	Device = new HW::Storage :: RamDiskStorageDevice ( "InitRD", ModuleVirtual, Length, HW::Storage::RamDiskStorageDevice :: kFlag_Read );
	
	MountFS::FileSystem :: MountStatus_t Status;
	MountFS::FileSystem :: MountDevice ( Device, & Status );
	
	//if ( Status != MountFS::FileSystem :: kMountStatus_Success )
	//	KPANIC ( "Failed to mount InitRD: %i", Status );
	
};
