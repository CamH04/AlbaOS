#include <hardwarecommunication/power.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <common/asl.h>



#define NULL 0

using namespace albaos;
using namespace albaos::common;
using namespace albaos::hardwarecommunication;

asl ASLPOWER;

void printf(char* str);

#define APM_INSTALLATION_CHECK 0x5300
#define APM_REAL_MODE_CONNECT  0x5301
#define APM_PROTECT_MODE_CONNECT 0x5303
#define APM_ENABLE_POWER_MGMT  0x5308
struct APMInfo {
    uint16_t version;
    uint16_t flags;
    uint16_t cseg;
    uint16_t offset;
    uint16_t cseg16;
    uint16_t dseg;
};
bool apm::poweroff() {
	printf("starting poweroff! \n");
    uint16_t ax = 0x5307;
    uint16_t bx = 0x0001;
	// power state 3= off 2=ssuspend 1=standby
    uint16_t cx = 0x0002;
    bool APM_error = false;
    asm volatile(
        "int $0x15\n\t"
        "setc %0\n\t"
        : "=r"(APM_error)
        : "a"(ax), "b"(bx), "c"(cx)
        : "memory", "cc"
    );
	printf("should be trurning off now! \n");

    return !APM_error;
}
//it was being an ass with embedded asm so i just chucked it into a seperate asm file
extern "C" {
    uint16_t apm_bios_call(uint16_t ax, uint16_t bx, uint16_t cx, uint16_t dx);
}
bool apm::init() {
    APMInfo info = {};
    uint16_t result, ax, bx, cx, dx;
    ax = APM_INSTALLATION_CHECK;
    bx = 0;
    result = apm_bios_call(ax, bx, 0, 0);
    if (result & 0x100) {
		printf("APM not supported!");
        return false;
    }
    info.version = result & 0xFFFF;
    info.flags = bx;
    ax = APM_PROTECT_MODE_CONNECT;
    bx = 0; // Device ID for APM BIOS
    result = apm_bios_call(ax, bx, 0, 0);
    if (result & 0x100) {
        printf("Failed to connect to protected mode interface \n");
		return false;
    }
    ax = APM_ENABLE_POWER_MGMT;
    bx = 0x0001;
    result = apm_bios_call(ax, bx, 0, 0);
	printf("APM init succesfull !!!!!!!!!!!!!!!!!!\n");
	//ASLPOWER.sleep(3000);
    return !(result & 0x100);
}
