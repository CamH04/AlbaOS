#include <cpuid.h>
#include <common/types.h>
#include <common/asl.h>

using namespace albaos;
using namespace albaos::common;

asl ASLCPUID;

int intelcpu(void);
void printregs(int eax, int ebx, int ecx, int edx);
void printf(char* str);

#define cpuid(in, a, b, c, d) __asm__("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

int cpuidentif::cpufind(void) {
    unsigned long ebx, unused;
    cpuid(0, unused, ebx, unused, unused);
    switch(ebx) {
        case 0x756e6547: // Intel Magic Code
        intelcpu();
        break;
        default:
        printf("Unknown x86 CPU Detected\n");
        break;
    }
    return 0;
}
//brands
char *Intel[] = {
    "Brand ID Not Supported.",
    "Intel(R) Celeron(R) processor",
    "Intel(R) Pentium(R) III processor",
    "Intel(R) Pentium(R) III Xeon(R) processor",
    "Intel(R) Pentium(R) III processor",
    "Reserved",
    "Mobile Intel(R) Pentium(R) III processor-M",
    "Mobile Intel(R) Celeron(R) processor",
    "Intel(R) Pentium(R) 4 processor",
    "Intel(R) Pentium(R) 4 processor",
    "Intel(R) Celeron(R) processor",
    "Intel(R) Xeon(R) Processor",
    "Intel(R) Xeon(R) processor MP",
    "Reserved",
    "Mobile Intel(R) Pentium(R) 4 processor-M",
    "Mobile Intel(R) Pentium(R) Celeron(R) processor",
    "Reserved",
    "Mobile Genuine Intel(R) processor",
    "Intel(R) Celeron(R) M processor",
    "Mobile Intel(R) Celeron(R) processor",
    "Intel(R) Celeron(R) processor",
    "Mobile Geniune Intel(R) processor",
    "Intel(R) Pentium(R) M processor",
    "Mobile Intel(R) Celeron(R) processor"
};

char *Intel_Other[] = {
    "Reserved",
    "Reserved",
    "Reserved",
    "Intel(R) Celeron(R) processor",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Intel(R) Xeon(R) processor MP",
    "Reserved",
    "Reserved",
    "Intel(R) Xeon(R) processor",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};
int cpuidentif::intelcpu(void) {
    printf("Intel Specific Features:\n");
    unsigned long eax, ebx, ecx, edx, max_eax, signature, unused;
    int model, family, type, brand, stepping, reserved;
    int extended_family = -1;
    cpuid(1, eax, ebx, unused, unused);
    model = (eax >> 4) & 0xf;
    family = (eax >> 8) & 0xf;
    type = (eax >> 12) & 0x3;
    brand = ebx & 0xff;
    stepping = eax & 0xf;
    reserved = eax >> 14;
    signature = eax;
    printf("Type - ");
    printf(ASLCPUID.IntToString(type));
    printf("\n");
    switch(type) {
        case 0:
        printf("Original OEM");
        break;
        case 1:
        printf("Overdrive");
        break;
        case 2:
        printf("Dual-capable");
        break;
        case 3:
        printf("Reserved");
        break;
    }
    printf("\n");
    printf("Family - ");
    printf(ASLCPUID.IntToString(family));
    printf("\n");
    switch(family) {
        case 3:
        printf("i386");
        break;
        case 4:
        printf("i486");
        break;
        case 5:
        printf("Pentium");
        break;
        case 6:
        printf("Pentium Pro");
        break;
        case 15:
        printf("Pentium 4");
    }
    printf("\n");
    if(family == 15) {
        extended_family = (eax >> 20) & 0xff;
        printf("Extended family ");
        printf(ASLCPUID.IntToString(extended_family));
        printf("\n");
    }
    printf("Model - ");
    printf(ASLCPUID.IntToString(model));
    printf("\n");
    switch(family) {
        case 3:
        break;
        case 4:
        switch(model) {
            case 0:
            case 1:
            printf("DX");
            break;
            case 2:
            printf("SX");
            break;
            case 3:
            printf("487/DX2");
            break;
            case 4:
            printf("SL");
            break;
            case 5:
            printf("SX2");
            break;
            case 7:
            printf("Write-back enhanced DX2");
            break;
            case 8:
            printf("DX4");
            break;
        }
        break;
        case 5:
        switch(model) {
            case 1:
            printf("60/66");
            break;
            case 2:
            printf("75-200");
            break;
            case 3:
            printf("for 486 system");
            break;
            case 4:
            printf("MMX");
            break;
        }
        break;
        case 6:
        switch(model) {
            case 1:
            printf("Pentium Pro");
            break;
            case 3:
            printf("Pentium II Model 3");
            break;
            case 5:
            printf("Pentium II Model 5/Xeon/Celeron");
            break;
            case 6:
            printf("Celeron");
            break;
            case 7:
            printf("Pentium III/Pentium III Xeon - external L2 cache");
            break;
            case 8:
            printf("Pentium III/Pentium III Xeon - internal L2 cache");
            break;
        }
        break;
        case 15:
        break;
    }
    printf("\n");
    cpuid(0x80000000, max_eax, unused, unused, unused);
    if(max_eax >= 0x80000004) {
        printf("Brand: ");
        if(max_eax >= 0x80000002) {
            cpuid(0x80000002, eax, ebx, ecx, edx);
            printregs(eax, ebx, ecx, edx);
        }
        if(max_eax >= 0x80000003) {
            cpuid(0x80000003, eax, ebx, ecx, edx);
            printregs(eax, ebx, ecx, edx);
        }
        if(max_eax >= 0x80000004) {
            cpuid(0x80000004, eax, ebx, ecx, edx);
            printregs(eax, ebx, ecx, edx);
        }
        printf("\n");
    } else if(brand > 0) {
        printf("Brand - ");
        printf(ASLCPUID.IntToString(brand));
        printf("\n");
        if(brand < 0x18) {
            if(signature == 0x000006B1 || signature == 0x00000F13) {
                printf(Intel_Other[brand]);
            } else {
                printf(Intel[brand]);
                printf("\n");
            }
        } else {
            printf("Reserved\n");
        }
    }
    printf("Stepping: ");
    printf(ASLCPUID.IntToString(stepping));
    printf("Reserved: ");
    printf(ASLCPUID.IntToString(reserved));
    printf("\n");
    return 0;
}

/* Print Registers */
void cpuidentif::printregs(int eax, int ebx, int ecx, int edx) {
    int j;
    char string[17];
    string[16] = '\0';
    for(j = 0; j < 4; j++) {
        string[j] = eax >> (8 * j);
        string[j + 4] = ebx >> (8 * j);
        string[j + 8] = ecx >> (8 * j);
        string[j + 12] = edx >> (8 * j);
    }
    printf(string);
}
