#include <drivers/cmos.h>
#include <common/asl.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;

asl ASLCMOS;

char* IntToString(uint32_t num);
void printf(char* str);

Cmos::Cmos()
: lowmemLoc(0x70),
  highmemLoc(0x70){

}
Cmos::~Cmos() {
}

uint16_t Cmos::GetRAMFromCMOS(bool print){
    uint16_t ram;
    uint8_t lowmem, highmem;

    lowmemLoc.Write(0x30);
    lowmem = lowmemLoc.Read();

    highmemLoc.Write(0x31);
    highmem = highmemLoc.Read();

    ram = lowmem | highmem << 8;

    if(print == true){
        printf(ASLCMOS.IntToString(ram));
        printf("kb");
        printf("\n");
    }
    return ram;
}
