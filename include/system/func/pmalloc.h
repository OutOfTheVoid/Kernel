#ifndef SYSTEM_FUNC_PMALLOC_H
#define SYSTEM_FUNC_PMALLOC_H

#include <stddef.h>
#include <stdint.h>

#include <cpputil/linkage.h>

#define PMALLOC_PHSCHEME_DEFAULT 0
#define PMALLOC_PHSCHEME_BLOCK 1
#define PMALLOC_PHSCHEME_BINARY 2
#define PMALLOC_PHSCHEME_DISTRIBUTE 3

void * system_func_pmalloc ( uint32_t Pages, uint32_t PTFlags = 0, uint32_t Hint = PMALLOC_PHSCHEME_DEFAULT );
void system_func_pfree ( void * Base );

#endif
