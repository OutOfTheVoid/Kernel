#ifndef FS_MOUNT_MOUNTFS_H
#define FS_MOUNT_MOUNTFS_H

#include <fs/mountfs/MountFS.h>

#include <fs/FS.h>

#include <util/Vector.h>

/*
* MountFS is the virtual filesystem for mounted drives (other than the root filesystem).
* It's not technically using any other FS than the VFS, but I decided to seperate it
* from the vfs root for organization reasons.
*/

namespace FS
{
	
	namespace MountFS
	{
		
		void Init ();
		
	};
	
};

#endif