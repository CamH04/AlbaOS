#ifndef __ALBAOS__DRIVERS__CMOS_H
#define __ALBAOS__DRIVERS__CMOS_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>


namespace albaos {

	namespace drivers {

		class Cmos {

			public:
				hardwarecommunication::Port8Bit lowmemLoc;
				hardwarecommunication::Port8Bit highmemLoc;

			public:
				Cmos();
				~Cmos();

                common::uint16_t GetRAMFromCMOS(bool print);
		};
	}
}


#endif
