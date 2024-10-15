
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
        };

        class acpi{
        public:
        };
    }

}

#endif
