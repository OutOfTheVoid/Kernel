#ifndef FS_MOUNTFS_FILESYSTEM_H
#define FS_MOUNTFS_FILESYSTEM_H

#include <fs/mountfs/FileSystem.h>
#include <fs/mountfs/MountFS.h>

#include <fs/vfs/FileSystem.h>

#include <hw/storage/StorageDevice.h>

/*
* Initialization and amanagement code for the Mount file system.
*/

namespace FS
{
	
	namespace MountFS
	{
		
		class FileSystem
		{
		public:
			
			typedef uint32_t MountStatus_t;
			
			static void Init ();
			
			static void MountDevice ( HW::Storage::StorageDevice * Device, MountStatus_t * Status );
			//static void UnmountDevice ( HW::Storage::StorageDevice * Device, MountStatus_t * Status );
			
		private:
			
			static const char * kMountDirectoryName;
			
			static VFS::FileSystem :: VFS_VirtualDirectory_FSNode * MountDirectory;
			
		};
		
	};
	
};

#endif
