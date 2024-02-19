#ifndef __ALBAOS__DRIVERS__PIT_H
#define __ALBAOS__DRIVERS__PIT_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>


namespace albaos {

	namespace drivers {

		class PIT : public Driver {
			public:
				hardwarecommunication::Port8Bit channel0;
				hardwarecommunication::Port8Bit channel1;
				hardwarecommunication::Port8Bit channel2;
				hardwarecommunication::Port8Bit commandPort;

			public:
				PIT();
				~PIT();

				common::uint32_t readCount();
				void setCount(common::uint32_t count);
		};
	}
}


#endif
