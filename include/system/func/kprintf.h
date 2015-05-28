#ifndef SYSTEM_FUNC_KPRINTF
#define SYSTEM_FUNC_KPRINTF

#include <stdarg.h>
#include <cpputil/linkage.h>

C_LINKAGE void system_func_kprintf ( const char * FormatString, ... );
C_LINKAGE void system_func_kprintfOutput ( void ( * Output ) ( const char *, va_list Args ) );

#endif
