#include <drivers/cmos.h>
#include <common/asl.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;

asl ASLCMOS;

void printf(char* str);

Cmos::Cmos()
: cmosAdress(0x70),
  cmosData(0x71){

}
Cmos::~Cmos() {
}

uint16_t Cmos::GetRAMFromCMOS(bool print){
    uint16_t ram;
    uint8_t highmem,lowmem;

    cmosAdress.Write(0x30);
    lowmem = cmosAdress.Read();

    cmosAdress.Write(0x31);
    highmem = cmosAdress.Read();

    ram = lowmem | highmem << 8;

    if(print == true){
        printf(ASLCMOS.IntToString(ram));
        printf("kb");
        printf("\n");
    }
    return ram;
}


uint8_t Cmos::get_RTC_register(int reg) {
      cmosAdress.Write(reg);
      return cmosData.Read();
}
uint8_t Cmos::GetRTCFromCMOS(bool print) {
    uint8_t second,minute,hour,day,month,year;
    second = get_RTC_register(0x00);
    minute = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);
    day = get_RTC_register(0x07);
    month = get_RTC_register(0x08);
    year = get_RTC_register(0x09);

    if(print == true){
        printf("Time: ");
        printf(ASLCMOS.IntToString(hour));
        printf(":");
        printf(ASLCMOS.IntToString(minute));
        printf("\n");
        printf("Day: ");
        printf(ASLCMOS.IntToString(day));
        printf(" Month: ");
        printf(ASLCMOS.IntToString(month));
        printf(" Year: ");
        printf(ASLCMOS.IntToString(year));
        printf("\n");
    }
    return minute,second;
}
