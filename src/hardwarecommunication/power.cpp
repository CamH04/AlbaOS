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
    uint16_t cx = 0x0003;
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













//ACPI ========================================================================
dword *SMI_CMD;
byte ACPI_ENABLE;
byte ACPI_DISABLE;
dword *PM1a_CNT;
dword *PM1b_CNT;
word SLP_TYPa;
word SLP_TYPb;
word SLP_EN;
word SCI_EN;
byte PM1_CNT_LEN;


struct RSDPtr
{
	byte Signature[8];
	byte CheckSum;
	byte OemID[6];
	byte Revision;
	dword *RsdtAddress;
};



struct FACP
{
	byte Signature[4];
	dword Length;
	byte unneded1[40 - 8];
	dword *DSDT;
	byte unneded2[48 - 44];
	dword *SMI_CMD;
	byte ACPI_ENABLE;
	byte ACPI_DISABLE;
	byte unneded3[64 - 54];
	dword *PM1a_CNT_BLK;
	dword *PM1b_CNT_BLK;
	byte unneded4[89 - 72];
	byte PM1_CNT_LEN;
};



// check if the given address has a valid header
unsigned int *acpi::acpiCheckRSDPtr(unsigned int *ptr)
{
	char *sig = "RSD PTR ";
	struct RSDPtr *rsdp = (struct RSDPtr *) ptr;
	byte *bptr;
	byte check = 0;
	int i;

	if (ASLPOWER.memcmp(sig, rsdp, 8) == 0)
	{
		// check checksum rsdpd
		bptr = (byte *) ptr;
		for (i=0; i<sizeof(struct RSDPtr); i++)
		{
			check += *bptr;
			bptr++;
		}

		// found valid rsdpd
		if (check == 0) {
			return (unsigned int *) rsdp->RsdtAddress;
		}
	}
	return NULL;
}



// finds the acpi header and returns the address of the rsdt
unsigned int *acpi::acpiGetRSDPtr(void) {
    unsigned int *addr;
    unsigned int *rsdp;
    for (addr = (unsigned int *) 0x000E0000; (int) addr < 0x00100000; addr += 0x10 / sizeof(addr)) {
        rsdp = acpiCheckRSDPtr(addr);
        if (rsdp != NULL) {
			printf("found a valid rsdp!\n");
            return rsdp;
        }
    }
    // At address 0x40:0x0E is the RM segment of the EBDA
    int ebda = *((short *) 0x40E);  // Get EBDA pointer
    ebda = ebda * 0x10 & 0x000FFFFF;  // Convert segment to linear address
    // Search the Extended BIOS Data Area (EBDA) for the Root System Description Pointer signature
    for (addr = (unsigned int *) ebda; (int) addr < ebda + 1024; addr += 0x10 / sizeof(addr)) {
        rsdp = acpiCheckRSDPtr(addr);
        if (rsdp != NULL) {
            return rsdp;
        }
    }
    printf("No valid RSDP found.\n");
    return NULL;
}

int acpi::acpiCheckHeader(unsigned int *ptr, char *sig){
	if (ASLPOWER.memcmp(ptr, sig, 4) == 0){
		char *checkPtr = (char *) ptr;
		int len = *(ptr + 1);
		char check = 0;
		while (0<len--){
			check += *checkPtr;
			checkPtr++;
		}
		if (check == 0)
			return 0;
	}
	return -1;
}



int acpi::acpiEnable(void){
	// check if acpi is enabled
	if ( (ASLPOWER.inw((unsigned int) PM1a_CNT) &SCI_EN) == 0 )
	{
		// check if acpi can be enabled
		if (SMI_CMD != 0 && ACPI_ENABLE != 0)
		{
			ASLPOWER.outb((unsigned int) SMI_CMD, ACPI_ENABLE); // send acpi enable command
			// give 3 seconds time to enable acpi
			int i;
			for (i=0; i<300; i++ )
			{
				if ( (ASLPOWER.inw((unsigned int) PM1a_CNT) &SCI_EN) == 1 )
					break;
				ASLPOWER.sleep(10);
			}
			if (PM1b_CNT != 0)
				for (; i<300; i++ )
				{
					if ( (ASLPOWER.inw((unsigned int) PM1b_CNT) &SCI_EN) == 1 )
						break;
					ASLPOWER.sleep(10);
				}
			if (i<300) {
				printf("enabled acpi.\n");
				return 0;
			} else {
				printf("PM1b_CNT != 0 check failed.");
				printf("couldn't enable acpi.\n");
				printf("shutdown command will not work!!!!!!!!\n");
				ASLPOWER.sleep(1000);
				return -1;
			}
		} else {
			printf("no known way to enable acpi.\n");
			return -1;
		}
	} else {
		printf("acpi was already enabled.\n");
		return 0;
	}
}
int acpi::initAcpi(void)
{
	unsigned int *ptr = acpiGetRSDPtr();
	if (ptr == NULL) {
		printf("RSDP not found. gibbing!!!!!!!!!!!!!!!!!!!\n");
		return -1;  // RSDP not found, return early.
	} else {
		printf("Detected RSDP at: ");
		//char* addressString = ASLPOWER.ArrayIntToString(ptr);
		//printf(addressString);
		printf("\n");
		//delete[] addressString;
	}
	if (ptr != NULL && acpiCheckHeader(ptr, "RSDT") == 0)
	{
		// the RSDT contains an unknown number of pointers to acpi tables
		int entrys = *(ptr + 1);
		entrys = (entrys-36) /4;
		ptr += 36/4;	// skip header information

		while (0<entrys--){
			// check if the desired table is reached
			if (acpiCheckHeader((unsigned int *) *ptr, "FACP") == 0)
			{
				entrys = -2;
				struct FACP *facp = (struct FACP *) *ptr;
				if (acpiCheckHeader((unsigned int *) facp->DSDT, "DSDT") == 0)
				{
					// search the \_S5 package in the DSDT
					char *S5Addr = (char *) facp->DSDT +36; // skip header
					printf("Detected S5 Adress at: ");
					printf(S5Addr);
					printf("\n");
					int dsdtLength = *(facp->DSDT+1) -36;
					while (0 < dsdtLength--)
					{
						if ( ASLPOWER.memcmp(S5Addr, "_S5_", 4) == 0)
							break;
						S5Addr++;
					}
					// check if \_S5 was found
					if (dsdtLength > 0)
					{
						// check for valid AML structure
						if ( ( *(S5Addr-1) == 0x08 || ( *(S5Addr-2) == 0x08 && *(S5Addr-1) == '\\') ) && *(S5Addr+4) == 0x12 )
						{
							S5Addr += 5;
							S5Addr += ((*S5Addr &0xC0)>>6) +2;	// calculate PkgLength size

							if (*S5Addr == 0x0A)
								S5Addr++;	// skip byteprefix
							SLP_TYPa = *(S5Addr)<<10;
							S5Addr++;

							if (*S5Addr == 0x0A)
								S5Addr++;	// skip byteprefix
							SLP_TYPb = *(S5Addr)<<10;

							SMI_CMD = facp->SMI_CMD;

							ACPI_ENABLE = facp->ACPI_ENABLE;
							ACPI_DISABLE = facp->ACPI_DISABLE;

							PM1a_CNT = facp->PM1a_CNT_BLK;
							PM1b_CNT = facp->PM1b_CNT_BLK;

							PM1_CNT_LEN = facp->PM1_CNT_LEN;

							SLP_EN = 1<<13;
							SCI_EN = 1;

							return 0;
						} else {
							printf("\\_S5 parse error.\n");
						}
					} else {
						printf("\\_S5 not present.\n");
					}
				} else {
					printf("DSDT invalid.\n");
				}
			}
			ptr++;
		}
		printf("no valid FACP present.\n");
	} else {
		printf("no acpi.\n");
	}

	return -1;
}



void acpi::acpiPowerOff(void)
{
	if (SCI_EN == 0)
		return;

	acpiEnable();

	// send the shutdown command
	ASLPOWER.outw((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN );
	if ( PM1b_CNT != 0 )
		ASLPOWER.outw((unsigned int) PM1b_CNT, SLP_TYPb | SLP_EN );

	printf("acpi poweroff failed.\n");
}
