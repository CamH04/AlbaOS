#ifndef __ALBAOS__DRIVERS__CMOS_H
#define __ALBAOS__DRIVERS__CMOS_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>


namespace albaos {

	namespace drivers {

		class Cmos {

			public:
				hardwarecommunication::Port8Bit cmosAdress;
				hardwarecommunication::Port8Bit cmosData;

			public:
				Cmos();
				~Cmos();

                common::uint16_t GetRAMFromCMOS(bool print);


				common::uint8_t get_RTC_register(int reg);
				common::uint8_t ReadRTC(bool print);
				common::uint32_t get_update_in_progress_flag();
		};
	}
}


#endif
