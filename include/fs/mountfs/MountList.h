#ifndef FS_MOUNT_MOUNTLIST_H
#define FS_MOUNT_MOUNTLIST_H

#include <fs/FS.h>

#include <util/Vector.h>

#include <mt/synchronization/RWLock.h>

#include <hw/storage/StorageDevice.h>

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
			
			// Initialize the mount list.
			static void Init ();
			
			// Add a filesystem driver to the list for the ability to mount.
			static void AddFileSystem ( FileSystemEntry * Entry );
			
		private:
			
			friend class FileSystem;
			
			static FileSystemEntry * FindMountableFileSystem ( HW::Storage::StorageDevice * Device );
			
			static Vector <FileSystemEntry *> * FileSystems;
			
			static MT::Synchronization::RWLock :: RWLock_t ListLock;
			
		};
		
	};
	
};

#endif