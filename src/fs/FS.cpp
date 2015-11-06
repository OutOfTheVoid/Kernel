#include <fs/FS.h>

#include <fs/vfs/VFS.h>
#include <fs/mountfs/MountFS.h>

#include <fs/qmfs/QMFS.h>

void FS :: Init ()
{
	
	VFS :: Init ();
	MountFS :: Init ();
	
	QMFS :: Init ();
	
};

