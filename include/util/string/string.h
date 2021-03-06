#ifndef UTIL_STRING_CSTRING_H
#define UTIL_STRING_CSTRING_H

#include <cpputil/linkage.h>
#include <stdint.h>
#include <stddef.h>

C_LINKAGE size_t strlen ( const char * Str );
C_LINKAGE size_t strcmp ( const char * Str1, const char * Str2 );
C_LINKAGE int32_t strncmp ( const char * Str1, const char * Str2, size_t Count );
C_LINKAGE char * strcpy ( char * Destination, const char * Source );
C_LINKAGE size_t strcspn ( const char * Source, const char * Search );

C_LINKAGE void atoi ( char * Str, int32_t * Value );
C_LINKAGE const char * itoa ( int32_t Value, char * Str, int32_t Base );
C_LINKAGE const char * utoa ( uint32_t Value, char * Str, int32_t Base );

C_LINKAGE void * memcpy ( void * Destination, const void * Source, size_t Count );

C_LINKAGE void memzero ( void * Destination, size_t Count );

#endif
