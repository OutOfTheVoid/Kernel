#ifndef FS_QMFS_FILESYSTEM_H
#define FS_QMFS_FILESYSTEM_H

#include <fs/qmfs/QMFS.h>
#include <fs/FS.h>

#include <hw/storage/StorageDevice.h>
#include <hw/storage/RamDiskStorageDevice.h>

namespace FS
{
	
	namespace QMFS
	{
		
		class FileSystem
		{
		public:
			
			static void RegisterFileSystem ();
			
			static bool TestStorageDevice ( HW::Storage::StorageDevice * Device );
			static FileSystem_Instance * MountStorageDevice ( HW::Storage::StorageDevice * Device );
			
		private:
			
			static const uint64_t kMinFSSize = 28;
			
			static const char * kSignatureString;
			static const uint32_t kSignatureLength = 8;
			
			static const uint32_t kQMFSNodeType_File = 0;
			static const uint32_t kQMFSNodeType_Directory = 1;
			
			typedef struct
			{
				
				uint8_t Signature [ kSignatureLength ];
				
				uint32_t RootDirectoryOffset;
				
				uint32_t Checksum;
				
			} __attribute__ (( packed )) StorageHeader;
			
			typedef struct QMFS_NodeHeader_Struct
			{
				
				uint32_t QMFSNodeType;
				
				uint32_t NameOffset;
				uint32_t NameLength;
				
			} __attribute__ (( packed )) QMFS_NodeHeader;
			
			typedef struct QMFS_DirectoryNode_Struct
			{
				
				QMFS_NodeHeader Header;
				
				uint32_t ChildNodeOffsetArrayOffset;
				uint32_t ChildNodeCount;
				
			} __attribute__ (( packed )) QMFS_DirectoryNode;
			
			typedef struct QMFS_FileNode_Struct
			{
				
				QMFS_NodeHeader Header;
				
				uint32_t Offset;
				uint32_t Length;
				
			} __attribute__ (( packed )) QMFS_FileNode;
			
			typedef struct QMFS_FileSystem_Instance_Struct : public FileSystem_Instance
			{
			} QMFS_FileSystem_Instance;
			
			
			
			typedef struct QMFS_File_FSNode_Struct : public FSNode
			{
				
				uint32_t DiskOffsetLinear;
				uint32_t LengthLinear;
				
			} QMFS_File_FSNode;
			
			typedef struct QMFS_Directory_FSNode_Struct : public FSNode
			{
				
				uint32_t ChildNodeCount;
				FSNode ** ChildNodeArray;
				
			} QMFS_Directory_FSNode;
			
			static FS::FileSystemEntry FSEntry;
			
			static FS::FSFunctionBlock FSFunctions;
			
			static bool EnumerateDirectoryChildren ( QMFS_Directory_FSNode * Directory, QMFS_DirectoryNode * StorageNode, HW::Storage::StorageDevice * Device );
			
			static void Open ( FSNode * Node, FSStatus_t * Status );
			static void Close ( FSNode * Node, FSStatus_t * Status );
			
			static void Read ( FSNode * Node, uint8_t * Buffer, uint32_t Position, uint32_t Length, FSStatus_t * Status );
			static void Write ( FSNode * Node, uint8_t * Buffer, uint32_t Position, uint32_t Length, FSStatus_t * Status );
			
			static void Enumerate ( FSNode * Node, uint32_t ChildIndex, const char ** ChildNamePTR, FSStatus_t * Status );
			static void Find ( FSNode * Node, const char * Name, FSNode ** ChildPTR, FSStatus_t * Status );
			
		};
		
	};
	
};

#endif
