#ifndef LINK_ELF_LIVEIMAGE_H
#define LINK_ELF_LIVEIMAGE_H

#include <link/elf/Elf.h>

#include <boot/multiboot/multiboot.h>

namespace Link
{
	
	namespace Elf
	{
		
		class LiveImage
		{
		public:
			
			static void FromMultibootSymbolTable ( PageTable * MemoryDomain, LiveImage ** ImagePointer, multiboot_info_t * MultibootInfo, ElfStatus_t * Status );
			static void LoadFromKernelMemory ( PageTable * MemoryDomain, LiveImage ** ImagePointer, void * Memory, uint32_t Bounds, ElfStatus_t * Status );
			static void LoadFromFile ( PageTable * MemoryDomain, LiveImage ** ImagePointer, const char * ObjectFile );
			
		};
		
	};
	
};

#endif
