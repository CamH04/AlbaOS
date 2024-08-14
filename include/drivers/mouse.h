#ifndef __ALBAOS__DRIVERS__MOUSE_H
#define __ALBAOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>

namespace albaos
{
    namespace drivers
    {

        class MouseEventHandler
        {
        public:
            MouseEventHandler();

            virtual void OnActivate();
            virtual void OnMouseDown(albaos::common::uint8_t button);
            virtual void OnMouseUp(albaos::common::uint8_t button);
            virtual void OnMouseMove(int x, int y);
        };


        class MouseDriver : public albaos::hardwarecommunication::InterruptHandler, public Driver
        {
            albaos::hardwarecommunication::Port8Bit dataport;
            albaos::hardwarecommunication::Port8Bit commandport;
            albaos::common::uint8_t buffer[3];
            albaos::common::uint8_t offset;
            albaos::common::uint8_t buttons;

            MouseEventHandler* handler;
        public:
            MouseDriver(albaos::hardwarecommunication::InterruptManager* manager, MouseEventHandler* handler);
            ~MouseDriver();
            virtual albaos::common::uint32_t HandleInterrupt(albaos::common::uint32_t esp);
            virtual void Activate();
        };

    }
}

#endif
