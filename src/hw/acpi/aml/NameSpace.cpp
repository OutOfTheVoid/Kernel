#include <hw/acpi/aml/NameSpace.h>
#include <mm/KMalloc.h>

HW::ACPI::AML :: DefinitionBlock HW::ACPI::AML::NameSpace :: RootDefBlock;
HW::ACPI::AML::NameSpace :: NameSpaceObject HW::ACPI::AML::NameSpace :: RootObject;

void HW::ACPI::AML::NameSpace :: Init ( uint32_t * Status )
{
	
	RootDefBlock.Name = "ACPI_System_Root";
	RootDefBlock.Type = kDefinitionBlockType_Root;
	
	RootDefBlock.Data = NULL;
	RootDefBlock.Length = 0;
	
	RootObject.DefBlock = & RootDefBlock;
	
	RootObject.Name [ 0 ] = '\\';
	RootObject.Name [ 1 ] = ' ';
	RootObject.Name [ 2 ] = ' ';
	RootObject.Name [ 3 ] = ' ';
	
	RootObject.Parent = NULL;
	
	* Status = kACPIStatus_Success;
	
};

HW::ACPI::AML::NameSpace :: TableHandle HW::ACPI::AML::NameSpace :: LoadTable ( DefinitionBlock * DefBlock, uint32_t * Status )
{
	
	
	
};