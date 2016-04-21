#ifndef MM_MM_H
#define MM_MM_H

#include <boot/multiboot/Multiboot.h>

#include <stdint.h>

namespace MM
{
	
	// Note: On x86, kernel VM lengths and starts must be aligned by 0x400000 for efficient mapping into user space.
	
	static const uint32_t kKernelVM_Start = 0x00000000;
	static const uint32_t kKernelVM_Length = 0x10000000;
	static const uint32_t kUserVM_Start = 0x10000000;
	static const uint32_t kUserVM_Length = 0xE0000000;
	
	void Init ( multiboot_info_t * MultibootInfo, multiboot_info_t ** NewMultibootInfo );
	
};

#endif
