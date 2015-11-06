#ifndef FS_MOUNT_MOUNTFS_H
#define FS_MOUNT_MOUNTFS_H

#include <fs/mount/Mount.h>

#include <fs/FS.h>

#include <util/Vector.h>

/*
* MountList maintains a list of filesystems installed into the kernel, as well
* as a list of mounted devices. It is here from which the kernel manipulates the
* vfs when mounting a drive other than the root file system.
*/

namespace FS
{
	
	namespace MountFS
	{
		
		class FileSystem;
		
		class MountList
		{
		public:
			
			static void Init ();
			
			static void AddFileSystem ( FileSystemEntry * Entry );
			
		private:
			
			friend class FileSystem;
			
			Vector <FileSystemEntry *> * FileSystems;
			
		};
		
	};
	
};

#endif