#include <hw/pci/DeviceList.h>

Vector <HW::PCI::DeviceList :: PCIDevice> * HW::PCI::DeviceList :: Devices = NULL;

void HW::PCI::DeviceList :: Init ()
{
	
	
	
};

uint32_t HW::PCI::DeviceList :: NextWithClassPair ( uint32_t Start, uint16_t Class, uint16_t SubClass )
{
	
	for ( uint32_t I = Start; I < DeviceList -> Length (); I ++ )
	{
		
		PCIDevice * Dev = & ( * DeviceList ) [ I ];
		
		if ( ( Dev -> Class == Class ) && ( Dev -> SubClass == SubClass ) )
			return I;
			
	}
	
	return kDeviceIndex_Invalid;
	
};
