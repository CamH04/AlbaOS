#include <common/asl.h>
#include <drivers/pit.h>
#include <hardwarecommunication/power.h>
#include <drivers/amd_am79c973.h>
#include <drivers/pit.h>

// most of this has been moved from kernel to here (AlbaOS Standard Lib)

//for random number gen
#define MODULUS    2147483647
#define MULTIPLIER 48271
#define CHECK      399268537
#define STREAMS    256
#define A256       22925


using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;

asl WOOPS;

//TODO move printf to here!!!!!!!!!!!!!!!!
void printf(char* str);
uint16_t hash(char* cmd);

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
char* asl::ArrayIntToString(unsigned int* arr) {
    int len = 0;
    while (arr[len] != -1) {
        len++;
    }
    char* result = new char[len * 12 + len + 1];  // geussung each integer takes up to 11 digits + space

    int idx = 0;
    for (int i = 0; i < len; i++) {
        int num = arr[i];
        char buffer[12];
        int pos = 0;
        if (num < 0) {
            buffer[pos++] = '-';
            num = -num;
        }
        int numPos = pos;
        do {
            buffer[pos++] = '0' + (num % 10);
            num /= 10;
        } while (num > 0);
        for (int j = numPos, k = pos - 1; j < k; j++, k--) {
            char temp = buffer[j];
            buffer[j] = buffer[k];
            buffer[k] = temp;
        }
        for (int j = 0; j < pos; j++) {
            result[idx++] = buffer[j];
        }
        if (i < len - 1) {
            result[idx++] = ' ';
        }
    }
    result[idx] = '\0';
    return result;
}

char* asl::CharPointerToString(const char* ptr) {
    if (ptr == nullptr) {
        return nullptr;
    }
    int length = 0;
    while (ptr[length] != '\0') { // until we hit the null terminator
        length++;
    }
    char* result = new char[length + 1];
    for (int i = 0; i < length; i++) {
        result[i] = ptr[i];
    }
    result[length] = '\0';
    return result;
}
char* asl::FloatToString(float number) {
    static char buffer[32];
    char* ptr = buffer;
    if (number < 0) {
        *ptr++ = '-';
        number = -number;
    }
    int integerPart = static_cast<int>(number);
    float fractionalPart = number - integerPart;
    char intBuffer[16];
    int i = 0;
    do {
        intBuffer[i++] = '0' + (integerPart % 10);
        integerPart /= 10;
    } while (integerPart > 0);
    while (i > 0) {
        *ptr++ = intBuffer[--i];
    }
    *ptr++ = '.';
    for (int j = 0; j < 6; ++j) {
        fractionalPart *= 10;
        int digit = static_cast<int>(fractionalPart);
        *ptr++ = '0' + digit;
        fractionalPart -= digit;
    }
    *ptr = '\0';
    return buffer;
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
    printf(IntToString(rdtsc()));
    printf("\n");
}
void asl::PrintCpuSpeed(){
    printf("CPU Speed: ");
    printf(IntToString((uint32_t)calculateClockSpeed() / 100000000));
    printf(".");
    /*i am sorry*/
    printf(IntToString(((uint32_t)calculateClockSpeed()%100000000)/10000000 ));
    printf(" GHz \n\n");
}

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

uint32_t asl::Trollfnv1a(char* str) {
	uint32_t hash = 0x811c9dc5;
	for (int i = 0; str[i] != '\0'; i++) {

		hash ^= str[i];
		hash *= 0x01000193;
	}
	//hash within sectors available on disk
	return (hash % 2048) + 1024;
}

uint16_t asl::hash(char* cmd) {
    uint32_t val = Trollfnv1a(cmd);
    return (val >> 16) ^ (val & 0xffff);
}



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
uint32_t asl::StringToInt(char* args){
    uint32_t number = 0;
    uint16_t i = 0;
    bool foundNum = false;

    for(uint16_t i = 0; args[i] != '\0'; i++){
        if((args[i] >= 58 || args[i] <= 47) && args[i] != ' '){
            return 0;
        }
        if(args[i] != ' '){
            number *= 10;
            number += ((uint32_t)args[i] -  48);
            foundNum = true;
            args[i] = ' ';
        }
        else{
            if(foundNum){
                return number;
            }
        }
    }
    return number;
}
char* asl::IntToString(uint32_t num) {
        uint32_t numChar = 1;
        uint8_t i = 1;
        if (num % 10 != num) {
                while ((num / (numChar)) >= 10) {
                        numChar *= 10;
                        i++;
                }
                char* str = "4294967296";
                uint8_t strIndex = 0;
                while (i) {
                        str[strIndex] = (char)(((num / (numChar)) % 10) + 48);
                        if (numChar >= 10) {

                                numChar /= 10;
                        }
                        strIndex++;
                        i--;
                }
                str[strIndex] = '\0';
                return str;
        }
        char* str = " ";
        str[0] = (num + 48);
        return str;
}


// UI  Prints =============================================
void asl::putcharTUI(unsigned char ch, unsigned char forecolor,
        unsigned char backcolor, uint8_t x, uint8_t y) {
    uint16_t attrib = (backcolor << 4) | (forecolor & 0x0f);
    volatile uint16_t* vidmem;
    vidmem = (volatile uint16_t*)0xb8000 + (80*y+x);
    *vidmem = ch | (attrib << 8);
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
    albaos::hardwarecommunication::acpi ACPI;
    ACPI.initAcpi();
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
    for (int i = 0; i < (strlen(args) + 1); i++) {
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


//RANDOM NUMBERS
//god help me random numbers are somthing else
//uses Lehmer random number generation
//Steve Park & Dave Geyer are legends btw read, their stuff
uint16_t asl::betterRandom() { //Linear-feedback shift register
	PIT pit;
	uint16_t seed = (uint16_t)pit.readCount();
	uint16_t lfsr = seed;
	uint16_t period = 0;

	do {
		uint16_t lsb = lfsr & 1u;
		lfsr >>= 1;
		lfsr ^= (-lsb) & 0xb400u;

		period++;

	} while (period < seed);


	return lfsr;
}

double asl::Random(void){ // betwwen 1 and 0
    PIT pit;
    static long seed[STREAMS] = {(uint16_t)pit.readCount()};
    static int  stream        = 0;
    const long Q = MODULUS / MULTIPLIER;
    const long R = MODULUS % MULTIPLIER;
            long t;
    t = MULTIPLIER * (seed[stream] % Q) - R * (seed[stream] / Q);
    if (t > 0)
        seed[stream] = t;
    else
        seed[stream] = t + MODULUS;
    return ((double) seed[stream] / MODULUS);
}

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
void asl::printfHex(uint8_t key){
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
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

//string manip functions
uint16_t asl::strlen(char* args) {
        uint16_t length = 0;
        for (length = 0; args[length] != '\0'; length++) {

        }
        return length;
}
