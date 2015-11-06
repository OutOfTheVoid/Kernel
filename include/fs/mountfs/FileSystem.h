#ifndef FS_MOUNTFS_FILESYSTEM_H
#define FS_MOUNTFS_FILESYSTEM_H

#include <fs/mountfs/FileSystem.h>
#include <fs/mountfs/MountFS.h>

#include <fs/vfs/FileSystem.h>

#include <hw/storage/StorageDevice.h>

#include <util/Vector.h>

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
			
			static const MountStatus_t kMountStatus_Success = 0;
			static const MountStatus_t kMountStatus_Failure_FileSystemUnrecognized = 1;
			static const MountStatus_t kMountStatus_Failure_MountFailure = 2;
			static const MountStatus_t kMountStatus_Failure_VFSStaging = 3;
			
			static void Init ();
			
			static void MountDevice ( HW::Storage::StorageDevice * Device, MountStatus_t * Status );
			static void UnmountDevice ( HW::Storage::StorageDevice * Device, MountStatus_t * Status );
			
		private:
			
			static const char * kMountDirectoryName;
			
			static VFS::FileSystem :: VFS_VirtualDirectory_FSNode * MountDirectory;
			
			static Vector <FileSystem_Instance *> * FSInstances;
			
		};
		
	};
	
};

#endif
