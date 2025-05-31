#include <hardwarecommunication/power.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <common/asl.h>
#include <hardwarecommunication/acpi.h>

#define NULL 0
#define SCI_EN_BIT     1
#define SLP_EN_BIT     (1 << 13)

using namespace albaos;
using namespace albaos::common;
using namespace albaos::hardwarecommunication;

asl ASLACLPI;

void printf(char* str);

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

struct RSDPtr {
    byte Signature[8];
    byte CheckSum;
    byte OemID[6];
    byte Revision;
    dword *RsdtAddress;
};

struct FACP {
    byte Signature[4];
    dword Length;
    byte reserved1[32];
    dword *DSDT;
    byte reserved2[4];
    dword *SMI_CMD;
    byte ACPI_ENABLE;
    byte ACPI_DISABLE;
    byte reserved3[9];
    dword *PM1a_CNT_BLK;
    dword *PM1b_CNT_BLK;
    byte reserved4[17];
    byte PM1_CNT_LEN;
};

unsigned int* acpi::acpiCheckRSDPtr(unsigned int* ptr) {
    const char* sig = "RSD PTR ";
    RSDPtr* rsdp = (RSDPtr*)ptr;

    if (ASLACLPI.memcmp(sig, rsdp, 8) == 0) {
        byte sum = 0;
        byte* bptr = (byte*)ptr;
        for (int i = 0; i < sizeof(RSDPtr); ++i)
            sum += bptr[i];

        if (sum == 0)
            return (unsigned int*)rsdp->RsdtAddress;
    }
    return NULL;
}

unsigned int* acpi::acpiGetRSDPtr(void) {
    unsigned int* addr;
    unsigned int* rsdp;

    for (addr = (unsigned int*)0x000E0000; (int)addr < 0x00100000; addr += 4) {
        rsdp = acpiCheckRSDPtr(addr);
        if (rsdp != NULL) {
            printf("Found a valid RSDP!\n");
            return rsdp;
        }
    }
    int ebda = *((short*)0x40E) << 4 & 0xFFFFF;
    for (addr = (unsigned int*)ebda; (int)addr < ebda + 1024; addr += 4) {
        rsdp = acpiCheckRSDPtr(addr);
        if (rsdp != NULL)
            return rsdp;
    }

    printf("No valid RSDP found.\n");
    return NULL;
}

int acpi::acpiCheckHeader(unsigned int* ptr, char* sig) {
    if (ASLACLPI.memcmp(ptr, sig, 4) != 0)
        return -1;

    int length = *(ptr + 1);
    char sum = 0;
    char* data = (char*)ptr;

    while (length-- > 0)
        sum += *data++;

    return (sum == 0) ? 0 : -1;
}

int acpi::acpiEnable(void) {
    printf("=== ACPI Enabling Debug ===\n");

    ASLACLPI.printfHex((unsigned int)SMI_CMD);       printf(" SMI_CMD\n");
    ASLACLPI.printfHex((unsigned int)ACPI_ENABLE);   printf(" ACPI_ENABLE\n");
    ASLACLPI.printfHex((unsigned int)PM1a_CNT);      printf(" PM1a_CNT\n");

    if ((ASLACLPI.inw((unsigned int)PM1a_CNT) & SCI_EN) != 0) {
        printf("ACPI is already enabled.\n");
        return 0;
    }

    if (SMI_CMD && ACPI_ENABLE) {
        printf("Sending ACPI_ENABLE to SMI_CMD...\n");
        ASLACLPI.outb((unsigned int)SMI_CMD, ACPI_ENABLE);

        for (int i = 0; i < 300; ++i) {
            if ((ASLACLPI.inw((unsigned int)PM1a_CNT) & SCI_EN) != 0) {
                printf("ACPI enabled successfully.\n");
                return 0;
            }
            ASLACLPI.sleep(10);
        }

        printf("ACPI enable timed out.\n");
        return -1;
    }

    printf("SMI_CMD or ACPI_ENABLE is invalid.\n");
    return -1;
}

int acpi::initAcpi(void) {
    unsigned int* ptr = acpiGetRSDPtr();
    if (!ptr) {
        printf("RSDP not found.\n");
        return -1;
    }

    if (acpiCheckHeader(ptr, "RSDT") != 0) {
        printf("RSDT header invalid.\n");
        return -1;
    }

    int entries = (*(ptr + 1) - 36) / 4;
    ptr += 36 / 4;

    for (int i = 0; i < entries; ++i, ++ptr) {
        if (acpiCheckHeader((unsigned int*)*ptr, "FACP") != 0)
            continue;

        FACP* facp = (FACP*)*ptr;

        if (acpiCheckHeader((unsigned int*)facp->DSDT, "DSDT") != 0) {
            printf("Invalid DSDT.\n");
            return -1;
        }

        char* S5Addr = (char*)facp->DSDT + 36;
        int dsdtLen = *((dword*)facp->DSDT + 1) - 36;

        while (dsdtLen-- > 0) {
            if (ASLACLPI.memcmp(S5Addr, "_S5_", 4) == 0)
                break;
            ++S5Addr;
        }

        if (dsdtLen <= 0) {
            printf("_S5 not present.\n");
            return -1;
        }

        if ((*(S5Addr - 1) != 0x08 &&
             (*(S5Addr - 2) != 0x08 || *(S5Addr - 1) != '\\')) ||
             *(S5Addr + 4) != 0x12) {
            printf("_S5 AML parse error.\n");
            return -1;
        }

        S5Addr += 5;
        S5Addr += ((*S5Addr & 0xC0) >> 6) + 2;

        if (*S5Addr == 0x0A) ++S5Addr;
        SLP_TYPa = *(S5Addr++) << 10;

        if (*S5Addr == 0x0A) ++S5Addr;
        SLP_TYPb = *(S5Addr++) << 10;

		SMI_CMD      = facp->SMI_CMD;
        ACPI_ENABLE  = facp->ACPI_ENABLE;
        ACPI_DISABLE = facp->ACPI_DISABLE;
        PM1a_CNT     = facp->PM1a_CNT_BLK;
        PM1b_CNT     = facp->PM1b_CNT_BLK;
        PM1_CNT_LEN  = facp->PM1_CNT_LEN;
        SLP_EN       = SLP_EN_BIT;
        SCI_EN       = SCI_EN_BIT;

        return 0;
    }

    printf("No valid FACP found.\n");
    return -1;
}

void acpi::acpiPowerOff(void) {
    if (!SCI_EN)
        return;

    if (acpiEnable() != 0) {
        printf("ACPI could not be enabled. Shutdown failed.\n");
        return;
    }

    ASLACLPI.outw((unsigned int)PM1a_CNT, SLP_TYPa | SLP_EN);
    if (PM1b_CNT)
        ASLACLPI.outw((unsigned int)PM1b_CNT, SLP_TYPb | SLP_EN);

    printf("Shutdown signal sent (ACPI).\n");
}
