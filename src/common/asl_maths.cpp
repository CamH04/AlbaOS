#include <common/types.h>
#include <common/asl_maths.h>
#include <drivers/pit.h>

// (AlbaOS Standard Lib For Mathamatics)


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

uint64_t asl_maths::bootID(){ //cpu sig, vender hash, timestamp, LFSR combined
    uint32_t eax1, ebx1, ecx1, edx1;
    asm volatile("cpuid"
                 : "=a"(eax1), "=b"(ebx1), "=c"(ecx1), "=d"(edx1)
                 : "a"(1));
    uint32_t cpuSignature = eax1;

    uint32_t ebx0, ecx0, edx0, unused;
    asm volatile("cpuid"
                 : "=a"(unused), "=b"(ebx0), "=c"(ecx0), "=d"(edx0)
                 : "a"(0));
    uint32_t vendorHash = ebx0 ^ ecx0 ^ edx0;

    uint32_t tsc_low, tsc_high;
    asm volatile("rdtsc" : "=a"(tsc_low), "=d"(tsc_high));
    uint64_t tsc = ((uint64_t)tsc_high << 32) | tsc_low;

    PIT pit;
    uint16_t pitSeed = (uint16_t)pit.readCount();
    uint16_t lfsr = pitSeed;
    for (int i = 0; i < 16; ++i) {
        uint16_t lsb = lfsr & 1u;
        lfsr >>= 1;
        lfsr ^= (-lsb) & 0xB400u;
    }
    uint64_t bootID = ((uint64_t)cpuSignature << 32)
                    ^ ((uint64_t)vendorHash << 16)
                    ^ ((uint64_t)lfsr)
                    ^ tsc;

    return bootID;
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


uint32_t asl_maths::abs(int32_t x) {

	int32_t y = x >> 31;
	return ((x ^ y) - y);
}
double asl_maths::absD(double x) {

	if (x < 0.0) { return -x; }
	return x;
}
double asl_maths::factorial(double x) {
	if (x == 0) { return 1.0; }
	return x * (factorial(x - 1.0));
}
double asl_maths::sin(double x) {
	x = fmod(x, 2*pi);
	if (x < 0) { x = (2 * pi) - x; }
	int8_t sign = 1;
	if (x > pi) {
		x -= pi;
		sign = -1;
	}
	double result = x;
	double coefficient = 3.0;
	for (int i = 0; i < 10; i++) {
		double pow = power(x, coefficient);
		double frac = factorial(coefficient);
		if (i % 2 == 0) { result -= (pow/frac); }
		else { result += (pow/frac); }
		coefficient += 2.0;
	}
	return ((double)sign)*result;
}
double asl_maths::power(double x, double p) {

	double n = x;
	for (double i = 1.0; i < p; i += 1.0) { n *= x; }
	return n;
}
double asl_maths::fmod(double a, double b) {

	double frac = a / b;
	int floor = frac > 0 ? (int)frac : (int)(frac - 0.999999999999999);
	return (a - b * (double)floor);
}
double asl_maths::cos(double x) {

	return sin((pi / 2.0) - x);
}


//Line stuff ===================================================
uint16_t asl_maths::LineFillArray(int32_t x0, int32_t y0,
				int32_t x1, int32_t y1,
				struct point arr[]) {

	uint16_t index = 0;

	if (abs(y1 - y0) < abs(x1 - x0)) {

		if (x0 > x1) { LineFillLow(x1, y1, x0, y0, arr, index);
		} else {       LineFillLow(x0, y0, x1, y1, arr, index); }
	} else {

		if (y0 > y1) { LineFillHigh(x1, y1, x0, y0, arr, index);
		} else {       LineFillHigh(x0, y0, x1, y1, arr, index); }
	}

	return index;
}
void asl_maths::LineFillLow(int32_t x0, int32_t y0,
				int32_t x1, int32_t y1,
				struct point arr[], uint16_t &index) {

	int16_t dx = x1 - x0;
	int16_t dy = y1 - y0;
	int16_t yi = 1;

	if (dy < 0) {

		yi = -1;
		dy = -dy;
	}
	int16_t D = (2*dy) - dx;
	int16_t y = y0;

	for (int x = x0; x < x1; x++) {

		arr[index].x = x;
		arr[index].y = y;
		index++;

		if (D > 0) {

			y += yi;
			D += (2*(dy-dx));
		} else {

			D += 2*dy;
		}
	}
}
void asl_maths::LineFillHigh(int32_t x0, int32_t y0,
				int32_t x1, int32_t y1,
				struct point arr[], uint16_t &index) {

	int16_t dx = x1 - x0;
	int16_t dy = y1 - y0;
	int16_t xi = 1;

	if (dx < 0) {

		xi = -1;
		dx = -dx;
	}
	int16_t D = (2*dx) - dy;
	int16_t x = x0;

	for (int y = y0; y < y1; y++) {

		arr[index].x = x;
		arr[index].y = y;
		index++;

		if (D > 0) {

			x += xi;
			D += (2*(dx-dy));
		} else {

			D += 2*dx;
		}
	}
}
