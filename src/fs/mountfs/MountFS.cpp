#include <fs/mountfs/MountFS.h>
#include <fs/mountfs/FileSystem.h>
#include <fs/mountfs/MountList.h>

void FS::MountFS :: Init ()
{
	
	FS::MountFS::FileSystem :: Init ();
	FS::MountFS::MountList :: Init ();
	
};
