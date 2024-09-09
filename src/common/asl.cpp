#include <common/asl.h>
#include <drivers/pit.h>
#include <drivers/amd_am79c973.h>


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
//doesnt work idk why
void asl::shutdown() {
	Port8Bit resetPort(0x604);
	resetPort.Write(0x2000);

}

uint16_t asl::SetTextColor(bool set, uint16_t color) {

    static uint16_t newColor = 0x07;

    if (set) {
        newColor = color;
    }

    return newColor;
}



//RANDOM NUMBERS
//god help me random numbers are somthing else
//uses Lehmer random number generation
//Steve Park & Dave Geyer are legends btw read, their stuff
uint16_t asl::betterRandom() {
    //Linear-feedback shift register
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

double asl::Random(void) // betwwen 1 and 0
{
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
void asl::printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}
void asl::printfHex16(uint16_t key)
{
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}
void asl::printfHex32(uint32_t key)
{
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
