#ifndef HW_STORAGE_RAMDISKSTORAGEDEVICE_H
#define HW_STORAGE_RAMDISKSTORAGEDEVICE_H

#include <hw/storage/RamDiskStorageDevice.h>
#include <hw/storage/StorageDevice.h>

#include <mt/synchronization/RWLock.h>

namespace HW
{
	
	namespace Storage
	{
		
		class RamDiskStorageDevice : public StorageDevice
		{
		public:
			
			static const uint32_t kFlag_Read = 1;
			static const uint32_t kFlag_Write = 2;
			
			RamDiskStorageDevice ( const char * Name, void * Address, uint32_t Length, uint32_t Flags );
			~RamDiskStorageDevice ();
			
			const char * GetName ();
			
			void Read ( uint8_t * Buffer, uint32_t Offset, uint32_t Length, StorageStatus * Status );
			void Write ( uint8_t * Buffer, uint32_t Offset, uint32_t Length, StorageStatus * Status );
			
			void ReadLinear ( uint8_t * Buffer, uint64_t Offset, uint64_t Length, StorageStatus * Status );
			void WriteLinear ( uint8_t * Buffer, uint64_t Offset, uint64_t Length, StorageStatus * Status );
			
			StorageType GetStorageType ();
			
			uint32_t GetLength ();
			
			uint64_t GetLengthLinear ();
			
		private:
			
			uint32_t Flags;
			
			void * Address;
			uint32_t Length;
			
			const char * Name;
			
			MT::Synchronization::RWLock :: RWLock_t Lock;
			
		};
		
	};
	
};

#endif
