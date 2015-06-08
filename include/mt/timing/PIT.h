#ifndef MT_TIMING_PIT_H
#define MT_TIMING_PIT_H

#include <mt/timing/Timing.h>

#include <stdint.h>

namespace MT
{
	
	namespace Timing
	{
		
		class PIT
		{
		public:
			
			static void Init ();
			
			static void SetTimeout ( double Time );
			static void SetTimer ( double Frequencey );
			
			static bool GetTimeout ();
			
			static void Disable ();
			
		private:
			
			static const uint32_t kDataPort_Channel0 = 0x40;
			static const uint32_t kDataPort_Channel1 = 0x41;
			static const uint32_t kDataPort_Channel2 = 0x42;
			
			static const uint32_t kCommandPort = 0x43;
			
			static const uint32_t kCommandFlag_Timer0 = 0x00;
			static const uint32_t kCommandFlag_Timer1 = 0x80;
			static const uint32_t kCommandFlag_Timer2 = 0x40;
			static const uint32_t kCommandFlag_ReadBack = 0xC0;
			
			static const uint32_t kCommandFlag_LatchAccessMode = 0x00;
			static const uint32_t kCommandFlag_LowByteAccessMode = 0x20;
			static const uint32_t kCommandFlag_HighByteAccessMode = 0x10;
			static const uint32_t kCommandFlag_WordAccessMode = 0x30;
			
			static const uint32_t kCommandFlag_CountdownMode = 0x00;
			static const uint32_t kCommandFlag_OneShotMode = 0x08;
			static const uint32_t kCommandFlag_RateGeneratorMode = 0x04;
			static const uint32_t kCommandFlag_SquareWaveMode = 0x0C;
			static const uint32_t kCommandFlag_SoftwareStrobe = 0x02;
			static const uint32_t kCommandFlag_HadwareStrobe = 0x0A;
			
		};
		
	};
	
};

#endif
