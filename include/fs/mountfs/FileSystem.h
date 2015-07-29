#ifndef FS_MOUNTFS_FILESYSTEM_H
#define FS_MOUNTFS_FILESYSTEM_H

#include <fs/mountfs/FileSystem.h>
#include <fs/mountfs/MountFS.h>

#include <fs/vfs/FileSystem.h>

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
			
			static void Init ();
			
		private:
			
			static const char * kMountDirectoryName;
			
			static VFS::FileSystem :: VFS_VirtualDirectory_FSNode * MountDirectory;
			
		};
		
	};
	
};

#endif
