#include <fs/mountfs/MountList.h>

#include <stdint.h>

MT::Synchronization::RWLock :: RWLock_t FS::MountFS::MountList :: ListLock = MT::Synchronization::RWLock :: Initializer ();

Vector <FS :: FileSystemEntry *> * FS::MountFS::MountList :: FileSystems = NULL;

void FS::MountFS::MountList :: Init ()
{
	
	FileSystems = new Vector <FileSystemEntry *> ();
	
};

void FS::MountFS::MountList :: AddFileSystem ( FileSystemEntry * Entry )
{
	
	MT::Synchronization::RWLock :: WriteAcquire ( & ListLock );
	
	FileSystems -> Push ( Entry );
	
	MT::Synchronization::RWLock :: WriteRelease ( & ListLock );
	
};

FS::FileSystemEntry * FS::MountFS::MountList :: FindMountableFileSystem ( HW::Storage::StorageDevice * Device )
{
	
	FileSystemEntry * Entry = NULL;
	
	MT::Synchronization::RWLock :: ReadAcquire ( & ListLock );
	
	for ( uint32_t I = 0; I < FileSystems -> Length (); I ++ )
	{
		
		if ( ( * FileSystems ) [ I ] -> TestStorageDevice ( Device ) )
		{
			
			Entry = ( * FileSystems ) [ I ];
			
			break;
			
		}
		
	}
	
	MT::Synchronization::RWLock :: ReadRelease ( & ListLock );
	
	return Entry;
	
};
