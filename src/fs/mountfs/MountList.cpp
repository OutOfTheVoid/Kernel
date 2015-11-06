#include <fs/mountfs/MountList.h>

void FS::MountFS::MountList :: Init ()
{
	
	FileSystems = new Vector <FileSystemEntry *> ();
	
};

void FS::MountFS::MountList :: AddFileSystem ( FileSystemEntry * Entry )
{
	
	FileSystems -> Push ( Entry );
	
};
