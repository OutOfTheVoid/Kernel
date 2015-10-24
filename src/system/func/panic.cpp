#include <system/func/panic.h>

#include <hw/video/VText.h>

#include <hw/cpu/interrupt.h>
#include <hw/cpu/hang.h>

#include <mt/exception/MPException.h>

C_LINKAGE void system_func_panic ( const char * FormatString, ... )
{
	
	hw_cpu_cli ();
	
	MT::Exception::MPException :: Trigger ();
	
	HW::Video::VText :: Init ( HW::Video::VText :: MakeColor ( HW::Video::VText :: Color_Red, HW::Video::VText :: Color_White ) );
	
	if ( FormatString != NULL )
	{
		
		HW::Video::VText :: PrintF ( "KERNEL PANIC: \n================================================================================" );
		
		va_list Args;
		
		va_start ( Args, FormatString );
		HW::Video::VText :: PrintVAList ( FormatString, Args );
		va_end ( Args );
		
		HW::Video::VText :: PrintF ( "\n================================================================================System halted." );
		
	}
	else
		HW::Video::VText :: PrintF ( "KERNEL PANIC!\nSystem halted.\n" );
	
	hw_cpu_hang ();
	
};
