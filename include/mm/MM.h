#ifndef MM_MM_H
#define MM_MM_H

#include <boot/multiboot/Multiboot.h>

#include <stdint.h>

namespace MM
{
	
	static const uint32_t KernelVMStart = 0x00000000;
	static const uint32_t KernelVMLength = 0x10000000;
	static const uint32_t UserVMStart = 0x10000000;
	static const uint32_t UserVMLength = 0xE0000000;
	
	void Init ( multiboot_info_t * MultibootInfo );
	
};

#endif
