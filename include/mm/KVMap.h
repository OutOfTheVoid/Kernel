#ifndef MM_KVMAP_H
#define MM_KVMAP_H

#include <stdint.h>

#include <cpputil/Linkage.h>

#include <mm/paging/PageTable.h>

C_LINKAGE void * mm_kvmap ( void * Physical, uint32_t Length, uint32_t Flags );
C_LINKAGE void mm_kvremap ( void * Virtual, void * Physical, uint32_t Flags );
C_LINKAGE void mm_kvunmap ( void * Virtual );

#endif
