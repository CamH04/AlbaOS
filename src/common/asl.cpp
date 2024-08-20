#include <common/asl.h>
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


void cprintf(char* str, uint8_t forecolor, uint8_t backcolor, uint8_t x, uint8_t y) {
    asl WOOPS;
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
