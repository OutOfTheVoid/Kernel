#ifndef HW_ACPI_AML_INTERPRETER_H
#define HW_ACPI_AML_INTERPRETER_H

#include <hw/HW.h>
#include <hw/acpi/ACPI.h>
#include <hw/acpi/aml/AML.h>

#include <stdint.h>

namespace HW
{
	
	namespace ACPI
	{
		
		namespace AML
		{
			
			class Interpreter
			{
			public:
				
				static void Init ();
				
			private:
				
				typedef struct
				{
				} InterpreterContext;
				
				static void ( * OpTable [ 0x100 ] ) ( InterpreterContext * Context );
				static void ( * ExtOpTable [ 0x100 ] ) ( InterpreterContext * Context );
				
				static void IllegalOp ( InterpreterContext * Context );
				
			};
			
		};
		
	};
	
};

#endif
