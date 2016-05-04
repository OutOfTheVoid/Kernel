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
				
				typedef uint32_t TableHandle;
				
				static const TableHandle kTableHandle_Invalid = 0xFFFFFFFF;
				
				static void Init ( uint32_t * Status );
				
				static TableHandle LoadTable ( DefinitionBlock * DefBlock, uint32_t * Status );
				
			private:
				
				friend class Interpreter;
				
				typedef struct NameSpaceObject_Struct
				{
					
					char Name [ 4 ];
					
					DefinitionBlock * DefBlock;
					
					NameSpaceObject_Struct * Parent;
					
				} NameSpaceObject;
				
				typedef struct Scope_Struct : public NameSpaceObject
				{
					
					
					
				} Scope;
				
				typedef struct ControlMethod_Struct : public NameSpaceObject
				{
					
					
				} ControlMethod;
				
				static DefinitionBlock RootDefBlock;
				static NameSpaceObject RootObject;
				
				static void ParseScope (  );
				
			};
			
		};
		
	};
	
};

#include <stdint.h>



#endif
