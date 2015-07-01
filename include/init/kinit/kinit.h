#ifndef INIT_KERNEL_KINIT_H
#define INIT_KERNEL_KINIT_H

#include <stdint.h>
#include <stddef.h>
#include <cpputil/linkage.h>
#include <boot/multiboot/multiboot.h>

ASM_LINKAGE void init_kinit_kinit ( uint32_t Magic, multiboot_info_t * MultibootInfo );

#endif
