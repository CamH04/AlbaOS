#ifndef __ALBAOS__CPUID_H
#define __ALBAOS__CPUID_H

#include <common/types.h>



    namespace albaos {

        class  cpuidentif{

            public:

                int cpufind(void);
                int intelcpu(void);
                void printregs(int eax, int ebx, int ecx, int edx);

        };
    }

#endif
