#include <mt/timing/PIT.h>
#include <mt/timing/PWaitMS.h>

#include <hw/cpu/IO.h>

#include <hw/pc/ISA.h>

#include <system/func/Panic.h>
#include <system/func/KPrintF.h>

#include <cpputil/Linkage.h>
 
 C_LINKAGE void mt_timing_waitms ( double MS );

void MT::Timing::PIT :: InitPWait ()
{
	
	mt_timing_pwaitmsMechanism ( & mt_timing_waitms );
	
};

void MT::Timing::PIT :: InitTimedIRQ ( void ( * InterruptHandler ) ( Interrupt::InterruptHandlers :: ISRFrame * ) )
{
	
	mt_timing_pwaitmsMechanism ( NULL );

	if ( ! ::HW::PC::ISA :: TryAllocateIRQ ( kISAIRQ_PIT ) )
		KPANIC ( "PIT IRQ Init failed: IRQ was already allocated!" );
	
	::HW::PC::ISA :: SetIRQHandler ( kISAIRQ_PIT, InterruptHandler );
	
};

void MT::Timing::PIT :: SetIRQEnabled ( bool Enabled )
{
	
	::HW::PC::ISA :: SetIRQEnabled ( kISAIRQ_PIT, Enabled );
	
};

void MT::Timing::PIT :: SetTimeoutMS ( double Time )
{
	
	if ( Time <= 0 )
		return;
	
	uint32_t Division = 1193182.0 * ( Time / 1000.0 );
	uint32_t IntegerDivision = static_cast <uint32_t> ( Division );
	uint32_t Command = kCommandFlag_Timer0 | kCommandFlag_CountdownMode | kCommandFlag_WordAccessMode;
	
	if ( IntegerDivision > 0xFFFF )
	{
		
		system_func_kprintf ( "Warning: PIT::SetTimeoutMS () value of %i MS is too much and will be trnucated to ~55 MS!\n", static_cast <uint32_t> ( Time ) );
		
		IntegerDivision = 0xFFFF;
		
	}
	
	::HW::CPU::IO :: Out8 ( kCommandPort, Command );
	::HW::CPU::IO :: Out8 ( kDataPort_Channel0,  IntegerDivision & 0xFF );
	::HW::CPU::IO :: Out8 ( kDataPort_Channel0, ( IntegerDivision >> 8 ) & 0xFF );
	
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
	
	double Divider = 1193182.0 / Frequencey;
	uint32_t IntegerDivider = static_cast <uint32_t> ( Divider );
	uint32_t Command = kCommandFlag_Timer0 | kCommandFlag_WordAccessMode;
	 
	 if ( IntegerDivider > 0xFFFF )
	{
		
		system_func_kprintf ( "Warning: PIT::SetTimer () value of %i Hz is too low and will be truncated to ~ 18 Hz!\n" );
		
		IntegerDivider = 0xFFFF;
		
	}
	 
	::HW::CPU::IO :: Out8 ( kCommandPort, Command );
	::HW::CPU::IO :: Out8 ( kDataPort_Channel0,  IntegerDivider & 0xFF );
	::HW::CPU::IO :: Out8 ( kDataPort_Channel0, ( IntegerDivider >> 8 ) & 0xFF ); 
	
};

bool MT::Timing::PIT :: GetTimeout ()
{
	
	uint32_t Command = kCommandFlag_ReadBack | kCommandFlag_NotLatchCount | kCommandFlag_ReadTimer0;
	
	::HW::CPU::IO :: Out8 ( kCommandPort, Command );
	return ( ::HW::CPU::IO :: In8 ( kDataPort_Channel0 ) & kStatusFlag_WaitState ) != 0;
	
};

void mt_timing_waitms ( double MS )
{
	
	if ( MS < 0 ) // Allow 0 for deadtime callibration.
		return;
	
	while ( MS > 50.0 )
	{
		
		MT::Timing::PIT :: SetTimeoutMS ( 50.0 );
		
		while ( ! MT::Timing::PIT :: GetTimeout () );
		
		MS -= 50.0;
		
	}
	
	MT::Timing::PIT :: SetTimeoutMS ( MS );
	
	while ( ! MT::Timing::PIT :: GetTimeout () );
	
};

