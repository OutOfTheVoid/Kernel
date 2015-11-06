#include <fs/qmfs/FileSystem.h>
#include <fs/mountfs/MountList.h>

#include <util/string/String.h>

#include <cpputil/Unused.h>

#include <mm/KMalloc.h>

#include <system/func/kprintf.h>

FS::FileSystemEntry FS::QMFS::FileSystem :: FSEntry;

FS::FSFunctionBlock FS::QMFS::FileSystem :: FSFunctions;

const char * FS::QMFS::FileSystem :: kSignatureString = "qmfs.sig";

void FS::QMFS::FileSystem :: RegisterFileSystem ()
{
	
	FSEntry.Name = "qmfs";
	
	FSEntry.TestStorageDevice = & TestStorageDevice;
	FSEntry.MountStorageDevice = & MountStorageDevice;
	
	memzero ( reinterpret_cast <void *> ( & FSFunctions ), sizeof ( FSFunctions ) );
	
	for ( uint32_t I = 0; I < kNodeTypes; I ++ )
		FSFunctions.NodeTypeFlags [ I ] = kFSNodeTypeFlag_IllegalNodeType;
	
	FSFunctions.NodeTypeFlags [ kFSNodeType_Directory ] = kFSNodeTypeFlag_OpenClose | kFSNodeTypeFlag_Enumerate | kFSNodeTypeFlag_Find;
	FSFunctions.NodeTypeFlags [ kFSNodeType_File ] = kFSNodeTypeFlag_OpenClose | kFSNodeTypeFlag_Read | kFSNodeTypeFlag_Write;
	
	FSFunctions.Read = & Read;
	FSFunctions.Write = & Write;
	FSFunctions.Find = & Find;
	FSFunctions.Enumerate = & Enumerate;
	
	MountFS::MountList :: AddFileSystem ( & FSEntry );
	
};

bool FS::QMFS::FileSystem :: TestStorageDevice ( HW::Storage::StorageDevice * Device )
{
	
	uint8_t SignatureTest [ 8 ];
	
	HW::Storage::StorageDevice :: StorageStatus DeviceStatus;
	
	switch ( Device -> GetStorageType () )
	{
		
	case HW::Storage::StorageDevice :: kStorageType_RamDisk:
		
		if ( reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> GetLength () < kMinFSSize )
			return false;
		
		reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> Read ( & SignatureTest [ 0 ], 0, 8, & DeviceStatus );
		
		if ( DeviceStatus != HW::Storage::StorageDevice :: kStorageStatus_Success )
			return false;
		
		if ( strncmp ( reinterpret_cast <const char *> ( & SignatureTest [ 0 ] ), kSignatureString, kSignatureLength ) != 0 )
			return false;
		
		return true;
		
		break;
		
	default:
		
		if ( Device -> GetLengthLinear () < static_cast <uint64_t> ( kMinFSSize ) )
			return false;
		
		Device -> ReadLinear ( & SignatureTest [ 0 ], 0, 8, & DeviceStatus );
		
		if ( DeviceStatus != HW::Storage::StorageDevice :: kStorageStatus_Success )
			return false;
		
		if ( strncmp ( reinterpret_cast <const char *> ( & SignatureTest [ 0 ] ), kSignatureString, kSignatureLength ) != 0 )
			return false;
		
		break;
		
	}
	
	return false;
	
};

FS :: FileSystem_Instance * FS::QMFS::FileSystem :: MountStorageDevice ( HW::Storage::StorageDevice * Device )
{
	
	StorageHeader FSHeader;
	uint32_t Checksum;
	
	HW::Storage::StorageDevice :: StorageStatus Status;
	
	QMFS_FileSystem_Instance * FSInstance;
	QMFS_Directory_FSNode * RootNode;
	
	switch ( Device -> GetStorageType () )
	{
		
	case HW::Storage::StorageDevice :: kStorageType_RamDisk:
		{
			
			reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> Read ( reinterpret_cast <uint8_t *> ( & FSHeader ), 0, sizeof ( StorageHeader ), & Status );
			
			if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
				return NULL;
			
			Checksum = 0;
			
			for ( uint32_t I = 0; I < 4; I ++ )
				Checksum += reinterpret_cast <uint32_t *> ( & FSHeader ) [ I ];
			
			if ( Checksum != 0 )
				return NULL;
			
			FSInstance = reinterpret_cast <QMFS_FileSystem_Instance *> ( mm_kmalloc ( sizeof ( QMFS_FileSystem_Instance ) ) );
			
			if ( FSInstance == NULL )
				return NULL;
			
			RootNode = reinterpret_cast <QMFS_Directory_FSNode *> ( mm_kmalloc ( sizeof ( QMFS_Directory_FSNode ) ) );
			
			if ( RootNode == NULL )
			{
				
				mm_kfree ( FSInstance );
				
				return NULL;
				
			}
			
			QMFS_DirectoryNode DirectoryNode;
			
			reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> Read ( reinterpret_cast <uint8_t *> ( & DirectoryNode ), FSHeader.RootDirectoryOffset, sizeof ( QMFS_DirectoryNode ), & Status );
			
			if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
			{
				
				mm_kfree ( FSInstance );
				mm_kfree ( RootNode );
				
				return NULL;
				
			}
			
			if ( DirectoryNode.Header.QMFSNodeType != kQMFSNodeType_Directory )
			{
				
				mm_kfree ( FSInstance );
				mm_kfree ( RootNode );
				
				return NULL;
				
			}
			
			RootNode -> FSInstance = FSInstance;
			
			if ( ! EnumerateDirectoryChildren ( RootNode, & DirectoryNode, Device ) )
			{
				
				mm_kfree ( FSInstance );
				mm_kfree ( RootNode );
				
				return NULL;
				
			}
			
			FSInstance -> FSEntry = & FSEntry;
			FSInstance -> FSFunctions = & FSFunctions;
			
			FSInstance -> Device = Device;
			FSInstance -> RootNode = RootNode;
			
		}
		break;
		
	default:
		{
			
			Device -> ReadLinear ( reinterpret_cast <uint8_t *> ( & FSHeader ), 0, sizeof ( StorageHeader ), & Status );
			
			if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
				return NULL;
			
			Checksum = 0;
			
			for ( uint32_t I = 0; I < sizeof ( StorageHeader ); I += sizeof ( uint32_t ) )
				Checksum += * reinterpret_cast <uint32_t *> ( reinterpret_cast <uint32_t> ( & FSHeader ) + I );
			
			if ( Checksum != 0 )
				return NULL;
			
			FSInstance = reinterpret_cast <QMFS_FileSystem_Instance *> ( mm_kmalloc ( sizeof ( QMFS_FileSystem_Instance ) ) );
			
			if ( FSInstance == NULL )
				return NULL;
			
			RootNode = reinterpret_cast <QMFS_Directory_FSNode *> ( mm_kmalloc ( sizeof ( QMFS_Directory_FSNode ) ) );
			
			if ( RootNode == NULL )
			{
				
				mm_kfree ( FSInstance );
				
				return NULL;
				
			}
			
			union
			{
				
				QMFS_NodeHeader RootNodeHeader;
				QMFS_DirectoryNode DirectoryNode;
				
			};
			
			Device -> ReadLinear ( reinterpret_cast <uint8_t *> ( & RootNodeHeader ), 0, sizeof ( QMFS_DirectoryNode ), & Status );
			
			if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
			{
				
				mm_kfree ( FSInstance );
				mm_kfree ( RootNode );
				
				return NULL;
				
			}
			
			if ( RootNodeHeader.QMFSNodeType != kQMFSNodeType_Directory )
			{
				
				mm_kfree ( FSInstance );
				mm_kfree ( RootNode );
				
				return NULL;
				
			}
			
			RootNode -> FSInstance = FSInstance;
			
			if ( ! EnumerateDirectoryChildren ( RootNode, & DirectoryNode, Device ) )
			{
				
				mm_kfree ( FSInstance );
				mm_kfree ( RootNode );
				
				return NULL;
				
			}
			
			FSInstance -> FSEntry = & FSEntry;
			FSInstance -> FSFunctions = & FSFunctions;
			
			FSInstance -> Device = Device;
			FSInstance -> RootNode = RootNode;
			
		}
		break;
		
	}
	
	return FSInstance;
	
};

bool FS::QMFS::FileSystem :: EnumerateDirectoryChildren ( QMFS_Directory_FSNode * Directory, QMFS_DirectoryNode * StorageNode, HW::Storage::StorageDevice * Device )
{
	
	HW::Storage::StorageDevice :: StorageStatus Status;
	
	Directory -> FSNodeType = kFSNodeType_Directory;
	Directory -> Lock = MT::Synchronization::RWLock :: Initializer ();
	Directory -> ChildNodeCount = StorageNode -> ChildNodeCount;
	Directory -> ChildNodeArray = reinterpret_cast <FSNode **> ( mm_kmalloc ( sizeof ( FSNode * ) * Directory -> ChildNodeCount ) );
	
	if ( Directory -> ChildNodeArray == NULL )
		return false;
	
	Directory -> Name = reinterpret_cast <const char *> ( mm_kmalloc ( StorageNode -> Header.NameLength + 1 ) );
	
	if ( Directory -> Name == NULL )
	{
		
		mm_kfree ( Directory -> ChildNodeArray );
		Directory -> ChildNodeArray = NULL;
		
		return false;
		
	}
	
	switch ( Device -> GetStorageType () )
	{
		
	case HW::Storage::StorageDevice :: kStorageType_RamDisk:
		reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> Read ( reinterpret_cast <uint8_t *> ( const_cast <char *> ( Directory -> Name ) ), StorageNode -> Header.NameOffset, StorageNode -> Header.NameLength, & Status );
		break;
		
	default:
		Device -> ReadLinear ( reinterpret_cast <uint8_t *> ( const_cast <char *> ( Directory -> Name ) ), StorageNode -> Header.NameOffset, StorageNode -> Header.NameLength, & Status );
		break;
		
	}
	
	const_cast <char *> ( Directory -> Name ) [ StorageNode -> Header.NameLength ] = '\0';
	
	if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
	{
		
		mm_kfree ( const_cast <char *> ( Directory -> Name ) );
		mm_kfree ( Directory -> ChildNodeArray );
		Directory -> ChildNodeArray = NULL;
		
	}
	
	uint32_t HeaderAddresses [ StorageNode -> ChildNodeCount ];
	
	switch ( Device -> GetStorageType () )
	{
		
	case HW::Storage::StorageDevice :: kStorageType_RamDisk:
		reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> Read ( reinterpret_cast <uint8_t *> ( & HeaderAddresses [ 0 ] ), StorageNode -> ChildNodeOffsetArrayOffset, 4 * StorageNode -> ChildNodeCount, & Status );
		break;
		
	default:
		Device -> ReadLinear ( reinterpret_cast <uint8_t *> ( & HeaderAddresses [ 0 ] ), StorageNode -> ChildNodeOffsetArrayOffset, 4 * StorageNode -> ChildNodeCount, & Status );
		break;
		
	}
	
	if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
	{
		
		mm_kfree ( const_cast <char *> ( Directory -> Name ) );
		mm_kfree ( Directory -> ChildNodeArray );
		Directory -> ChildNodeArray = NULL;
		
		return false;
		
	}
	
	uint32_t I;
	
	for ( I = 0; I < Directory -> ChildNodeCount; I ++ )
	{
		
		union
		{
			
			QMFS_NodeHeader TestHeader;
			
			QMFS_DirectoryNode DirectoryHeader;
			QMFS_FileNode FileHeader;
			
		};
		
		switch ( Device -> GetStorageType () )
		{
			
		case HW::Storage::StorageDevice :: kStorageType_RamDisk:
			reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> Read ( reinterpret_cast <uint8_t *> ( & TestHeader ), HeaderAddresses [ I ], sizeof ( QMFS_NodeHeader ), & Status );
			break;
			
		default:
			Device -> ReadLinear ( reinterpret_cast <uint8_t *> ( & TestHeader ), HeaderAddresses [ I ], sizeof ( QMFS_NodeHeader ), & Status );
			break;
			
		}
		
		if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
		{
			
			mm_kfree ( const_cast <char *> ( Directory -> Name ) );
			mm_kfree ( Directory -> ChildNodeArray );
			Directory -> ChildNodeArray = NULL;
			
			return false;
			
		}
		
		switch ( TestHeader.QMFSNodeType )
		{
			
		case kFSNodeType_Directory:
			{
				
				switch ( Device -> GetStorageType () )
				{
					
				case HW::Storage::StorageDevice :: kStorageType_RamDisk:
					reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> Read ( reinterpret_cast <uint8_t *> ( & DirectoryHeader ), HeaderAddresses [ I ], sizeof ( QMFS_DirectoryNode ), & Status );
					break;
					
				default:
					Device -> ReadLinear ( reinterpret_cast <uint8_t *> ( & DirectoryHeader ), HeaderAddresses [ I ], sizeof ( QMFS_DirectoryNode ), & Status );
					break;
					
				}
				
				if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
				{
					
					uint32_t F;
					
					for ( F = 0; F < I; F ++ )
					{
						
						mm_kfree ( Directory -> ChildNodeArray [ F ] );
						Directory -> ChildNodeArray [ F ] = NULL;
						
					}
					
					mm_kfree ( const_cast <char *> ( Directory -> Name ) );
					mm_kfree ( Directory -> ChildNodeArray );
					Directory -> ChildNodeArray = NULL;
					
					return false;
					
				}
				
				QMFS_Directory_FSNode * FSDirectoryNode = reinterpret_cast <QMFS_Directory_FSNode *> ( mm_kmalloc ( sizeof ( QMFS_Directory_FSNode ) ) );
				
				if ( FSDirectoryNode == NULL )
				{
					
					uint32_t F;
					
					for ( F = 0; F < I; F ++ )
					{
						
						mm_kfree ( Directory -> ChildNodeArray [ F ] );
						Directory -> ChildNodeArray [ F ] = NULL;
						
					}
					
					mm_kfree ( const_cast <char *> ( Directory -> Name ) );
					mm_kfree ( Directory -> ChildNodeArray );
					Directory -> ChildNodeArray = NULL;
					
					return false;
					
				}
				
				FSDirectoryNode -> FSInstance = Directory -> FSInstance;
				
				if ( ! EnumerateDirectoryChildren ( FSDirectoryNode, & DirectoryHeader, Device ) )
				{
					
					uint32_t F;
					
					for ( F = 0; F < I; F ++ )
					{
						
						mm_kfree ( Directory -> ChildNodeArray [ F ] );
						Directory -> ChildNodeArray [ F ] = NULL;
						
					}
					
					mm_kfree ( const_cast <char *> ( Directory -> Name ) );
					mm_kfree ( Directory -> ChildNodeArray );
					Directory -> ChildNodeArray = NULL;
					
					mm_kfree ( FSDirectoryNode );
					
					return false;
					
				}
				
				Directory -> ChildNodeArray [ I ] = FSDirectoryNode;
				
			}
			break;
			
		case kFSNodeType_File:
			{
				
				switch ( Device -> GetStorageType () )
				{
					
				case HW::Storage::StorageDevice :: kStorageType_RamDisk:
					reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> Read ( reinterpret_cast <uint8_t *> ( & FileHeader ), HeaderAddresses [ I ], sizeof ( QMFS_FileNode ), & Status );
					break;
					
				default:
					Device -> ReadLinear ( reinterpret_cast <uint8_t *> ( & FileHeader ), HeaderAddresses [ I ], sizeof ( QMFS_FileNode ), & Status );
					break;
					
				}
				
				if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
				{
					
					uint32_t F;
					
					for ( F = 0; F < I; F ++ )
					{
						
						mm_kfree ( Directory -> ChildNodeArray [ F ] );
						Directory -> ChildNodeArray [ F ] = NULL;
						
					}
					
					mm_kfree ( const_cast <char *> ( Directory -> Name ) );
					mm_kfree ( Directory -> ChildNodeArray );
					Directory -> ChildNodeArray = NULL;
					
					return false;
					
				}
				
				QMFS_File_FSNode * FSFileNode = reinterpret_cast <QMFS_File_FSNode *> ( mm_kmalloc ( sizeof ( QMFS_File_FSNode ) ) );
				
				if ( FSFileNode == NULL )
				{
					
					uint32_t F;
					
					for ( F = 0; F < I; F ++ )
					{
						
						mm_kfree ( Directory -> ChildNodeArray [ F ] );
						Directory -> ChildNodeArray [ F ] = NULL;
						
					}
					
					mm_kfree ( const_cast <char *> ( Directory -> Name ) );
					mm_kfree ( Directory -> ChildNodeArray );
					Directory -> ChildNodeArray = NULL;
					
					return false;
					
				}
				
				FSFileNode -> Name = reinterpret_cast <const char *> ( mm_kmalloc ( FileHeader.Header.NameLength + 1 ) );
				
				if ( FSFileNode -> Name == NULL )
				{
					
					uint32_t F;
					
					for ( F = 0; F < I; F ++ )
					{
						
						mm_kfree ( Directory -> ChildNodeArray [ F ] );
						Directory -> ChildNodeArray [ F ] = NULL;
						
					}
					
					mm_kfree ( FSFileNode );
					
					mm_kfree ( const_cast <char *> ( Directory -> Name ) );
					mm_kfree ( Directory -> ChildNodeArray );
					Directory -> ChildNodeArray = NULL;
					
					return false;
					
				}
				
				switch ( Device -> GetStorageType () )
				{
					
				case HW::Storage::StorageDevice :: kStorageType_RamDisk:
					reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( Device ) -> Read ( reinterpret_cast <uint8_t *> ( const_cast <char *> ( FSFileNode -> Name ) ), FileHeader.Header.NameOffset, FileHeader.Header.NameLength, & Status );
					break;
					
				default:
					Device -> ReadLinear ( reinterpret_cast <uint8_t *> ( const_cast <char *> ( FSFileNode -> Name ) ), FileHeader.Header.NameOffset, FileHeader.Header.NameLength, & Status );
					break;
					
				}
				
				if ( Status != HW::Storage::StorageDevice :: kStorageStatus_Success )
				{
					
					uint32_t F;
					
					for ( F = 0; F < I; F ++ )
					{
						
						mm_kfree ( Directory -> ChildNodeArray [ F ] );
						Directory -> ChildNodeArray [ F ] = NULL;
						
					}
					
					mm_kfree ( const_cast <char *> ( FSFileNode -> Name ) );
					mm_kfree ( FSFileNode );
					
					mm_kfree ( const_cast <char *> ( Directory -> Name ) );
					mm_kfree ( Directory -> ChildNodeArray );
					Directory -> ChildNodeArray = NULL;
					
					return false;
					
				}
				
				const_cast <char *> ( FSFileNode -> Name ) [ FileHeader.Header.NameLength ] = '\0';
				
				FSFileNode -> FSNodeType = kFSNodeType_File;
				FSFileNode -> FSInstance = Directory -> FSInstance;
				FSFileNode -> DiskOffsetLinear = FileHeader.Offset;
				FSFileNode -> LengthLinear = FileHeader.Length;
				
			}
			break;
			
		default:
			break;
			
		}
		
	}
	
	return true;
	
};

void FS::QMFS::FileSystem :: Open ( FSNode * Node, FSStatus_t * Status )
{
	
	UNUSED ( Node );
	
	* Status = kFSStatus_Success;
	
};

void FS::QMFS::FileSystem :: Close ( FSNode * Node, FSStatus_t * Status )
{
	
	UNUSED ( Node );
	
	* Status = kFSStatus_Success;
	
};

void FS::QMFS::FileSystem :: Read ( FSNode * Node, uint8_t * Buffer, uint32_t Position, uint32_t Length, FSStatus_t * Status )
{
	
	if ( Node -> FSNodeType != kFSNodeType_File )
	{
		
		* Status = kFSStatus_Failure_NonFile;
		
		return;
		
	}
	
	uint32_t Offset = reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> DiskOffsetLinear + Position;
	
	if ( Length > reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> LengthLinear )
	{
		
		* Status = kFSStatus_Failure_Length;
		
		return;
		
	}
	
	if ( ( Offset < reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> DiskOffsetLinear ) || ( Position + Length >= reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> LengthLinear ) )
	{
		
		* Status = kFSStatus_Failure_Position;
		
		return;
		
	}
	
	MT::Synchronization::RWLock :: ReadAcquire ( & Node -> Lock );
	
	HW::Storage::StorageDevice :: StorageStatus SStatus;
	
	switch ( reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> FSInstance -> Device -> GetStorageType () )
	{
		
	case HW::Storage::StorageDevice :: kStorageType_RamDisk:
		reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> FSInstance -> Device ) -> Read ( Buffer, Offset, reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> LengthLinear, & SStatus );
		break;
		
	default:
		reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> FSInstance -> Device -> ReadLinear ( Buffer, Offset, reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> LengthLinear, & SStatus );
		
	}
	
	MT::Synchronization::RWLock :: ReadRelease ( & Node -> Lock );
	
	if ( SStatus != HW::Storage::StorageDevice :: kStorageStatus_Success )
	{
		
		* Status = kFSStatus_Failure_StorageDevice;
		
		return;
		
	}
	
	* Status = kFSStatus_Success;
	
};

void FS::QMFS::FileSystem :: Write ( FSNode * Node, uint8_t * Buffer, uint32_t Position, uint32_t Length, FSStatus_t * Status )
{
	
	if ( Node -> FSNodeType != kFSNodeType_File )
	{
		
		* Status = kFSStatus_Failure_NonFile;
		
		return;
		
	}
	
	uint32_t Offset = reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> DiskOffsetLinear + Position;
	
	if ( Length > reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> LengthLinear )
	{
		
		* Status = kFSStatus_Failure_Length;
		
		return;
		
	}
	
	if ( ( Offset < reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> DiskOffsetLinear ) || ( Position + Length >= reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> LengthLinear ) )
	{
		
		* Status = kFSStatus_Failure_Position;
		
		return;
		
	}
	
	MT::Synchronization::RWLock :: WriteAcquire ( & Node -> Lock );
	
	HW::Storage::StorageDevice :: StorageStatus SStatus;
	
	switch ( reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> FSInstance -> Device -> GetStorageType () )
	{
		
	case HW::Storage::StorageDevice :: kStorageType_RamDisk:
		reinterpret_cast <HW::Storage::RamDiskStorageDevice *> ( reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> FSInstance -> Device ) -> Write ( Buffer, Offset, reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> LengthLinear, & SStatus );
		break;
		
	default:
		reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> FSInstance -> Device -> WriteLinear ( Buffer, Offset, reinterpret_cast <QMFS_File_FSNode *> ( Node ) -> LengthLinear, & SStatus );
		
	}
	
	MT::Synchronization::RWLock :: WriteRelease ( & Node -> Lock );
	
	if ( SStatus != HW::Storage::StorageDevice :: kStorageStatus_Success )
	{
		
		* Status = kFSStatus_Failure_StorageDevice;
		
		return;
		
	}
	
	* Status = kFSStatus_Success;
	
};

void FS::QMFS::FileSystem :: Enumerate ( FSNode * Node, uint32_t ChildIndex, const char ** ChildNamePTR, FSStatus_t * Status )
{
	
	if ( Node -> FSNodeType != kFSNodeType_Directory )
	{
		
		* Status = kFSStatus_Failure_NonDirectory;
		
		return;
		
	}
	
	MT::Synchronization::RWLock :: ReadAcquire ( & Node -> Lock );
	
	if ( reinterpret_cast <QMFS_Directory_FSNode *> ( Node ) -> ChildNodeCount > ChildIndex )
	{
		
		* ChildNamePTR = reinterpret_cast <QMFS_Directory_FSNode *> ( Node ) -> ChildNodeArray [ ChildIndex ] -> Name;
		
		* Status = kFSStatus_Success;
		
	}
	else
	{
		
		* ChildNamePTR = NULL;
		
		* Status = kFSStatus_Failure_Existance;
	
	}
	
	MT::Synchronization::RWLock :: ReadRelease ( & Node -> Lock );
	
};

void FS::QMFS::FileSystem :: Find ( FSNode * Node, const char * Name, FSNode ** ChildPTR, FSStatus_t * Status )
{
	
	if ( Node -> FSNodeType != kFSNodeType_Directory )
	{
		
		* Status = kFSStatus_Failure_NonDirectory;
		
		return;
		
	}
	
	* ChildPTR = NULL;
	
	MT::Synchronization::RWLock :: ReadAcquire ( & Node -> Lock );
	
	for ( uint32_t I = 0; I < reinterpret_cast <QMFS_Directory_FSNode *> ( Node ) -> ChildNodeCount; I ++ )
	{
		
		if ( strcmp ( Name, reinterpret_cast <QMFS_Directory_FSNode *> ( Node ) -> ChildNodeArray [ I ] -> Name ) == 0 )
		{
			
			* ChildPTR = reinterpret_cast <QMFS_Directory_FSNode *> ( Node ) -> ChildNodeArray [ I ];
			
			break;
			
		}
		
	}
	
	MT::Synchronization::RWLock :: ReadRelease ( & Node -> Lock );
	
	if ( * ChildPTR == NULL )
	{
		
		* Status = kFSStatus_Failure_Existance;
		
		return;
		
	}
	
	* Status = kFSStatus_Success;
	
};
