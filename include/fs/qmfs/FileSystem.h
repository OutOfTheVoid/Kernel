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
			
			static void Init ();
			
			static bool TestStorageDevice ( HW::Storage::StorageDevice * Device );
			static FSNode * MountDevice ( HW::Storage::StorageDevice * Device );
			
		private:
			
			static const uint64_t kMinFSSize = 8;
			
			static const char * kSignatureString;
			static const uint32_t kSignatureLength = 8;
			
			static FS::FileSystemEntry FSEntry;
			
			typedef struct QMFS_Directory_FSNode_Struct : public FSNode
			{
				
				
				
			} QMFS_Directory_FSNode;
			
		};
		
	};
	
};

#endif
