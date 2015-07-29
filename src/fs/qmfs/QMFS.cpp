#include <fs/qmfs/QMFS.h>
#include <fs/qmfs/FileSystem.h>

void FS::QMFS :: Init ()
{
	
	FileSystem :: RegisterFileSystem ();
	
};
