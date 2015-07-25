#ifndef MM_KMALLOC_H
#define MM_KMALLOC_H

#include <stddef.h>
#include <cpputil/Linkage.h>

C_LINKAGE void * mm_kmalloc ( size_t Length );
C_LINKAGE void * mm_krealloc ( void * Address, size_t Length );
C_LINKAGE void * mm_kcalloc ( size_t Count, size_t Length );
C_LINKAGE void mm_kfree ( void * Address );

void liballoc_switchlockmode ();

#endif
