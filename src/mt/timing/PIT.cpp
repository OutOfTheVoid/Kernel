#include <mt/timing/PIT.h>
#include <hw/cpu/IO.h>

#include <system/func/KPrintF.h>

void MT::Timing::PIT :: Init ()
{
	
	SetTimer ( 50.0 );
	
	
};

void MT::Timing::PIT :: SetTimeoutMS ( double Time )
{
	
	if ( Time <= 0 )
		return;
	
	uint32_t Division = 1193182.0 * ( Time / 1000.0 );
	uint32_t IntegerDivision = static_cast <uint32_t> ( Division );
	uint32_t Command = kCommandFlag_Timer0 | kCommandFlag_CountdownMode | kCommandFlag_WordAccessMode;
	
	if ( IntegerDivision >= 0x10000 )
	{
		
		system_func_kprintf ( "Warning: PIT::SetTimeoutMS () value of %i MS is too much and will be trnucated to ~55 MS!\n", static_cast <uint32_t> ( Time ) );
		
		IntegerDivision = 0xFFFF;
		
	}
	
};

void MT::Timing::PIT :: SetTimer ( double Frequencey )
{
	
	if ( Frequencey > 1193182.0 )
	{
		
		system_func_kprintf ( "Warning: PIT::SetTimer () value of %i Hz is too high and will be truncated to 1193182 Hz!\n" );
		
		Frequencey = 1193182;
		
	}
	
	if ( Frequencey <= 0 )
		return;
	
	if ( Frequencey < 18.0 )
	{
		
		system_func_kprintf ( "Warning: PIT::SetTimer () value of %i Hz is too low and will be truncated to 1193182 Hz!\n" );
		
		Frequencey = 1193182;
		
	}
	
	double Divider = 1193182.0 / Frequencey;
	uint32_t IntegerDivider = static_cast <uint32_t> ( Divider );
	uint32_t Command = kCommandFlag_Timer0 | kCommandFlag_WordAccessMode;
	 
	IO :: Out8 ( kCommandPort, Command );
	IO :: Out8 ( kDataPort_Channel0,  IntegerDivider & 0xFF );
	IO :: Out8 ( kDataPort_Channel0, ( IntegerDivider >> 8 ) & 0xFF ); 
	
};

bool MT::Timing::PIT :: GetTimeout ()
{
	
	
	
};

void MT::Timing::PIT :: Disable ()
{
	
	
	
};

