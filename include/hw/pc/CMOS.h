#ifndef HW_PC_CMOS_H
#define HW_PC_CMOS_H

#include <hw/pc/PC.h>

#include <stdint.h>

namespace HW
{
	
	
	namespace PC
	{
		
		
		class CMOS
		{
		public:
			
			static const uint32_t kRegister_RTC_Seconds = 0x00;
			static const uint32_t kRegister_RTCAlarm_Seconds = 0x01;
			static const uint32_t kRegister_RTC_Minutes = 0x02;
			static const uint32_t kRegister_RTCAlarm_Minutes = 0x03;
			static const uint32_t kRegister_RTC_Hours = 0x04;
			static const uint32_t kRegister_RTCAlarm_Hours = 0x05;
			static const uint32_t kRegister_RTC_DayOfWeek = 0x06;
			static const uint32_t kRegister_RTC_DateDay= 0x07;
			static const uint32_t kRegister_RTC_DateMonth= 0x08;
			static const uint32_t kRegister_RTC_DateYear= 0x09;
			
			static const uint32_t kRegister_RTCStatus_A= 0x0A;
			static const uint32_t kRegister_RTCStatus_B= 0x0B;
			
			static const uint32_t kRegister_System_Status = 0x0C;
			static const uint32_t kRegister_System_CMOSRamStatus = 0x0D;
			static const uint32_t kRegister_System_DiagnosticStatus = 0x0E;
			
			static const uint32_t kRegister_System_ShutdownCode = 0x0F;
			
			static const uint8_t kShutdownCode_JumpNoEOI = 0x0A;
		
			static void Write ( uint8_t Register, uint8_t Value );
			static uint8_t Read ( uint8_t Register );
			
		private:
			
			static const uint16_t kPort_Index = 0x70;
			static const uint16_t kPort_Data = 0x71;
			
		};
		
	};
	
};

#endif
