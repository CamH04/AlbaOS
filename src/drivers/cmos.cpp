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

//RTC CLOCK =================================================
#define CURRENT_YEAR 24


uint8_t second,minute,hour,day,month,year;
//set bu acpi table
int century_register = 0x00;

uint32_t Cmos::get_update_in_progress_flag(){
    cmosAdress.Write(0x0A);
    return (cmosData.Read() & 0x80);
}

uint8_t Cmos::get_RTC_register(int reg) {
      cmosAdress.Write(reg);
      return cmosData.Read();
}
uint8_t Cmos::ReadRTC(bool print) {
    unsigned char century;
    unsigned char last_second;
    unsigned char last_minute;
    unsigned char last_hour;
    unsigned char last_day;
    unsigned char last_month;
    unsigned char last_year;
    unsigned char last_century;
    unsigned char registerB;

    while (get_update_in_progress_flag());
    second = get_RTC_register(0x00);
    minute = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);
    day = get_RTC_register(0x07);
    month = get_RTC_register(0x08);
    year = get_RTC_register(0x09);
    if(century_register != 0) {
            century = get_RTC_register(century_register);
    }
    do {
            last_second = second;
            last_minute = minute;
            last_hour = hour;
            last_day = day;
            last_month = month;
            last_year = year;
            last_century = century;
            // making sure an update isnt happning, bcz it messess with the values
            while (get_update_in_progress_flag());
            second = get_RTC_register(0x00);
            minute = get_RTC_register(0x02);
            hour = get_RTC_register(0x04);
            day = get_RTC_register(0x07);
            month = get_RTC_register(0x08);
            year = get_RTC_register(0x09);
            if(century_register != 0) {
                  century = get_RTC_register(century_register);
            }
      }
      while( (last_second != second) || (last_minute != minute) || (last_hour != hour) || (last_day != day) || (last_month != month) || (last_year !=year) || (last_century != century) );
        registerB = get_RTC_register(0x0B);
      // Convert BCD to binary values
      if (!(registerB & 0x04)) {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);
            if(century_register != 0) {
                  century = (century & 0x0F) + ((century / 16) * 10);
            }
      }

      //12 to 24 hr
      if (!(registerB & 0x02) && (hour & 0x80)) {
            hour = ((hour & 0x7F) + 12) % 24;
      }
      if(century_register != 0) {
            year += century * 100;
      } else {
            year += (CURRENT_YEAR / 100) * 100;
            if(year < CURRENT_YEAR) year += 100;
      }
    if(print == true){
        printf("Time: ");
        printf(ASLCMOS.IntToString(hour + 1));
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
    return hour,minute,second;
}
