#ifndef BOOT_BOOTIMAGE_H
#define BOOT_BOOTIMAGE_H

#include <cpputil/Linkage.h>

/*
* BootImage.h
*
* Contains relevant information to the linking of the kernel.
*/

// Note that these are not actualy integers, but just symbols, and should only be referenced for their address.

// Beginning of the kernel text and data (physical).
C_LINKAGE uint32_t __kbegin;

// End of the kernel text and data (physical).
C_LINKAGE uint32_t __kend;

// Bottom and top of BSP kernel stack
C_LINKAGE uint32_t boot_multiboot_StackBottom;
C_LINKAGE uint32_t boot_multiboot_StackTop;

#endif
