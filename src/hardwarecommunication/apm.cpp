#include <hardwarecommunication/apm.h>

using namespace albaos;
using namespace albaos::common;
using namespace albaos::hardwarecommunication;


void printf(char* str);

bool apm::poweroff(){
        char* power_state = "03h";
        bool APM_error = false;

        asm volatile (
                "movb $0x53, %%ah\n\t"
                "movb $0x07, %%al\n\t"
                "movw $0x0001, %%bx\n\t"
                "movl %1, %%ecx\n\t"
                "int $0x15\n\t"
                "jc APM_error_label\n\t"
                "jmp end_label\n\t"
                "APM_error_label:\n\t"
                "movb $1, %0\n\t"
                "end_label:\n\t"
                : "=r" (APM_error)
                : "r" (power_state)
                : "%ah", "%al", "%bx", "%ecx"
        );
}
