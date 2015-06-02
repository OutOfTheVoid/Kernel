#ifndef HW_PCI_CONFIG_H
#define HW_PCI_CONFIG_H

#include <hw/HW.h>
#include <hw/pci/PCI.h>

#include <stdint.h>

namespace HW
{
	
	namespace PCI
	{
		
		class Config
		{
		public:
			
			static const uint16_t kVendor_NonexistentDevice = 0xFFFF;
			
			uint32_t Read32 ( uint32_t Bus, uint32_t Device, uint32_t Function, uint32_t Offset );
			uint16_t Read16 ( uint32_t Bus, uint32_t Device, uint32_t Function, uint32_t Offset );
			uint8_t Read8 ( uint32_t Bus, uint32_t Device, uint32_t Function, uint32_t Offset );
			
			uint16_t CheckVendor ( uint32_t Bus, uint32_t Device );
			
		private:
			
			static const uint32_t kPort_Address = 0xCF8;
			static const uint32_t kPort_Data = 0xCFC;
			
		};
		
	};
	
};

#endif
