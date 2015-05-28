#ifndef SYSTEM_FUNC_PANIC_H
#define SYSTEM_FUNC_PANIC_H

#include <stdarg.h>
#include <stddef.h>

#include <cpputil/linkage.h>

#define __KPANIC_STRINGIFY(X) #X
#define __KPANIC_TOSTRING(X) __KPANIC_STRINGIFY(X)

#define KPANIC( x, ... ) system_func_panic ( __FILE__", "__KPANIC_TOSTRING ( __LINE__ )": "x, ## __VA_ARGS__ )

C_LINKAGE void system_func_panic ( const char * FormatString = NULL, ... );

#endif
