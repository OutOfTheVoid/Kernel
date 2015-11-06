#include <fs/mountfs/FileSystem.h>
#include <fs/vfs/FileSystem.h>
#include <fs/FS.h>

#include <system/func/Panic.h>

#include <mt/synchronization/RWLock.h>

#include <mm/KMalloc.h>

#include <cpputil/Memory.h>

const char * FS::MountFS::FileSystem :: kMountDirectoryName = "Mount";

void FS::MountFS::FileSystem :: Init ()
{
	
	VFS::FileSystem :: VFS_VirtualDirectory_FSNode_Struct * MountDirectory = reinterpret_cast <VFS::FileSystem :: VFS_VirtualDirectory_FSNode_Struct *> ( mm_kmalloc ( sizeof ( VFS::FileSystem :: VFS_VirtualDirectory_FSNode_Struct ) ) );
	
	if ( MountDirectory == NULL )
		KPANIC ( "Failed to allocate virtual directory node for mount file system." );
	
	MountDirectory -> Name = kMountDirectoryName;
	MountDirectory -> FSNodeType = FS :: kFSNodeType_Directory;
	MountDirectory -> Lock = MT::Synchronization::RWLock :: Initializer ();
	new ( & ( MountDirectory -> Children ) ) Vector <FSNode *> ();
	
	FSNode * RootNode = FS::VFS::FileSystem :: GetRootNode ();
	
	FSStatus_t Status;
	
	FS::VFS::FileSystem :: Add ( RootNode, MountDirectory, & Status );
	
	if ( Status != kFSStatus_Success )
		KPANIC ( "Failed to add mound directory to system root." );
	
};
