#include <fs/FS.h>

#include <fs/vfs/VFS.h>
#include <fs/mountfs/MountFS.h>
#include <fs/qmfs/QMFS.h>

#include <fs/initrd/InitRamDisk.h>

#include <system/func/kprintf.h>

void FS :: Init ( multiboot_info_t * MultibootInfo )
{
	
	VFS :: Init ();
	MountFS :: Init ();
	
	QMFS :: Init ();
	
	FS::InitRD::InitRamDisk :: Init ( MultibootInfo );
	
};

