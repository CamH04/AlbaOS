
#ifndef __ALBAOS__SYSCALLS_H
#define __ALBAOS__SYSCALLS_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <multitasking.h>

namespace albaos
{

    class SyscallHandler : public hardwarecommunication::InterruptHandler
    {

    public:
        SyscallHandler(hardwarecommunication::InterruptManager* interruptManager, albaos::common::uint8_t InterruptNumber);
        ~SyscallHandler();

        virtual albaos::common::uint32_t HandleInterrupt(albaos::common::uint32_t esp);

    };


}


#endif
