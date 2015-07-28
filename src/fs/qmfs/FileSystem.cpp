#include <fs/qmfs/FileSystem.h>

#include <util/string/String.h>

FS::FileSystemEntry FS::QMFS::FileSystem :: FSEntry;

const char * FS::QMFS::FileSystem :: kSignatureString = "__qmfs__";

void FS::QMFS::FileSystem :: Init ()
{
	
	FSEntry.Name = "qmfs";
	
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

FS :: FSNode * FS::QMFS::FileSystem :: MountDevice ( HW::Storage::StorageDevice * Device )
{
	
	
	
};
