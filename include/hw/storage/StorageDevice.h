#ifndef HW_STORAGE_STORAGEDEVICE_H
#define HW_STORAGE_STORAGEDEVICE_H

#include <hw/storage/Storage.h>

#include <stdint.h>

namespace HW
{
	
	namespace Storage
	{
		
		class StorageDevice
		{
		public:
			
			typedef enum
			{
				
				kStorageType_RamDisk
				
			} StorageType;
			
			typedef enum
			{
				
				kStorageStatus_Success,
				kStorageStatus_Failure_NoRead,
				kStorageStatus_Failure_NoWrite,
				kStorageStatus_Failure_Bounds
				
			} StorageStatus;
			
			virtual StorageType GetStorageType () = 0;
			virtual const char * GetName () = 0;
			
			virtual void ReadLinear ( uint8_t * Buffer, uint64_t Offset, uint64_t Length, StorageStatus * Status ) = 0;
			virtual void WriteLinear ( uint8_t * Buffer, uint64_t Offset, uint64_t Length, StorageStatus * Status ) = 0;
			
			virtual uint64_t GetLengthLinear () = 0;
			
		};
		
	};
	
};

#endif
