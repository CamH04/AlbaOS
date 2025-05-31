#ifndef __ALBAOS__HARDWARECOMMUNICATION__ACPI_H
#define __ALBAOS__HARDWARECOMMUNICATION__ACPI_H

#include <common/types.h>
#include <hardwarecommunication/port.h>

namespace albaos
{
    namespace hardwarecommunication
    {
        class acpi{
        public:
            unsigned int *acpiCheckRSDPtr(unsigned int *ptr);
            unsigned int *acpiGetRSDPtr(void);
            int acpiCheckHeader(unsigned int *ptr, char *sig);
            int acpiEnable(void);
            int initAcpi(void);
            void acpiPowerOff(void);

        };
    }

}

#endif
