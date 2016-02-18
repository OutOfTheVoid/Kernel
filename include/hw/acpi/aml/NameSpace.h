#ifndef HW_ACPI_AML_NAMESPACE_H
#define HW_ACPI_AML_NAMESPACE_H

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>
#include <hw/acpi/aml/AML.h>

namespace HW
{
	
	namespace ACPI
	{
		
		namespace AML
		{
			
			class NameSpace
			{
			public:
				
				static void Init ( uint32_t * Status );
				
			private:
				
				typedef struct Package_Struct
				{
					
					char Name [ 4 ];
					
					Package_Struct * Parent;
					
					
				} Package;
				
			};
			
		};
		
	};
	
};

#include <stdint.h>



#endif
