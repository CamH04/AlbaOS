#include <common/asl.h>
#include <common/asl_string.h>
#include <drivers/pit.h>
#include <hardwarecommunication/acpi.h>
#include <hardwarecommunication/power.h>
#include <drivers/amd_am79c973.h>
#include <drivers/pit.h>

// most of this has been moved from kernel to here (AlbaOS Standard Lib)

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;

asl WOOPS;
asl_string WOOPSAGAIN;

void printf(char* str);


bool asl::cpuSupportsTSC() {
    uint32_t eax, edx;
    asm volatile("cpuid" : "=d"(edx) : "a"(1) : "ebx", "ecx");
    return (edx & (1 << 4)) != 0;
}

//from little to big endien as internet is written in big endien
uint16_t asl::SwitchEndian16Bit(uint16_t inp) {
    return (inp>>8) | (inp<<8);
}
uint32_t asl::SwitchEndian32Bit(uint32_t inp) {
    return ((inp>>24)&0xff)      |
            ((inp<<8)&0xff0000)   |
            ((inp>>8)&0xff00)     |
            ((inp<<24)&0xff000000);
}
uint64_t asl::GetTicks() {
    PIT pit;
    pit.setCount(1193182/1000);
    return (double)(pit.readCount());
}

//time since ast cpu reset
uint64_t asl::rdtsc(void){
    uint32_t low, high;
    asm volatile("rdtsc":"=a"(low),"=d"(high));
    return ((uint64_t)high << 32) | low;
}
double asl::calculateClockSpeed() {
    uint64_t start, end;
    start = rdtsc();
    sleep(1000);
    end = rdtsc();
    return (double)(end - start); //delay is 1 second, cycles = Hz
}
void asl::benchmark(){
    printf(WOOPSAGAIN.IntToString(rdtsc()));
    printf("\n");
}
void asl::PrintCpuSpeed(){
    uint32_t hz = (uint32_t)calculateClockSpeed();
    uint32_t ghz_whole = hz / 1000000000;
    uint32_t ghz_frac  = (hz % 1000000000) / 100000000;  // first d.p
    printf("CPU Speed: ");
    printf(WOOPSAGAIN.IntToString(ghz_whole));
    printf(".");
    printf(WOOPSAGAIN.IntToString(ghz_frac));
    printf(" GHz\n\n");
}

//ASL_MEM=======================================================
uint16_t asl::inw (unsigned short int __port){
  unsigned short _v;
  __asm__ __volatile__ ("inw %w1,%0":"=a" (_v):"Nd" (__port));
  return _v;
}
void asl::outb (unsigned char __value, unsigned short int __port){
  __asm__ __volatile__ ("outb %b0,%w1": :"a" (__value), "Nd" (__port));
}
void asl::outw (unsigned short int __value, unsigned short int __port){
  __asm__ __volatile__ ("outw %w0,%w1": :"a" (__value), "Nd" (__port));
}


int asl::memcmp(const void *s1, const void *s2, size_t n){
    size_t i;
    uint8_t *p1 = (uint8_t *)s1, *p2 = (uint8_t *)s2;
    for (i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] > p2[i] ? 1 : -1;
        }
    }
    return 0;
}

void asl::memmove(void *dest, void *src, size_t n){
    char *csrc = (char *)src;
    char *cdest = (char *)dest;
    char *temp = new char[n];
    for (int i=0; i<n; i++)
        temp[i] = csrc[i];
    for (int i=0; i<n; i++)
        cdest[i] = temp[i];
    delete [] temp;
}

void asl::memcpy(void *dest, void *src, size_t n) {
    char *csrc = (char *)src;
    char *cdest = (char *)dest;
    for (int i=0; i<n; i++)
        cdest[i] = csrc[i];
}
void asl::memWrite(uint32_t memory, uint32_t inputVal) {
	volatile uint32_t* value;
	value = (volatile uint32_t*)memory;
	*value = inputVal;
}

uint32_t asl::memRead(uint32_t memory) {
	volatile uint32_t* value;
	value = (volatile uint32_t*)memory;

	return *value;
}
//======================================================

// UI  Prints =============================================
void asl::TUI(uint8_t forecolor, uint8_t backcolor,uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,bool shadow) {
    for (uint8_t y = 0; y < 25; y++) {
        for (uint8_t x = 0; x < 80; x++) {
            putcharTUI(0xff, 0x00, backcolor, x, y);
        }
    }
    uint8_t resetX = x1;
    while (y1 < y2) {
        while (x1 < x2) {
            putcharTUI(0xff, 0x00, forecolor, x1, y1);
            x1++;
        }
        y1++;
        //side shadow
        if (shadow) {
            putcharTUI(0xff, 0x00, 0x00, x1, y1);
        }
        x1 = resetX;
    }
    //bottom shadow
    if (shadow) {
        for (resetX++; resetX < (x2 + 1); resetX++) {

            putcharTUI(0xff, 0x00, 0x00, resetX, y1);
        }
    }
}


void asl::printfTUI(char* str, uint8_t forecolor, uint8_t backcolor, uint8_t x, uint8_t y) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            y++;
            x = 0;
        } else {
            putcharTUI(str[i], forecolor, backcolor, x, y);
            x++;
        }
        if (x >= 80) {
            y++;
            x = 0;
        }
        if (y >= 25) {
            y = 0;
        }
    }
}
void asl::putcharTUI(unsigned char ch, unsigned char forecolor,
        unsigned char backcolor, uint8_t x, uint8_t y) {
    uint16_t attrib = (backcolor << 4) | (forecolor & 0x0f);
    volatile uint16_t* vidmem;
    vidmem = (volatile uint16_t*)0xb8000 + (80*y+x);
    *vidmem = ch | (attrib << 8);
}
//==========================================================================

//sleeps zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
void asl::sleep(uint32_t ms) {//like arduino (ms)
    PIT pit;
    for (uint32_t i = 0; i < ms; i++) {
        pit.setCount(1193182/1000);
        uint32_t start = pit.readCount();
        while ((start - pit.readCount()) < 1000) {}
    }
}
uint8_t asl::Web2EGA(uint32_t colour) {
    uint8_t bytes[3];
    bytes[2] = colour >> 16;
    bytes[1] = (colour >> 8) & 0xff;
    bytes[0] = colour & 0xff;
    uint8_t result = 0;
    for (int i = 0; i < 3; i++) {
        if(bytes[i] < 0x2b){
            bytes[i] = 0x00;
        }
        else if (bytes[i] < 0x80) {
            bytes[i] = 0x55;
        }
        else if (bytes[i] < 0xd5) {
            bytes[i] = 0xaa;
        }
        else{
            bytes[i] = 0xff;
        }
    }
    for (int i = 0; i < 3; i++) {
        switch (bytes[i]) {
            case 0xff:
                result |= (1 << (i+3));
                result |= (1 << i);
                break;
            case 0x55: result |= (1 << (i+3)); break;
            case 0xaa: result |= (1 << i); break;
            default: break;
        }
    }
    return result;
}
void asl::reboot() {
	asm volatile ("cli");
	uint8_t read = 0x02;
	Port8Bit resetPort(0x64);
	while (read & 0x02) {
		read = resetPort.Read();
	}
	resetPort.Write(0xfe);
	asm volatile ("hlt");
}
void asl::shutdown() {
    acpi ACPI;
    //ACPI.initAcpi();
    ACPI.acpiPowerOff();
}

uint16_t asl::SetTextColor(bool set, uint16_t color) {
    static uint16_t newColor = 0x07;
    if (set) {
        newColor = color;
    }
    return newColor;
}


char* asl::argparse(char* args, uint8_t num) {
    char buffer[256];
    bool valid = false;
    uint8_t argIndex = 0;
    uint8_t bufferIndex = 0;
    for (int i = 0; i < (WOOPSAGAIN.strlen(args) + 1); i++) {
        if (args[i] == ' ' || args[i] == '\0') {
            if (valid) {
                if (argIndex == num) {
                    buffer[bufferIndex] = '\0';
                    char* arg = buffer;
                    return arg;
                }
                argIndex++;
            }
            valid = false;
        } else {
            if (argIndex == num) {

                buffer[bufferIndex] = args[i];
                bufferIndex++;
            }
            valid = true;
        }
    }
    return "how";
}

uint8_t asl::argcount(char* args) {
    uint8_t i = 0;
    char* foo = argparse(args, i);
    while (foo != "how") {
        foo = argparse(args, i);
        i++;
    }
    return i-1;
}
// MORE ALS_UI ================================================
/*
BLACK 0x00
BLUE 0x01
GREEN 0x02
CYAN 0x03
RED 0x04
MAGENTA 0x05
ORANGE 0x06
WHITE 0x07
GRAY 0x08
BLUELIGHT 0x09
GREENLIGHT 0x0A
CYANLIGHT 0x0B
PINK 0x0C
MAGENTALIGHT 0x0D
YELLOW 0x0E
WHITELIGHT 0x0F
*/
void asl::putchar(unsigned char ch, unsigned char forecolor,unsigned char backcolor, uint8_t x, uint8_t y) {
    uint16_t attrib = (backcolor << 4) | (forecolor & 0x0f);
    volatile uint16_t* VideoMemory;
    VideoMemory = (volatile uint16_t*)0xb8000 + (80*y+x);
    *VideoMemory = ch | (attrib << 8);
}


void asl::cprintf(char* str, uint8_t forecolor, uint8_t backcolor, uint8_t x, uint8_t y) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            y++;
            x = 0;
        } else {
            WOOPS.putchar(str[i], forecolor, backcolor, x, y);
            x++;
        }
        if (x >= 80) {
            y++;
            x = 0;
        }
        if (y >= 25) {
            y = 0;
        }
    }
}

void asl::printc(char c){
    static uint8_t x = 0, y = 0;
    uint16_t Ccolour = WOOPS.SetTextColor(false);
    volatile uint16_t* CVideoMemory;
    CVideoMemory = (volatile uint16_t *)0xb8000 + (y * 80 + x) ;
    *CVideoMemory = c | (Ccolour << 8);
}


void asl::printfhere(const char* str, uint8_t line) {
    for (uint16_t i = 0; str[i] != '\0'; i++) {
        volatile uint16_t* VideoMemory = (volatile uint16_t*)0xb8000 + (80*line+i);
        *VideoMemory = str[i] | 0x700;
    }
}
//  ================================================

char* asl::printfHex(uint8_t key){
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
    return foo;
}
void asl::printfHex16(uint16_t key){
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}
void asl::printfHex32(uint32_t key){
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}

