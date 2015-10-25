#ifndef MM_PAGING_PAGETABLE_H
#define MM_PAGING_PAGETABLE_H

#include <boot/multiboot/multiboot.h>

#include <mm/MM.h>
#include <mm/paging/Paging.h>

#include <mt/synchronization/Spinlock.h>

#include <stdint.h>

namespace MM
{
	
	namespace Paging
	{
		
		class Invalidation;
		
		class PageTable
		{
		public:
			
			typedef uint32_t * PDirectory;
			
			static const uint32_t Flags_Cutsom_KMap = 0x200;
			static const uint32_t Flags_Global = 0x100;
			static const uint32_t Flags_Dirty = 0x40;
			static const uint32_t Flags_Accessed = 0x20;
			static const uint32_t Flags_NoCache = 0x10;
			static const uint32_t Flags_WriteThrough = 0x08;
			static const uint32_t Flags_User = 0x04;
			static const uint32_t Flags_Writeable = 0x02;
			static const uint32_t Flags_Present = 0x01;
			
			static void KInit ();
			static PDirectory GetKernelPD ();
			
			PageTable ( bool MapKernel );
			~PageTable ();
			
			void IncrementRef ();
			void DecrementRef ();
			
			void SetMapping ( uint32_t Virtual, uint32_t Physical, uint32_t Flags );
			void SetRegionMapping ( uint32_t Virtual, uint32_t Physical, uint32_t Length, uint32_t Flags );
			
			void ClearMapping ( uint32_t Virtual );
			void ClearRegionMapping ( uint32_t Virtual, uint32_t Length );
			
			uint32_t GetCR3 ();
			
			static void SetKernelMapping ( uint32_t Virtual, uint32_t Physical, uint32_t Flags );
			static void SetKernelRegionMapping ( uint32_t Virtual, uint32_t Physical, uint32_t Length, uint32_t Flags );
			
			static void ClearKernelMapping ( uint32_t Virtual );
			static void ClearKernelRegionMapping ( uint32_t Virtual, uint32_t Length );
			
			static uint32_t KernelVirtualToPhysical ( uint32_t Virtual );
			
			static void EnableKPaging ();
			static void DisableKPaging ();
			
			static void FlushKPDirectory ();
			
			static bool KPagingStatus ();
			
			static bool ShouldFault ( uint32_t Address, bool Write );
			
		private:
			
			friend class Invalidation;
			
			static void FlushEntry ( uint32_t Virtual );
			
			PDirectory DirectoryPhysical;
			PDirectory DirectoryVirtual;
			
			uint32_t RefCount;
			
			MT::Synchronization::Spinlock :: Spinlock_t Lock;
			
			static void SetDirectoryEntry ( uint32_t * Directory, uint32_t TableAddress, uint32_t Flags );
			
			static const uint32_t DFlags_4MB = 0x40;
			static const uint32_t DFlags_Accessed = 0x20;
			static const uint32_t DFlags_NoCache = 0x10;
			static const uint32_t DFlags_WriteThrough = 0x08;
			static const uint32_t DFlags_User = 0x04;
			static const uint32_t DFlags_Writeable = 0x02;
			static const uint32_t DFlags_Present = 0x01;
			
		};

	};

};

#endif
