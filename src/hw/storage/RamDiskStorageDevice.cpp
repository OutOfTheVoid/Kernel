#include <hw/storage/RamDiskStorageDevice.h>

#include <system/func/Panic.h>

#include <util/string/String.h>

HW::Storage::RamDiskStorageDevice :: RamDiskStorageDevice ( const char * Name, void * Address, uint32_t Length, uint32_t Flags ):
	Flags ( Flags ),
	Address ( Address ),
	Length ( Length ),
	Name ( Name ),
	Lock ()
{
	
	Lock = MT::Synchronization::RWLock :: Initializer ();
	
	if ( reinterpret_cast <uint32_t> ( Address ) + Length < reinterpret_cast <uint32_t> ( Address ) )
		KPANIC ( "Memory range [ Address: %H, Length: %h ] supplied to [ RamDiskStorageDevice %s ] wraps around 32 bit address space", Name, reinterpret_cast <uint32_t> ( Address ), Length );
	
};

HW::Storage::RamDiskStorageDevice :: ~RamDiskStorageDevice ()
{
};

const char * HW::Storage::RamDiskStorageDevice :: GetName ()
{
	
	return Name;
	
};

void HW::Storage::RamDiskStorageDevice :: Read ( uint8_t * Buffer, uint32_t Offset, uint32_t Length, StorageStatus * Status )
{
	
	if ( ( Flags & kFlag_Read ) == 0 )
	{
		
		* Status = kStorageStatus_Failure_NoRead;
		
		return;
		
	}
	
	if ( Offset + Length < Offset )
	{
		
		* Status = kStorageStatus_Failure_Bounds;
		
		return;
		
	}
	
	if ( Offset + Length > this -> Length )
	{
		
		* Status = kStorageStatus_Failure_Bounds;
		
		return;
		
	}
	
	MT::Synchronization::RWLock :: ReadAcquire ( & Lock );
	
	memcpy ( Buffer, reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Address ) + Offset ), Length );
	
	MT::Synchronization::RWLock :: ReadRelease ( & Lock );
	
};

void HW::Storage::RamDiskStorageDevice :: Write ( uint8_t * Buffer, uint32_t Offset, uint32_t Length, StorageStatus * Status )
{
	
	if ( ( Flags & kFlag_Write ) == 0 )
	{
		
		* Status = kStorageStatus_Failure_NoWrite;
		
		return;
		
	}
	
	if ( Offset + Length < Offset )
	{
		
		* Status = kStorageStatus_Failure_Bounds;
		
		return;
		
	}
	
	if ( Offset + Length > this -> Length )
	{
		
		* Status = kStorageStatus_Failure_Bounds;
		
		return;
		
	}
	
	MT::Synchronization::RWLock :: WriteAcquire ( & Lock );
	
	memcpy ( reinterpret_cast <void *> ( reinterpret_cast <uint32_t> ( Address ) + Offset ), Buffer, Length );
	
	MT::Synchronization::RWLock :: WriteRelease ( & Lock );
	
};

void HW::Storage::RamDiskStorageDevice :: ReadLinear ( uint8_t * Buffer, uint64_t Offset, uint64_t Length, StorageStatus * Status )
{
	
	if ( ( Offset + Length > 0xFFFFFFFF ) || ( Offset + Length > Offset ) )
	{
		
		* Status = kStorageStatus_Failure_Bounds;
		
		return;
		
	}
	
	Read ( Buffer, static_cast <uint32_t> ( Offset ), static_cast <uint32_t> ( Length ), Status );
	
};

void HW::Storage::RamDiskStorageDevice :: WriteLinear ( uint8_t * Buffer, uint64_t Offset, uint64_t Length, StorageStatus * Status )
{
	
	if ( ( Offset + Length > 0xFFFFFFFF ) || ( Offset + Length > Offset ) )
	{
		
		* Status = kStorageStatus_Failure_Bounds;
		
		return;
		
	}
	
	Write ( Buffer, static_cast <uint32_t> ( Offset ), static_cast <uint32_t> ( Length ), Status );
	
};

HW::Storage::StorageDevice :: StorageType HW::Storage::RamDiskStorageDevice :: GetStorageType ()
{
	
	return kStorageType_RamDisk;
	
};

uint32_t HW::Storage::RamDiskStorageDevice :: GetLength ()
{
	
	return Length;
	
};

uint64_t HW::Storage::RamDiskStorageDevice :: GetLengthLinear ()
{
	
	return GetLength ();
	
};
