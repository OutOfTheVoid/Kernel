#ifndef HW_PCI_DEVICELIST_H
#define HW_PCI_DEVICELIST_H

#include <hw/HW.h>
#include <hw/pci/PCI.h>

#include <stdint.h>

#include <util/Vector.h>

namespace HW
{
	
	namespace PCI
	{
		
		class DeviceList
		{
		public:
			
			static const uint32_t kDeviceIndex_Invalid = 0xFFFFFFFF;
			
			static void Init ();
			
			static uint32_t NextWithClassPair ( uint32_t Start, uint16_t Class, uint16_t SubClass );
			
		private:
			
			typedef struct
			{
				
				uint32_t Bus;
				uint32_t Device;
				
				uint16_t Vendor;
				
				uint16_t Class;
				uint16_t SubClass;
				
			} PCIDevice;
			
			Vector <PCIDevice> * Devices;
			
		};
		
	};
	
};

#endif
