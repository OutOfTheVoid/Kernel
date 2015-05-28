#include <system/func/kprintf.h>

void ( * system_func_kprintfOutputFunction ) ( const char *, va_list Args );

C_LINKAGE void system_func_kprintfOutput ( void ( * Output ) ( const char *, va_list Args ) )
{
	
	system_func_kprintfOutputFunction = Output;
	
}

C_LINKAGE void system_func_kprintf ( const char * FormatString, ... )
{
	
	va_list Args;

	va_start ( Args, FormatString );
	( * system_func_kprintfOutputFunction ) ( FormatString, Args );
	va_end ( Args );
	
};
