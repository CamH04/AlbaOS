#include <drivers/pit.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;


PIT::PIT()

: channel0(0x40),
  channel1(0x41),
  channel2(0x42),
  commandPort(0x43) {

}


PIT::~PIT() {
}


uint32_t PIT::readCount() {

	uint32_t count = 0;

	asm("cli");

	commandPort.Write(0b0000000);

	count = channel0.Read();
	count |= channel0.Read() << 8;

	asm("sti");
	return count;
}

void PIT::setCount(uint32_t count) {

	asm("cli");

	channel0.Write(count);
	channel0.Write(count >> 8);

	asm("sti");
}
