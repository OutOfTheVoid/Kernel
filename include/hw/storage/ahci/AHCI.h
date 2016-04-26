#ifndef HW_STORAGE_AHCI_AHCI_H
#define HW_STORAGE_AHCI_AHCI_H

#include <hw/HW.h>
#include <hw/storage/Storage.h>

namespace HW
{
	
	namespace Storage
	{
		
		namespace AHCI
		{
			
			const uint16_t kPCIClass = 0x01;
			const uint16_t kPCISubClass = 0x06;
			
			void Init ();
			
		};
		
	};
	
};

#endif
