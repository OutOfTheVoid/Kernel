#ifndef CPPUTIL_LINKAGE_H
#define CPPUTIL_LINKAGE_H

// Defines a symbol as having C linkage (IE no name mangling)
#define C_LINKAGE extern "C"

// Defines a function symbol with C linkage using the cdecl ABI
#define ASM_LINKAGE C_LINKAGE __attribute__((__cdecl__))

#endif
