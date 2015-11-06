#include <fs/mountfs/FileSystem.h>
#include <fs/vfs/FileSystem.h>
#include <fs/FS.h>

#include <fs/mountfs/MountList.h>

#include <system/func/Panic.h>

#include <mt/synchronization/RWLock.h>

#include <mm/KMalloc.h>

#include <cpputil/Memory.h>

const char * FS::MountFS::FileSystem :: kMountDirectoryName = "Mount";

FS::VFS::FileSystem :: VFS_VirtualDirectory_FSNode * FS::MountFS::FileSystem :: MountDirectory;

Vector <FS::FileSystem_Instance *> * FS::MountFS::FileSystem :: FSInstances = NULL;

void FS::MountFS::FileSystem :: Init ()
{
	
	MountDirectory = reinterpret_cast <VFS::FileSystem :: VFS_VirtualDirectory_FSNode_Struct *> ( mm_kmalloc ( sizeof ( VFS::FileSystem :: VFS_VirtualDirectory_FSNode_Struct ) ) );
	
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
	
	FSInstances = new Vector <FileSystem_Instance *> ();
	
};

void FS::MountFS::FileSystem :: MountDevice ( HW::Storage::StorageDevice * Device, MountStatus_t * Status )
{
	
	FileSystemEntry * MountEntry = MountList :: FindMountableFileSystem ( Device );
	
	if ( MountEntry == NULL )
	{
		
		* Status = kMountStatus_Failure_FileSystemUnrecognized;
		
		return;
		
	}
	
	FileSystem_Instance * Instance = MountEntry -> MountStorageDevice ( Device );
	
	if ( Instance == NULL )
	{
		
		* Status = kMountStatus_Failure_MountFailure;
		
		return;
		
	}
	
	FSStatus_t SubStatus;
	
	FS::VFS::FileSystem :: Add ( MountDirectory, Instance -> RootNode, & SubStatus );
	
	if ( SubStatus != kFSStatus_Success )
	{
		
		// TODO: Unmount device
		
		* Status = kMountStatus_Failure_VFSStaging;
		
		return;
		
	};
	
	FSInstances -> Push ( Instance );
	
	* Status = kMountStatus_Success;
	
};

void FS::MountFS::FileSystem :: UnmountDevice ( HW::Storage::StorageDevice * Device, MountStatus_t * Status )
{
	
	( void ) Device;
	( void ) Status;
	
};
