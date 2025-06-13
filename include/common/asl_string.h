#ifndef __ALBAOS__COMMON__ASLSTRING_H
#define __ALBAOS__COMMON__ASLSTRING_H

#include <common/types.h>


namespace albaos
{
    namespace common
    {
        class asl_string{
        public:
            int atoi(char*& str, int base);
        };
    }
}

#endif
