#ifndef FS_INIT_INITRAMDISK_H
#define FS_INIT_INITRAMDISK_H

#include <fs/initrd/InitRD.h>

#include <boot/multiboot/Multiboot.h>

#include <hw/storage/RamDiskStorageDevice.h>

namespace FS
{
	
	namespace InitRD
	{
		
		class InitRamDisk
		{
		public:	
			
			static void Init ( multiboot_info_t * MultibootInfo );
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
