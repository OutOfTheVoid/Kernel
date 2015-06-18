#ifndef MM_PAGING_PFA_H
#define MM_PAGING_PFA_H

#include <stdint.h>
#include <stddef.h>

#include <boot/multiboot/multiboot.h>

#include <mm/MM.h>
#include <mm/paging/Paging.h>
#include <mm/paging/PageTable.h>
#include <mm/paging/AddressSpace.h>
#include <mm/paging/PAlloc.h>

#include <system/func/kprintf.h>
#include <hw/video/VText.h>

#include <math/bitmath.h>

#define PFA_DEBUG

namespace MM
{
	
	namespace Paging
	{
		
		class PFA
		{
		public:
			
			static void Init ( multiboot_info_t * MultibootInfo, uint32_t MultibootInfoSize, uint32_t * KVStart );
			static void Init2 ( multiboot_info_t * MultibootInfo, uint32_t MultibootInfoSize );
			
			static size_t CalcMBICopySize ( multiboot_info_t * MultibootInfo );
			static void CopyMBI ( multiboot_info_t * Old, multiboot_info_t * New );
			
			static uint32_t GetFreeKB ();
			
			static bool Alloc ( uint32_t Length, void ** Address );
			static void Free ( void * Address );
			
			static uint32_t GetAllocationSize ( void * Address );
			
		private:
			
			static void MBEarlyAnalyze ( multiboot_info_t * MultibootInfo );
			
			static PAlloc :: PageFreeZone * GeneralPhysicalFree;
			static PAlloc :: PageAllocZone * KernelGeneralPhysicalAlloc;
			
			static multiboot_info_t * MBInfoCopy;
			
		};

	};

};

#endif
