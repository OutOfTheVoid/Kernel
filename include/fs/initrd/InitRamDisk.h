#ifndef FS_INIT_INITRAMDISK_H
#define FS_INIT_INITRAMDISK_H

#include <fs/initrd/InitRD.h>

#include <boot/multiboot/Multiboot.h>

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
			
		};
		
	};
	
};

#endif
