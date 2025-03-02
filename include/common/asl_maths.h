#ifndef __ALBAOS__COMMON__ASLMATHS_H
#define __ALBAOS__COMMON__ASLMATHS_H

#include <common/types.h>


namespace albaos
{
    namespace common
    {
        class asl_maths{
        public:
            uint16_t hash(char* cmd);
            uint32_t Trollfnv1a(char* str);
            double Random(void);
            uint16_t betterRandom();

        };
    }
}

#endif
