#ifndef HW_ACPI_AML_GLOBALLOCK_H
#define HW_ACPI_AML_GLOBALLOCK_H

#include <stdint.h>

#define HW_ACPI_AML_GLOBALLOCK_ACQUIRED - 1
#define HW_ACPI_AML_GLOBALLOCK_PENDING 0

void hw_acpi_aml_globallockinit ( uint32_t * Status );

int32_t hw_acpi_aml_globallockacquire ();
int32_t hw_acpi_aml_globallockrelease ();


#endif
