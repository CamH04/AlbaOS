#include <common/types.h>
#include <common/asl_maths.h>
#include <drivers/pit.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::drivers;
using namespace albaos::hardwarecommunication;

//for random number gen
#define MODULUS    2147483647
#define MULTIPLIER 48271
#define CHECK      399268537
#define STREAMS    256
#define A256       22925

uint16_t asl_maths::hash(char* cmd) {
    uint32_t val = Trollfnv1a(cmd);
    return (val >> 16) ^ (val & 0xffff);
}
uint32_t asl_maths::Trollfnv1a(char* str) {
	uint32_t hash = 0x811c9dc5;
	for (int i = 0; str[i] != '\0'; i++) {

		hash ^= str[i];
		hash *= 0x01000193;
	}
	//hash within sectors available on disk
	return (hash % 2048) + 1024;
}

//RANDOM NUMBERS
//god help me random numbers are somthing else
//uses Lehmer random number generation
//Steve Park & Dave Geyer are legends btw read, their stuff
uint16_t asl_maths::betterRandom() { //Linear-feedback shift register
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

double asl_maths::Random(void){ // betwwen 1 and 0
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
