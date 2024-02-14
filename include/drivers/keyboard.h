
#ifndef __ALBAOS__DRIVERS__KEYBOARD_H
#define __ALBAOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>


namespace albaos {

	namespace drivers {

		class KeyboardEventHandler {
			public:
				bool ctrl;
				bool alt;
				bool shift;
				bool caps;

				bool cli;

			public:
				KeyboardEventHandler();
				virtual void OnKeyDown(char);
				virtual void OnKeyUp();
				virtual void resetMode();
				virtual void modeSet(albaos::common::uint8_t);
		};


		class KeyboardDriver : public albaos::hardwarecommunication::InterruptHandler, public Driver {


			public:
				albaos::hardwarecommunication::Port8Bit dataport;
				albaos::hardwarecommunication::Port8Bit commandport;

				KeyboardEventHandler* handler;

			public:

				albaos::common::uint8_t keyHex = 0xff;

				KeyboardDriver(albaos::hardwarecommunication::InterruptManager* manager, KeyboardEventHandler *handler);
				~KeyboardDriver();

				virtual albaos::common::uint32_t HandleInterrupt(albaos::common::uint32_t esp);
				virtual void Activate();
		};

	}
}


#endif
