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
            void itoa(int value, char* str, int base);
            void utoa(unsigned int value, char* str, int base);
            int strcmp(const char* str1, const char* str2);
            uint32_t StringToInt(char* args);
            char* IntToString(uint32_t num);
            uint16_t strlen(char* args);



        };
    }
}

#endif
