
#ifndef __ALBAOS__HARDWARECOMMUNICATION__APM_H
#define __ALBAOS__HARDWARECOMMUNICATION__APM_H
#include <common/types.h>
#include <hardwarecommunication/port.h>

namespace albaos
{
    namespace hardwarecommunication
    {
        class apm{
        public:
            bool poweroff();
            bool init();
        };


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
