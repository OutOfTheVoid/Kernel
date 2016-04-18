#ifndef FS_INIT_INITRAMDISK_H
#define FS_INIT_INITRAMDISK_H

#include <fs/initrd/InitRD.h>

#include <boot/multiboot/Multiboot.h>

#include <hw/storage/RamDiskStorageDevice.h>

/*
* InitRamDisk is the initial ram disk used for the first-stage kernel initialization, before the main drive has been mounted.
* This class essentially is just for the loading and cleanup code for the ramdisk, which is supplied to the kernel as a multiboot module.
*/

namespace FS
{
	
	namespace InitRD
	{
		
		class InitRamDisk
		{
		public:	
			
			// Parse the multiboot module and mount the ramdisk.
			static void Init ( multiboot_info_t * MultibootInfo );
			
			// Unmount the ramdisk and free resources.
			static void Discard ();
			
		private:
			
			static void * ModulePhysical;
			static void * ModuleVirtual;
			static uint32_t Length;
			
			static HW::Storage::RamDiskStorageDevice * Device;
			
		};
		
	};
	
};

#endif
